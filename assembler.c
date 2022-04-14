/* 	
 	File: assembler.c
	date: 14.04.2021
	author: cbozan 		*/





/*

----------------------------------------------------------------
---------------INSTRUCTION AND REGISTER SET --------------------
----------------------------------------------------------------

REGISTERS
AX, BX, CX, DX

INSTRUCTION SET

Move
HRK X1,X2 -> X1=X2

Arithmetic
TOP X1,X2 -> X1=X1+X2
CRP X1,X2 -> X1=X1*X2
CIK X1,X2 -> X1=X1-X2
BOL X1,X2 -> X1=X1/X2 and DX=X1%X2

Logical
VE X1,X2 -> X1=X1 AND X2
VEYA X1,X2 -> X1=X1 OR X2
DEG X1 -> X1= NOT X1 (X1')

Branch
SS ADDRESS -> if the result of previous operation is zero then branch to ADDRESS
SSD ADRESS -> if the result of previous operation is not zero then branch to ADDRESS
SN ADDRESS -> if the result of previous operation is negative the branch to ADDRESS
SP ADDRESS -> if the result of previous operation is positive the branch to ADDRESS

----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------

*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define RAM_BELLEK_BOYUT 2048 
#define DOSYA_YOL_UZUNLUK 64
#define SEMBOL_BOYUT 3
#define PROGRAM_BELLEK_BOYUT 256
#define ETIKET_BELLEK_BOYUT 64
#define KOMUT_SAYISI 12
#define KAYDEDICI_SAYISI 4
#define STATUS_BOYUT 7

//*********************** STATÝC TANIMLAMALAR **************************//

// RAM Belleði
static short RAM[RAM_BELLEK_BOYUT] = {0}; 

// IR : Komut Belleði
static char *IR[PROGRAM_BELLEK_BOYUT];	

// Komutlar
static char const *KOMUT[KOMUT_SAYISI] = {"HRK", "TOP", "CRP", "CIK", "BOL", "VE", 
										"VEYA", "DEG", "SS", "SSD", "SN", "SP"};

// Tanýmlanmýþ etiketleri tutar
static char *ETIKET_BELLEK[ETIKET_BELLEK_BOYUT]; 

// Etiketin Komut belleðini(IR) tutar.
static short ETIKET_ADRES[ETIKET_BELLEK_BOYUT] = {0};

// Özel iþlem bitleri (taþma, sýfýr sonuc ve adres tipi gibi bitleri tutar)
static bool STATUS[STATUS_BOYUT] = {0};

// Etiket Belleði Sayacý
static short int etiketSayac = 0;

// Komut belleði(IR) sayacý			
static short tokenSayac = 0;	

// Kaydediciler
static const char *KAYDEDICI[KAYDEDICI_SAYISI] = {"AX", "BX", "CX", "DX"};

// AX Kaydedicisi
static short *AX;  

//BX Kaydedicisi
static short *BX;

// CX Kaydedicisi
static short *CX;

// DX Kaydedicisi
static short *DX;

// hemen adreslemede veriyi tutar
static short *veri;

// boþluk karakteri
static const char SPACE = ' ';		

// Sýradaki iþlenecek komutun Komut belleðindeki(IR) adresini tutar.(Program Sayýcý)
static int PS = 0; 	


// *****************************  FONKSÝYON PROTOTÝPLERÝ  ************************ //

void tokenizer(char *, int);

void assembler();
int getKomutTipi(char [], short);
int getAdresTipi(char [], short);
int getRamAdres(char [], short);
short getHemenAdres(char [], short);
int getEtiketAdres(char komut[]);

short* parse(int, int);
void ALU(int []);
void yazdir(char *);

void hata(const char *, char []);


// ****************************  MAIN FONKSÝYONU  ********************************* //
int main( void ){
	
	AX = (short *)malloc(sizeof(short));
	BX = (short *)malloc(sizeof(short));
	CX= (short *)malloc(sizeof(short));
	DX = (short *)malloc(sizeof(short)); 
	veri = (short *)malloc(sizeof(short));
	
	*BX = 0; *CX = 0; *DX = 0; *AX = 0; *veri = 0;
	
	FILE *kaynakKodDosya = NULL;		// kaynak kod dosyasý
	char *kaynakKod = NULL;				// kaynak kod dosyasýnýn içeriðini tutar
	char dosyaYolu[DOSYA_YOL_UZUNLUK];	// kaynak kod dosya yolunu tutar
	size_t sayac;						// kaynak kod'un karakter sayýsýný tutar
	
	printf("\n%76s\n\n", "----------------------- assembler.c -----------------------------");
	printf("%s", "path : ");				
	scanf("%s", dosyaYolu);
	printf("\n");
	
	
	if( (kaynakKodDosya = fopen(dosyaYolu, "r")) == NULL){
		
		puts("File not found or read");
		
	} else{
		
		// dosyanýn sonuna git
		fseek(kaynakKodDosya, 0L, SEEK_END);
		
		// dosyanýn karakter sayýsýný döndür	
		sayac = ftell(kaynakKodDosya);
		
		// dosya baþlangýcýna git
		rewind(kaynakKodDosya);
		
		kaynakKod = (char*)malloc(sayac * sizeof(char) );
		
		sayac = 0;
		do{
			
			*(kaynakKod + sayac) = fgetc(kaynakKodDosya);
			sayac++;
				
		} while( !feof(kaynakKodDosya) );
		
		fclose(kaynakKodDosya); 			 
		
		*(kaynakKod + sayac) = '\0';
		tokenizer(kaynakKod, sayac);
		assembler();
		
	
	}
	
	yazdir(kaynakKod); 
	free(kaynakKod);
	 
	printf("\n\nThe program has been terminated... Press any key to exit.");
	getch();
	return 0;
}
  
void assembler(){
	
	int buyruk[3];
	short buyrukSayac;
	
	PS = 0;
	while(PS < tokenSayac){ // Program sayýcý sonlanana kadar devam et
		
		buyrukSayac = 0;
		
		while(PS < tokenSayac && (int)IR[PS][0] != 10 ){
			
			
			buyruk[buyrukSayac] = getKomutTipi(IR[PS], buyrukSayac);
			buyrukSayac++;
			PS++;
		}
		
		PS++;
		
		ALU(buyruk); // Arithmetich Logic Unit
		
		
	}
	
}


int getKomutTipi(char komut[], short buyrukSayac){
	
	short indis;
	switch(buyrukSayac){
			
		case 0:

			indis = 0;
			while(indis < KOMUT_SAYISI){
				if(strcmp(komut, KOMUT[indis]) == 0)
					return indis;
				
				indis++;
			}
			
			hata("Undefined command", komut);
			
		case 1:
			return getAdresTipi(komut, buyrukSayac);
		case 2:
			return getAdresTipi(komut, buyrukSayac);
		default:
			hata("command not processed", komut);	
			
	}
}

int getAdresTipi(char komut[], short buyrukSayac){
	
	size_t i = 0;
	while(i < KAYDEDICI_SAYISI){
		if(strcmp(komut, KAYDEDICI[i]) == 0){
                   									
			STATUS[buyrukSayac * 2 + 1] = 0;
			STATUS[buyrukSayac * 2] = 1;

			return i;
		}
		i++;
	}
	
	if( komut[0] == '[')
		return getRamAdres(komut, buyrukSayac);
	else if((int)komut[0] >= 48 && (int)komut[0] <= 57){
		return getHemenAdres(komut, buyrukSayac);
	} else{
		return getEtiketAdres(komut);
	}
		
	
	
}



int getRamAdres(char komut[], short buyrukSayac){
	
	int i;
	int adres = 0;
	
	if(komut[1] == '['){ // dolayli ram adresleme [[adres]]
		i = 2;
		
		STATUS[buyrukSayac * 2 + 1] = 1;
		STATUS[buyrukSayac * 2] = 1;
		
	}else{ 				// doðrudan ram adresleme [adres]
		i = 1;
		
		STATUS[buyrukSayac * 2 + 1] = 1; 		
		STATUS[buyrukSayac * 2] = 0; 
	}
	
	while(komut[i] != '\0' && komut[i] != ']'){
		
		if((int)komut[i] < 48 || (int)komut[i] > 57)
			hata("a non-digit character in the addressing.", komut); 
			
		adres = adres * 10 + ( (int)komut[i] - 48 );
		
		++i;
	}
	
	if(adres >= RAM_BELLEK_BOYUT || adres < 0)
		hata("addressing out of ram size", komut);
	
	return adres;
}



short getHemenAdres(char komut[], short buyrukSayac){
	
	short i = 0;
	short veri = 0;
	
	while(komut[i] != EOF && komut[i] != '\0'){
		
		if((int)komut[i] < 48 || (int)komut[i] > 57)
			hata("a non-digit character in the addressing.", komut);
			
		veri = veri * 10 + ( (int)komut[i] - 48 );
		i++;	
	}
	
	STATUS[buyrukSayac * 2 + 1] = 0;
	STATUS[buyrukSayac * 2] = 0;
	
	return veri;
}

int getEtiketAdres(char komut[]){
	
	size_t i = 0;
	for(i = 0; i < etiketSayac; i++){
		
		if(strcmp(komut, ETIKET_BELLEK[i]) == 0) 
			return i;
	}
	
	hata("Tanimlanmayan bir etiket icin dallanma komutu", komut);
}

short* parse(int buyruk, int buyrukSayac){
	
	short decimal = 1 * (int)STATUS[buyrukSayac * 2] + 2 * (int)STATUS[buyrukSayac * 2 + 1];
	char hataKodu[] = "";
	
	switch(decimal){
		
		// Hemen adresleme - immediate addressing
		case 0:	
			
			*veri = buyruk;
			return veri;
			
		// Register addressing
		case 1:
			
			switch(buyruk){
				case 0:
					
					return AX;
					
				case 1:
					
					return BX;
				
				case 2:
				
					return CX;
				
				case 3:
				
					return DX;
				
				default:
				
					hata("register error", hataKodu);
			}
			
		// Doðrudan adresleme - direct addressing
		case 2:
			
			return &RAM[buyruk];
			
		// Dolaylý adresleme - indirect addressing 
		case 3:
			
			if(RAM[buyruk] >= RAM_BELLEK_BOYUT || RAM[buyruk] < 0)
				hata("addressing out of ram size", hataKodu);
			
			return &RAM[RAM[buyruk]];
			
		default:
			hata("unknown error", hataKodu);
			
	}
		
}


// Arithmetic Logic Unit
void ALU(int buyruk[]){
	
	short *veriPtr1 = (short *)malloc(sizeof(short));
	short *veriPtr2;
	
	if(buyruk[0] < 8){ // dallanma komutu deðilse ayrýþtýr - if not branch command
		
		veriPtr1 = parse(buyruk[1], 1);
		veriPtr2 = parse(buyruk[2], 2);
	}
	
	int temp1 = 0, temp2 = 0;
	size_t i;
	
	switch(buyruk[0]){
		
		
		case 0:	// HRK KOMUT ÝÞLEMLERÝ - Move
			
			*veriPtr1 = *veriPtr2;
			
			break;

		case 1:	// TOP KOMUT ÝÞLEMLERÝ - sum
			
			temp1 = (*veriPtr1) + (*veriPtr2); 
			
			if(temp1 > 255){
				STATUS[0] = 1;
				temp1 = temp1 % 256;
			}
			
			*veriPtr1 = temp1;
			
			break;
			
		case 2: // CRP KOMUT ÝÞLEMLERÝ - multiply

			temp1 = (*veriPtr1) * (*veriPtr2); 
			
			if(temp1 > 255){
				STATUS[0] = 1;
				temp1 = temp1 % 256;
			}
			
			*veriPtr1 = temp1;
			
			break;	
						 
		case 3: // CIK KOMUT ÝÞLEMLERÝ - substraction

			*veriPtr1 = (*veriPtr1) - (*veriPtr2); 
			
			break;
			
		case 4: // BOL KOMUT ÝÞLEMLERÝ - division
			
			if(*veriPtr2 != 0)
				*veriPtr1 = (short)( (*veriPtr1) / (*veriPtr2) ); 
			
			break;
			
		case 5:	// VE KOMUT ÝÞLEMLERÝ - and logic
			
			temp1 = *veriPtr1;
			temp2 = *veriPtr2;
			*veriPtr1 = 0;
			
			i = 0;
			while(i < 8){
				*veriPtr1 += (temp1 % 2) * (temp2 % 2) * pow(2, i);
				temp1 /= 2;
				temp2 /= 2;
				i++;
			}
			
			break;
			
		case 6:	// VEYA KOMUT ÝÞLEMLERÝ - or logic

			temp1 = *veriPtr1;
			temp2 = *veriPtr2;
			*veriPtr1 = 0;
			
			i = 0;
			while(i < 8){
				*veriPtr1 += ( ( (temp1 % 2) * (temp2 % 2) + (temp1 % 2) + (temp2 % 2) ) % 2) * pow(2, i); 
				temp1 /= 2;
				temp2 /= 2;
				i++;
			}
			
			break;
			
		case 7: // DEG KOMUT ÝÞLEMLERÝ - not logic

			*veriPtr1 = 255 - *veriPtr1;
			
			break;
			
		case 8: // SS KOMUT ÝÞLEMLERÝ - branch if zero
			
			if(STATUS[1]){
				PS = ETIKET_ADRES[buyruk[1]];
			}
			
			break;
			
		case 9:	// SSD KOMUT ÝÞLEMLERÝ - branch if not zero
			
			if(!STATUS[1]){
				PS = ETIKET_ADRES[buyruk[1]];
			}
			
			break;
			
		case 10:// SSN KOMUT ÝÞLEMLERÝ - branch if negative
		
			if(STATUS[6]){
				PS = ETIKET_ADRES[buyruk[1]];	
			}
			
			break;
			
		case 11:// SSP KOMUT ÝÞLEMLERÝ - branch if positive
			
			if(!STATUS[6]){
				PS = ETIKET_ADRES[buyruk[1]];
			}
			
			break;
		default:
			break;
			
	}
	
	// Aritmetik iþlemin sonucunu þartlý dallanma için STATUS[6] ' da belirt
	
	//if(buyruk[0] < 8){ // dallanma komutu deðilse *****Ana programda eksik
		
		if(*veriPtr1 == 0){
			
			STATUS[1] = 1;
			
		} else if(*veriPtr1 < 0){
			
			STATUS[1] = 0;
			STATUS[6] = 1;
			
		} else{				// *veriPtr1>0
		
			STATUS[1] = 0;
			STATUS[6] = 0;
		}
//	}
	
}


// move tokenizer.c
void tokenizer(char *kaynakKod, int sayac){
	
	size_t i, j, indis, temp = 0;
	
	for(i = 0; i < sayac; i++){
		
		if( (kaynakKod[i] == SPACE || kaynakKod[i] == ',' || 
								(int)kaynakKod[i] == 10) || kaynakKod[i] == EOF || kaynakKod[i] == '\t'){
			if( i != temp){
				
				indis = 0;
				
				IR[tokenSayac] = (char*)malloc((i - temp) * sizeof(char));
				
				while(temp < i){
					
					IR[tokenSayac][indis] = kaynakKod[temp];
					
					temp++;
					++indis;
					
				}
				
				IR[tokenSayac][indis] = '\0';
				
				tokenSayac++;
				
				if((int)kaynakKod[i] == 10){
				
					IR[tokenSayac] = (char *)malloc(sizeof(char) * 1);
					
					IR[tokenSayac][0] = kaynakKod[i];
					IR[tokenSayac][1] = '\0';
					
					tokenSayac++;
					PS++;
				}
				
				temp++;
				
			} else {
				++temp;
			}
			
		}else if( kaynakKod[i] == ':' && i != temp){
			
			indis = 0;
			
			ETIKET_BELLEK[etiketSayac] = (char *)malloc((i - temp) * sizeof(char));
			
			while(temp < i){
				
				if(kaynakKod[temp] != SPACE){
					
					ETIKET_BELLEK[etiketSayac][indis] = kaynakKod[temp];
					indis++;
				}
				
				temp++;
				
			}
			
			ETIKET_BELLEK[etiketSayac][indis] = '\0';
			
			ETIKET_ADRES[etiketSayac] = tokenSayac;
			++etiketSayac;
			++temp;
			
			
		}
		
	}

}

void yazdir(char *kaynakKod){
	printf("\nSource Code:\n");
	printf("%s", kaynakKod);
	printf("\n\nAX = %d, BX = %d, CX = %d, DX = %d", *AX, *BX, *CX, *DX);
	
}

void hata(const char *hata, char hataKodu[]){
	
	printf("\n!!! Error source code !!!\n\n");
	printf("'%s'%s\n", hataKodu, hata);
	
	exit(-1); 
	
}


