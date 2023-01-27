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
			
			printf("\n\033[33m%d. token bad code..\033[0m", (cursor + 1));			// cursorin g�steridi�i (hatal� olan) karakteri yaz.
			
			printf("\nPart up to the wrong code:\n");
			
			unsigned int x;			
			for(x = 0; x < cursor + 1; x++){										// hatal� yaz�m olan kod'a(cursor'e) kadar yazd�r
				printf("\033[31m%s\033[0m", tokens[x]);
			}
			
			if(err[0] != '\0'){												// e�er hatal� kod 'id' ise id'yi yazd�r.
				printf("\n\033[31mError:\033[0m \033[33m%s\033[0m", err);
			} else{
				printf("\n");
			}
			
		} 
	
	}
	
	return 0;
	
}


int _if(char tokens[][MAX_TOKEN_LENGTH], const char *op[], const char *id[], const int tokenCount, int *cursor){
															
	int durum = 1;												// yaz�m denetimi yapan n�bet�i de�i�ken
	int ifBoolean = 1;											// else* veya (else if)* yaz�labilirli�ini kontrol eden de�i�ken
	int izin = 1;												// (else if)*  yap�s�n�n if* k�sm�n� ayn� if blo�unda kullanmak i�in izin kontrol de�i�keni
																
	
	while( (*cursor < tokenCount) && ifBoolean && durum){		// cursor s�n�r� a�m�yorsa ve else* ile kar��la��lmam��sa;
		
		if((strcmp(tokens[*cursor], "if") == 0) && izin){		// daha �nce if* girilmediyse(izin != 0) ve token if ise;
			izin = 0;											// if giri�i oldu�unu belirt 
			
			(*cursor)++;											// cursor'i bir sonraki kelimeye kayd�r
			
			durum = condition(tokens, op, id, tokenCount, cursor);	// condition fonksiyonu i�in denetleme yap ve sonucu durum'a d�nd�r
			
			if(durum)											// yaz�m hatas� yok ise;
				durum = isSentence(tokens, id, tokenCount, cursor);	// c�mle fonksiyonu i�in denetleme yap ve sonucu durum'a d�nd�r
			
			
		} else if( (strcmp(tokens[*cursor], "else") == 0) && (strcmp(tokens[*cursor + 1], "if") == 0) && ifBoolean){ // else if ise;
			
			izin = 1;											// tekrar if giri�ini yetkilendir
			(*cursor)++;											// cursor'i bir sonraki kelimeye kayd�r
			
		} else if((strcmp(tokens[*cursor], "else") == 0) && ifBoolean){ // else* ise;
			
			ifBoolean = 0;										// while d�ng�s�n�n �al��mas�n� sonland�r
			(*cursor)++;											// cursor'i bir sonraki kelimeye kayd�r										
			
			durum = isSentence(tokens, id, tokenCount, cursor);		// c�mle fonksiyonu i�in denetleme yap ve sonucu durum'a d�nd�r
			
		} else {												
			return durum;
		}
		
	}
	
	return durum;
}


int condition(char tokens[][MAX_TOKEN_LENGTH], const char *op[], const char *id[], const int tokenCount, int *cursor){
	
	int durum = 0;												// yaz�m denetimi yapan n�bet�i de�i�ken
	unsigned int i;												// d�ng� i�in de�i�ken
	
	if((strcmp(tokens[*cursor], "(") == 0)){						// e�er kelime parantez ise;

		
		*cursor = *cursor + 1;									// cursor'i bir sonraki kelimeye kayd�r
		
		if(!isId(tokens[*cursor], id)){							// kelimeyi isId fonksiyonuna g�nder id de�ilse;
	
			return 0;
			
		} else{													// id ise;
		
			*cursor = *cursor + 1;								// cursor'i bir sonraki kelimeye kayd�r
			
			for(i = 0; i < MAX_OPERATOR; i++){					// mevcut kelimeyi t�m operat�rlerle kar��la�t�ran d�ng�
				
				if((strcmp(tokens[*cursor], op[i]) == 0)){		// kelime operat�r ise;
					
					*cursor = *cursor + 1;						// cursor'i bir sonraki kelimeye kayd�r
					durum = 1;									// operat�r'de hata olmad���n� belirt
					
				}
			}
			
			if(!durum){
				
				return error(tokens[*cursor], " operator error");
				
			}
			
			if(!isId(tokens[*cursor], id)){			// kelimeyi isId fonksiyonuna g�nder sonuc 0 veya durum 0 ise;
				
				return 0;
		
			} else{												// sorun yok ise;
				
				*cursor = *cursor + 1;							// cursor'i bir sonraki kelimeye kayd�r
			}
				
					
		}
		
		if( !(strcmp(tokens[*cursor], ")") == 0)){				//  e�er parantez de�ilse;
			
			return error(tokens[*cursor], " condition parenthesis error");
		}
		*cursor = *cursor + 1;									// cursor'i bir sonraki kelimeye kayd�r
		
		return 1;												// program buraya kadar geldiyse sorun yok 1 d�nd�r
	} else{
		
		return error(tokens[*cursor], " condition parenthesis error");
		
	}
	
	return 0;													// if durumu ger�ekle�mediyse s�f�r d�nd�r
}

int isId(const char token[], const char *id[]){
	
	unsigned int i;												// d�ng� i�in yerel bir de�i�ken
	
	for(i = 0; i < MAX_ID; i++){								// t�m 'id'leri kar��la�t�rmak i�in d�n
		if(strcmp(token, id[i]) == 0){							// id ise;
			return 1;
		}
	}
	

	
	return error(token, " is not id");
}


int isSentence(char tokens[][MAX_TOKEN_LENGTH], const char *id[], const char tokenCount, int *cursor){
	
	
	// C�mle yap�s�n�n yaz�m denetimini yapan fonksiyon:
	
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
	
	if( (sourceCodePtr = fopen(path, "r")) == NULL){ 						// dosyay� okuma modunda ac null'a e�itse
	
		return 0;
		
	} else{ 															    // null'a e�it de�ilse
		
		do{
			// dosyadan karakter oku ve karakter dizisi tutan pointer'a (string dizisine) ekle  ve sayac�(count) artt�r
			sourceCode[*count] = fgetc(sourceCodePtr);
			*count = *count + 1; 
				
		} while( !feof(sourceCodePtr) && (*count < SOURCE_CODE_BUFFER - 1) ); 	// "EOF" sat�r sonu karakteri okuyana kadar devam et
		
		fclose(sourceCodePtr); 											   	// dosyayi kapat 
		
		sourceCode[*count] = '\0';											// c�mle sonu karakteri ekle
		
	}

	return 1;
}



void tokenizer(char tokens [][MAX_TOKEN_LENGTH] , char sourceCode [] , const char const *symbolTable, const int count, unsigned int *tokenCount){
		
		/**
		* tokenCount : tokens'�n mevcut eleman sayisini g�sterir
		* range : hedef kelimenin uzunlu�unu g�sterir
		* hold : iki sembol aras�ndaki kelime'in ilk karakterinin indisini tutar.
		* i, j, tempCount : d�ng� elemanlar�
		*/
		unsigned int i, j, tempCount, range, hold = 0;
		
		
		// karakter sayisi kadar d�n
		for(i = 0; i < count; i++){ 
		
			// symbolTable'daki her bir karakter i�in d�n
			for(j = 0; j < MAX_SYMBOL; j++){ 
			
				// kaynak kode karakteri bir sembol ise
				if( sourceCode[i] == symbolTable[j]){ 

					

					// tespit edilen sembol�n, kaynak kodda kar��l�k gelen indisinden tutulan(hold de�i�keni) indisi ��kar
					range = i - hold; 
					
				
					
					// tespit edilen stringin her bir karakteri i�in d�n ve tokens'e ekle
					for(tempCount = 0; tempCount < range; tempCount++){
						
						tokens[*tokenCount][tempCount] = sourceCode[hold];
						hold++;
						
					}
					
					if(tempCount != 0) 					// iki sembol ard���k ise bu durum ger�ekle�ir. (i = hold)
						*tokenCount = *tokenCount + 1;
						
					if( sourceCode[hold] != ' '  		// whitespace de�ilse
						&& sourceCode[hold] != '\t' 	// tabspace de�ilse
						&& sourceCode[hold] != '\n'){  	// newline de�ilse
					
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
	
	// kelimele�tirilmi� kaynak kodu yaz
		printf("\n%80s\n", "--------------------------------- TOKENIZER ---------------------------------");
		puts("{\n");
		size_t i = 0;
		for(i = 0; i < size; i++)
			printf("%s,", tokens[i]);
		printf("\n}");
		
}






