/* 	
 	File: parser.c
	date: 21.03.2021
	author: cbozan 		*/
	
	

/*

----------------------------------------------------------------
-----------------------GRAMMAR RULES----------------------------
----------------------------------------------------------------
----														----
----	<if-sentence> -> if(<condition>)<sentence>			----
----		{else if(<condition>)<sentence>}				----
----		[else <sentence>]								----
----														----
----	<condition> -> <id><op><id>							----
----	<id> -> A|B|C|D										----
----	<op> -> >|<|<=|>=|==								----
----														----
----	<sentence> -> {<id>=<id>}							----
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------

*/


#include <stdio.h>
#include <string.h>

#define MAX_SYMBOL 23			
#define SOURCE_CODE_BUFFER 2048
#define MAX_TOKEN_LENGTH 32
#define MAX_PATH_LENGTH 128
#define MAX_OPERATOR 5
#define MAX_ID 4



//-------------------- Function declaration -----------------------------------

int fileread(FILE *, char *, char *, unsigned int *);
void tokenizer(char [][MAX_TOKEN_LENGTH], char [], const char const *, const int, unsigned int *);
void sourceCodePrint(char *, int );
void tokensPrint(char [][MAX_TOKEN_LENGTH], int );

int _if(char [][MAX_TOKEN_LENGTH], const char *[], const char *[], const int tokenCount, int *imlec);
int condition(char [][MAX_TOKEN_LENGTH], const char *[], const char *[], const int tokenCount, int *imlec);
int idMi(const char token[], const char *id[]);
int isSentence(char [][MAX_TOKEN_LENGTH], const char *[], const char , int *);
int error(const char [], char []);

//------------------------------------------------------------------------------

// error message box
static char err[MAX_TOKEN_LENGTH];

int main( void ){
	
	const char symbolTable[MAX_SYMBOL] = {'+', '-', '*', '/', '%', '=', '!', '>', '<', '&', 
											'|', '(', ')', ';', ',', '"', '[', ']', '{', '}', ' ', '\n', '\t'};
				
	const char *op[MAX_OPERATOR] = {"<", ">", "<=", ">=", "=="};
	const char *id[MAX_ID] = {"A", "B", "C", "D"};
								
	FILE *sourceCodePtr;
	
	char sourceCode[SOURCE_CODE_BUFFER];
	char tokens[SOURCE_CODE_BUFFER][MAX_TOKEN_LENGTH];
	char path[MAX_PATH_LENGTH];
	
	unsigned int count = 0;
	unsigned int tokenCount = 0;
	
	
	printf("Maximum file size (byte) = %d\n", SOURCE_CODE_BUFFER);
	printf("Max path length = %d\n\n", MAX_PATH_LENGTH);
	printf("%s", "path : ");
	scanf("%s", path);
	printf("\n");
	
	int isRead = fileRead(sourceCodePtr, sourceCode, path, &count);

	
	if( !isRead){
		
		puts("File not found or read");

	} else{
		
		tokenizer(tokens, sourceCode, symbolTable, count, &tokenCount);
		tokensPrint(tokens, tokenCount);
		sourceCodePrint(sourceCode, count);
		
		
		// ---------------------------- Parser operations start ---------------------------
			
		unsigned int cursor = 0;
		int durum = 1;
		
		while(cursor < tokenCount && durum){

				if(strcmp(tokens[cursor],"if") == 0){
					
					durum = _if(tokens, op, id, tokenCount, &cursor);
				
				} else{
					
					durum = 0;
					
				}
		}
	
	
		printf("\n\033[94mCompilation results:\033[0m ");
		
		
		if(durum){
			
			printf("\033[32mSuccessful\033[0m");	
			
		} else{
			
			printf("\n\033[33m%d. token bad code..\033[0m", (cursor + 1));			// cursorin gösteridiði (hatalý olan) karakteri yaz.
			
			printf("\nPart up to the wrong code:\n");
			
			unsigned int x;			
			for(x = 0; x < cursor + 1; x++){										// hatalý yazým olan kod'a(cursor'e) kadar yazdýr
				printf("\033[31m%s\033[0m", tokens[x]);
			}
			
			if(err[0] != '\0'){												// eðer hatalý kod 'id' ise id'yi yazdýr.
				printf("\n\033[31mError:\033[0m \033[33m%s\033[0m", err);
			} else{
				printf("\n");
			}
			
		} 
	
	}
	
	return 0;
	
}


int _if(char tokens[][MAX_TOKEN_LENGTH], const char *op[], const char *id[], const int tokenCount, int *cursor){
															
	int durum = 1;												// yazým denetimi yapan nöbetçi deðiþken
	int ifBoolean = 1;											// else* veya (else if)* yazýlabilirliðini kontrol eden deðiþken
	int izin = 1;												// (else if)*  yapýsýnýn if* kýsmýný ayný if bloðunda kullanmak için izin kontrol deðiþkeni
																
	
	while( (*cursor < tokenCount) && ifBoolean && durum){		// cursor sýnýrý aþmýyorsa ve else* ile karþýlaþýlmamýþsa;
		
		if((strcmp(tokens[*cursor], "if") == 0) && izin){		// daha önce if* girilmediyse(izin != 0) ve token if ise;
			izin = 0;											// if giriþi olduðunu belirt 
			
			(*cursor)++;											// cursor'i bir sonraki kelimeye kaydýr
			
			durum = condition(tokens, op, id, tokenCount, cursor);	// condition fonksiyonu için denetleme yap ve sonucu durum'a döndür
			
			if(durum)											// yazým hatasý yok ise;
				durum = isSentence(tokens, id, tokenCount, cursor);	// cümle fonksiyonu için denetleme yap ve sonucu durum'a döndür
			
			
		} else if( (strcmp(tokens[*cursor], "else") == 0) && (strcmp(tokens[*cursor + 1], "if") == 0) && ifBoolean){ // else if ise;
			
			izin = 1;											// tekrar if giriþini yetkilendir
			(*cursor)++;											// cursor'i bir sonraki kelimeye kaydýr
			
		} else if((strcmp(tokens[*cursor], "else") == 0) && ifBoolean){ // else* ise;
			
			ifBoolean = 0;										// while döngüsünün çalýþmasýný sonlandýr
			(*cursor)++;											// cursor'i bir sonraki kelimeye kaydýr										
			
			durum = isSentence(tokens, id, tokenCount, cursor);		// cümle fonksiyonu için denetleme yap ve sonucu durum'a döndür
			
		} else {												
			return durum;
		}
		
	}
	
	return durum;
}


int condition(char tokens[][MAX_TOKEN_LENGTH], const char *op[], const char *id[], const int tokenCount, int *cursor){
	
	int durum = 0;												// yazým denetimi yapan nöbetçi deðiþken
	unsigned int i;												// döngü için deðiþken
	
	if((strcmp(tokens[*cursor], "(") == 0)){						// eðer kelime parantez ise;

		
		*cursor = *cursor + 1;									// cursor'i bir sonraki kelimeye kaydýr
		
		if(!isId(tokens[*cursor], id)){							// kelimeyi isId fonksiyonuna gönder id deðilse;
	
			return 0;
			
		} else{													// id ise;
		
			*cursor = *cursor + 1;								// cursor'i bir sonraki kelimeye kaydýr
			
			for(i = 0; i < MAX_OPERATOR; i++){					// mevcut kelimeyi tüm operatörlerle karþýlaþtýran döngü
				
				if((strcmp(tokens[*cursor], op[i]) == 0)){		// kelime operatör ise;
					
					*cursor = *cursor + 1;						// cursor'i bir sonraki kelimeye kaydýr
					durum = 1;									// operatör'de hata olmadýðýný belirt
					
				}
			}
			
			if(!durum){
				
				return error(tokens[*cursor], " operator error");
				
			}
			
			if(!isId(tokens[*cursor], id)){			// kelimeyi isId fonksiyonuna gönder sonuc 0 veya durum 0 ise;
				
				return 0;
		
			} else{												// sorun yok ise;
				
				*cursor = *cursor + 1;							// cursor'i bir sonraki kelimeye kaydýr
			}
				
					
		}
		
		if( !(strcmp(tokens[*cursor], ")") == 0)){				//  eðer parantez deðilse;
			
			return error(tokens[*cursor], " condition parenthesis error");
		}
		*cursor = *cursor + 1;									// cursor'i bir sonraki kelimeye kaydýr
		
		return 1;												// program buraya kadar geldiyse sorun yok 1 döndür
	} else{
		
		return error(tokens[*cursor], " condition parenthesis error");
		
	}
	
	return 0;													// if durumu gerçekleþmediyse sýfýr döndür
}

int isId(const char token[], const char *id[]){
	
	unsigned int i;												// döngü için yerel bir deðiþken
	
	for(i = 0; i < MAX_ID; i++){								// tüm 'id'leri karþýlaþtýrmak için dön
		if(strcmp(token, id[i]) == 0){							// id ise;
			return 1;
		}
	}
	

	
	return error(token, " is not id");
}


int isSentence(char tokens[][MAX_TOKEN_LENGTH], const char *id[], const char tokenCount, int *cursor){
	
	
	// Cümle yapýsýnýn yazým denetimini yapan fonksiyon:
	
	if(strcmp(tokens[*cursor], "{") == 0){
		*cursor += 1;
		if(!isId(tokens[*cursor], id)){
	
			return 0;
			
		} 
		
		*cursor += 1;
			
		if(!(strcmp(tokens[*cursor], "=") == 0)){
			return error(tokens[*cursor], " arithmetic operation error in sentence");
		}
	
		*cursor += 1;
		
		if(!isId(tokens[*cursor], id)){
			return 0;
		}
		
		*cursor += 1;
		
	} else{
		return error(tokens[*cursor], " sentence parenthesis error");
	}
	
	if(!(strcmp(tokens[*cursor],"}") == 0)){
		return error(tokens[*cursor], " sentence parenthesis error");
	}
	
	*cursor += 1;
	
	return 1;
	
}



int error(const char token[], char hata[]){
	
	size_t i = 0, j = 0;
	
	while(token[i] != '\0'){
		err[i] = token[i];
		i++;
	}
	
	
	while(hata[j] != '\0'){
		err[i] = hata[j];
		i++;
		j++;
	}
	
	return 0;
	
}
int fileRead(FILE *sourceCodePtr, char *sourceCode, char *path, unsigned int *count){
	
	if( (sourceCodePtr = fopen(path, "r")) == NULL){ 						// dosyayý okuma modunda ac null'a eþitse
	
		return 0;
		
	} else{ 															    // null'a eþit deðilse
		
		do{
			// dosyadan karakter oku ve karakter dizisi tutan pointer'a (string dizisine) ekle  ve sayacý(count) arttýr
			sourceCode[*count] = fgetc(sourceCodePtr);
			*count = *count + 1; 
				
		} while( !feof(sourceCodePtr) && (*count < SOURCE_CODE_BUFFER - 1) ); 	// "EOF" satýr sonu karakteri okuyana kadar devam et
		
		fclose(sourceCodePtr); 											   	// dosyayi kapat 
		
		sourceCode[*count] = '\0';											// cümle sonu karakteri ekle
		
	}

	return 1;
}



void tokenizer(char tokens [][MAX_TOKEN_LENGTH] , char sourceCode [] , const char const *symbolTable, const int count, unsigned int *tokenCount){
		
		/**
		* tokenCount : tokens'ýn mevcut eleman sayisini gösterir
		* range : hedef kelimenin uzunluðunu gösterir
		* hold : iki sembol arasýndaki kelime'in ilk karakterinin indisini tutar.
		* i, j, tempCount : döngü elemanlarý
		*/
		unsigned int i, j, tempCount, range, hold = 0;
		
		
		// karakter sayisi kadar dön
		for(i = 0; i < count; i++){ 
		
			// symbolTable'daki her bir karakter için dön
			for(j = 0; j < MAX_SYMBOL; j++){ 
			
				// kaynak kode karakteri bir sembol ise
				if( sourceCode[i] == symbolTable[j]){ 

					

					// tespit edilen sembolün, kaynak kodda karþýlýk gelen indisinden tutulan(hold deðiþkeni) indisi çýkar
					range = i - hold; 
					
				
					
					// tespit edilen stringin her bir karakteri için dön ve tokens'e ekle
					for(tempCount = 0; tempCount < range; tempCount++){
						
						tokens[*tokenCount][tempCount] = sourceCode[hold];
						hold++;
						
					}
					
					if(tempCount != 0) 					// iki sembol ardýþýk ise bu durum gerçekleþir. (i = hold)
						*tokenCount = *tokenCount + 1;
						
					if( sourceCode[hold] != ' '  		// whitespace deðilse
						&& sourceCode[hold] != '\t' 	// tabspace deðilse
						&& sourceCode[hold] != '\n'){  	// newline deðilse
					
						if( (sourceCode[i] == '=' || sourceCode[i] == '<' || sourceCode[i] == '>') 
						&& (i < count - 1) && sourceCode[i + 1]== '='){
							
							tokens[*tokenCount][0] = sourceCode[hold];
							hold = hold + 1; 
							
							tokens[*tokenCount][1] = sourceCode[hold];
							i = i + 1;
							
							
						} else{
							tokens[*tokenCount][0] = sourceCode[hold]; 
						}
						
						*tokenCount = *tokenCount + 1;
						
						
						
					} // if sonu
					
					hold++;
					
				} // if sonu
				
			}// 2.for sonu
			
		} // 1.for sonu
}


void sourceCodePrint(char *sourceCode, int size){
	
	// kaynak kodu yaz
	printf("\n%80s\n", "--------------------------------- SOURCE CODE ---------------------------------");
	printf("%s", sourceCode);
	
	
	
}

void tokensPrint(char tokens[][MAX_TOKEN_LENGTH], int size){
	
	// kelimeleþtirilmiþ kaynak kodu yaz
		printf("\n%80s\n", "--------------------------------- TOKENIZER ---------------------------------");
		puts("{\n");
		size_t i = 0;
		for(i = 0; i < size; i++)
			printf("%s,", tokens[i]);
		printf("\n}");
		
}






