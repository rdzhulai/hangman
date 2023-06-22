#include "hangman.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv){
	
	srand(time(NULL));
	char secret[16];
	get_word(secret);

	hangman(secret);
	

	return 0;
}
