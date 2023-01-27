#include <stdio.h>
#include "tokenizer.h"

int main( void ){
	
	char **ptr = tokenizer();
	
	printf("\n%80s\n", "--------------------------------- TOKENIZED SOURCE CODE ---------------------------------");
	puts("{\n");
	int i = 0;
	while(ptr[i][0] != '\0'){
		printf("%s,", ptr[i]);
		++i;
	}
	
	puts("\n");
	
	return 0;
	
}
