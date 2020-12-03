/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Gabrielle Josephson
 * Date: 3/11/2020
 * Citation for getDistance(char *word1, char *word2):
 * https://www.lemoda.net/c/levenshtein/
 */

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    assert(file != NULL && map != NULL);

    char* word = nextWord(file);

    while (word != NULL) {
        hashMapPut(map, word, 0);
        free(word);
        word = nextWord(file);
    }
}

/**
 * Checks that input only contains upper and lower case letters
 * @param word
 * @return 1 if input is valid and 0 if input is invalid
 */
int checkWord(char *word) {
    int length = strlen(word);

    for (int i = 0; i < length; i++) {          // Iterate through each character in the string
        if ((*(word+i) < 'A') || (*(word+i) > 'Z' && *(word+i) < 'a') || (*(word+i) > 'z')) {
            return 0;
        }
    }
    return 1;
}

/**
 * Calculates the Levenshtein distance between two words
 * @param word1
 * @param word2
 * @return Levenshtein distance between word1 and word2
 *
 * citation: https://www.lemoda.net/c/levenshtein/
 */
int getDistance(char *word1, char *word2) {
    int length1 = strlen(word1);
    int length2 = strlen(word2);

    int dist[length1+1][length2+1];
    for (int i = 0; i <= length1; i++) {
        dist[i][0] = i;
    }
    for (int i = 0; i <= length2; i++) {
        dist[0][i] = i;
    }

    for (int i = 1; i <= length1; i++) {
        char first = word1[i-1];
        for (int j = 1; j <= length2; j++) {
            char second = word2[j-1];
            if (first == second) {
                dist[i][j] = dist[i-1][j-1];
            } else {
                int erase = dist[i-1][j] + 1;
                int insert = dist[i][j-1] + 1;
                int sub = dist[i-1][j-1] + 1;
                int min = erase;

                if (insert < min) {
                    min = insert;
                }
                if (sub < min) {
                    min = sub;
                }
                dist[i][j] = min;
            }
        }
    }
    return dist[length1][length2];
}

/**
 * Populates the hashmap with the Levenshtein distances between the search word and dictionary words
 * @param map
 * @param searchWord
 */
void populateDistance (HashMap* map, char *searchWord) {
    for (int i = 0; i < map->capacity; i++) {
        struct HashLink *cur = map->table[i];
        while (cur != 0) {
            cur->value = getDistance(searchWord, cur->key);
            cur = cur->next;
        }
    }
}

/**
 * Converts a string to all lower case letters
 * @param word
 * @return word (modified to be lower case
 */
char *toLowerCase(char *word) {
    for (int i = 0; i < strlen(word); i++) {
        if (*(word+i) < 'a') {
            *(word+i) = *(word+i) + 32;
        }
    }
    return word;
}

/**
 * Checks the spelling of the word provided by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provided word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)           // Menu loop
    {
        printf("Enter a word or \"quit\" to quit: ");
        fgets(inputBuffer, 256, stdin);
        if ((strlen(inputBuffer) > 0) && (inputBuffer[strlen(inputBuffer) - 1] == '\n')) {
            inputBuffer[strlen(inputBuffer) - 1] = '\0';
        }

        if (checkWord(inputBuffer) == 0) {              // Case for if input contains invalid character
            printf("Invalid input. Please try again\n");
        } else if (strcmp(toLowerCase(inputBuffer), "quit") == 0){      // Case for if user chooses to quit program
            quit = 1;
        } else if (hashMapContainsKey(map, toLowerCase(inputBuffer)) == 1) {        // Case for if word is spelled correctly
            printf("The inputted word %s is spelled correctly.\n", inputBuffer);
        } else {
            populateDistance(map, toLowerCase(inputBuffer));        // Case for if word is spelled incorrectly
            char suggest[5][20];
            int j = 0;
            int k = 1;
            while (j != 5) {
                for (int i = 0; i < map->capacity; i++){
                    struct HashLink *cur = map->table[i];
                    while (cur != 0) {
                        if (cur->value == k) {
                            strcpy(suggest[j], cur->key);
                            j++;
                        }
                        cur = cur->next;
                        if (j == 5) {
                            break;
                        }
                    }
                    if (j == 5) {
                        break;
                    }
                }
                k++;
            }
            printf("The inputted word %s is spelled incorrectly.\n", inputBuffer);
            printf("Did you mean...\n");
            for (int i = 0; i < 5; i++) {
                printf("%s \n", suggest[i]);
            }
        }
        fflush(stdin);
    }

    hashMapDelete(map);
    return 0;
}
