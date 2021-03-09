/*
	file: tokenizer.c 
	date: 09.03.2021 
	author: cbozan
*/

#include <stdio.h>

#define MAX_SYMBOL 23 
#define SOURCE_CODE_BUFFER 2048 
#define MAX_TOKEN_LENGTH 32 
#define MAX_PATH_LENGTH 128


int main( void ){
	
	const char symbolTable[MAX_SYMBOL] = {'+', '-', '*', '/', '%', '=', '!', '>', '<', '&', 
											'|', '(', ')', ';', ',', '"', '[', ']', '{', '}', ' ', '\n', '\t'};
	
	// source code character array
	char sourceCode[SOURCE_CODE_BUFFER];
	
	// holds lexical strings
	char tokenHolder[SOURCE_CODE_BUFFER][MAX_TOKEN_LENGTH];
	
	unsigned int cursor = 0;
	char path[MAX_PATH_LENGTH];
	
	printf("Maximum file size (byte) = %d\n", SOURCE_CODE_BUFFER);
	printf("Max path length = %d\n\n", MAX_PATH_LENGTH);
	printf("%s", "path : ");
	scanf("%s", path);
	printf("\n");
	
	
	// File Operations
	
	FILE *sourceCodePtr;
	if( (sourceCodePtr = fopen(path, "r")) == NULL){
		
		puts("File not found or read");
		
	} else{
		
		do{
			
			sourceCode[cursor] = fgetc(sourceCodePtr);
			cursor++;
			
		} while( !feof(sourceCodePtr) );
		
		fclose(sourceCodePtr);
		sourceCode[cursor] = '\0';
		
		
		// Tokenizer operations
		
		unsigned int i, j, tempCount, range;
		unsigned int head = 0, tokenHolderCursor = 0;
		
		// browse the source code character by character
		for(i = 0; i < cursor; i++){
			
			// loop to compare with each item in symbolTable
			for(j = 0; j < MAX_SYMBOL; j++){
				
				if( sourceCode[i] == symbolTable[j]){
					
					range = i - head;
					
					// characters in the target range constitute a token
					for( tempCount = 0; tempCount < range; tempCount++){
						
						tokenHolder[tokenHolderCursor][tempCount] = sourceCode[head];
						head++;
						
					}
					
					// (i = head) example: ++, += 
					if( tempCount != 0)
						tokenHolderCursor++;
					
					if( sourceCode[head] != ' '
						&& sourceCode[head] != '\t'
						&& sourceCode[head] != '\n'){
						
						tokenHolder[tokenHolderCursor][0] = sourceCode[head];
						tokenHolderCursor++;
								
					}
					
					++head;
				}
				
			}
			
		}
		
		
		printf("\n\n%80s\n\n", "--------------------------------- TOKENIZED SOURCE CODE ---------------------------------");
		puts("{\n");
		for(i = 0; i < tokenHolderCursor; i++)
			printf("%s,", tokenHolder[i]);
		printf("\n\n}");
			
		printf("\n\n%80s\n\n", "--------------------------------- SOURCE CODE ---------------------------------");
		printf("%s", sourceCode);
		
	}
	
	getch();
	return 0;
}
