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
 * 
 * I used Abraham Serrato Mesa, Josh Jones, and Antonio Florencio's conversation on Slack
 * (12/02/2019) to help me fix the memory leaks in this function.
 * 
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    char* word = nextWord(file);

    do
    {

        hashMapPut(map,word,0);
        free(word);
        word = nextWord(file);


    }while(word != NULL);

  

}

/*
*This function calculates the Levenshtein Distance recursively. I found this code at
* https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
*
*I googled outside of the provided wikipedia article after I read on Piazza that the
*recursive implementation was slow.
*
* The first parameter will be the input variable
* The second will be the word we are checking against
* This funtion returns the Levenshtein distance.
*/

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * 
 * I used https://helloacm.com/c-c-coding-exercisse-how-to-implement-tolowercase-function/
 * to implement a toLower code right below the scanf call to read inputBuffer.  This was 
 * necessary to ensure the program is case insensitive;
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    int a = 0;

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
   

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        while(inputBuffer[a] != '\0')
        {
            if(inputBuffer[a] >= 'A' && inputBuffer[a]<= 'Z')
            {
                inputBuffer[a] += 32;
            }

            a++;
        }

        // Implement the spell checker code here..

        if(hashMapContainsKey(map, inputBuffer))
        {
            printf("%s is spelled correctly. \n", inputBuffer);
        }
        else
        {
            printf("%s is not in the dictionary \n", inputBuffer);
            
            HashLink* current;
            int minDistance = 100;
            int suggestionCount = 0;
            //fill value with levenshtein distance
            for(int i = 0; i < map->capacity; i++)
            {
                current = map->table[i];

                while(current != NULL)
                {
                    int editDist = levenshtein(inputBuffer,current->key);
                    if (editDist < minDistance)
                    {
                        minDistance = editDist;
                    }
                    hashMapPut(map,current->key,editDist);
                    current = current->next;
                }
            }

            
            while(suggestionCount != 5)
            {
                for(int i = 0; i < map->capacity; i++)
                {
                    current = map->table[i];

                    while(current != 0)
                    {
                        if(suggestionCount == 5)
                        {
                            break;
                        }

                        if(current->value == minDistance)
                        {
                            printf("Did you mean: %s \n", current->key);
                            suggestionCount++;
                        }

                        current = current->next;
                    }

                }
                minDistance++;
            }

        }
        

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}
