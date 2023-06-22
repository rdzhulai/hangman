#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "hangman.h"
#include <stdbool.h>
#include <stdbool.h>
#include <errno.h>

bool is_in_str(const char *str, const char c){
        // The function returns True if 'c' is in 'str'.
        return (strchr(str, c) != NULL);
}

void exreps(const char *str, char **result_str){
        // The function excludes repeated characters from 'str' and saves that string  in 'result_str'.
        if (str == NULL) return;
        int slen = strlen(str);
	
        int ts_size = 1;
        errno = 0;
        char *temp_str = malloc(ts_size * sizeof(char));
        if(errno){
                fprintf(stderr, "Error allocating memmory: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        temp_str[0] = '\0';

        for(int i = 0; i < slen; i++){

                if(!is_in_str(temp_str, str[i])){
			char *tp;
                        errno = 0;
                        tp = realloc(temp_str, ++ts_size * sizeof(char));
                        if(errno){
                                fprintf(stderr, "Error reallocating merrory: %s\n", strerror(errno));
                                exit(EXIT_FAILURE);
                        }
			temp_str = tp;
                        temp_str[ts_size - 2] = str[i];
                        temp_str[ts_size - 1] = '\0';

                }
                *result_str = temp_str;
        }

}

bool put_in(char *str, const int size, const char c){
	// The function puts 'c' at the end of the string.
	int slen = strlen(str);
	if(slen == size-1) return false;
	str[slen+1] = str[slen];
	str[slen] = c;
	return true;
}

char put_out(char *str, const int size){
	// The function puts a char out of str and store in c.
	int slen = strlen(str);
	if(!slen) return '\0';
	char c = str[slen-1];
	str[slen-1] = str[slen];
	return c;
}

int get_word(char secret[]){
    // check if file exists first and is readable
    FILE *fp = fopen(WORDLIST_FILENAME, "rb");
    if( fp == NULL ){
        fprintf(stderr, "No such file or directory: %s\n", WORDLIST_FILENAME);
        return 1;
    }

    // get the filesize first
    struct stat st;
    stat(WORDLIST_FILENAME, &st);
    long int size = st.st_size;

    do{
        // generate random number between 0 and filesize
        long int random = (rand() % size) + 1;
        // seek to the random position of file
        fseek(fp, random, SEEK_SET);
        // get next word in row ;)
        int result = fscanf(fp, "%*s %20s", secret);
        if( result != EOF )
            break;
    }while(1);

    fclose(fp);

    return 0;
}

int is_word_guessed(const char secret[], const char letters_guessed[]){
	
	char *result_str;
	exreps(secret, &result_str);
 
	for(int i = 0, slen = strlen(result_str); i < slen ; i++){
		if(!is_in_str(letters_guessed, result_str[i])){
			free(result_str);
			return 0;
		}
	}
	
	free(result_str);
	return 1;

}

void get_guessed_word(const char secret[], const char letters_guessed[], char guessed_word[]){
	
	const char mark_char = '_';
	int s_len = strlen(secret);
	for(int i = 0; i < s_len ; i++){
		guessed_word[i] = is_in_str(letters_guessed, secret[i])	? secret[i] : mark_char;
	}
                
	guessed_word[s_len] = '\0';
}

void get_available_letters(const char letters_guessed[], char available_letters[]){

	int al_index = 0;
	for(int i = 0; i < 26; i++){
		if(!is_in_str(letters_guessed, 'a' + i))
			available_letters[al_index++] = 'a' + i;
	}
	available_letters[al_index] = '\0';
}

void hangman(const char secret[]){
	
	const int s_size = (int)strlen(secret)+1;
	int attempts = 8;
	char guess;
	char guesses[s_size]; 
	guesses[0] = '\0';
	char letters_guessed[27] = {'\0'};
	char guessed_word[s_size];
       	guessed_word[0] = '\0';
	char available_letters[27] = "abcdefghijklmnopqrstuvwxyz";
	char *secret_letters;
	exreps(secret, &secret_letters);

	printf("Welcome to the game, Hangman!\n");
	printf("I am thinking of a word that is %ld letters long.\n", strlen(secret));
	printf("-------------\n");
	printf("You have %d guesses left.\n", attempts);
	printf("Available letters: %s\nPlease guess a letter: ", available_letters);

	do{
		guess = put_out(guesses, s_size);
		if(!guess){
			do{
				guess = getchar();
			} while(guess == '\n');
	
			do{
				put_in(guesses, s_size, guess);
					if(strlen(guesses) == s_size-1){
					
						if(is_word_guessed(secret, guesses))
							printf("Congratulations, you won!\n");
						else
							printf("Sorry, bad guess. The word was %s.\n", secret);
						return;
					}
				guess = getchar();
				
			} while(guess != '\n');

			guess = put_out(guesses, s_size);
		}

		if(isalpha(guess)){
			guess = tolower(guess);
			if(is_in_str(available_letters, guess)){		
				if(is_in_str(secret_letters, guess))	
					printf("Good guess:");
				else{
					printf("Oops! That letter is not in my word:");
					attempts--;
				}
				put_in(letters_guessed, 27, guess);
				get_guessed_word(secret, letters_guessed, guessed_word);
				get_available_letters(letters_guessed, available_letters);
		
			} else{
				printf("Oops! You've already guessed that letter:");
			}
		
		} else {
			printf("Oops! '%c' is not a valid letter:", guess);
		}
		for(int i = 0, s_len = s_size - 1; i < s_len; i++)
		{
			printf(" %c", guessed_word[i]);
		}
		printf("\n-------------\n");
		if(is_word_guessed(secret, letters_guessed)){
			printf("Congratulations, you won!\n");
			break;
		}
		if(attempts){
			printf("You have %d guesses left.\n", attempts);
	        	printf("Available letters: %s\nPlease guess a letter: ", available_letters);
		} else
			printf("Sorry, you ran out of guesses. The word was %s.\n", secret);
	
	} while(attempts != 0);
	

}
