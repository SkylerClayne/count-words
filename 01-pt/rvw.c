//
// # Family Name: Layne
// # Given Name: Skyler
// # Section: Z
// # CSE Login: cse23170
//
//  Created by Skyler Layne on 2015-01-09.
//  THIS CODE USES C99  
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FUNCTION DECLARATIONS */

typedef struct WordArray{
    char word[101];
    int freq;
} WordArray;


int compare(const void *a, const void *b)
{
    WordArray *wone = (WordArray *)a;
    WordArray *wtwo = (WordArray *)b;
    return(wtwo->freq - wone->freq);
}

int main(int argc, char *argv[])
{
    // Grab a file for reading from the command line.

	int i;
    for (i = 1; i <= argc-1; i++)
    {
        //  printf("NEWFILE: %s\n", argv[i]);
        FILE *file = fopen( argv[i], "r");
        
        if (file == 0)
        { // ASSERT: Couldn't open the file for reading.
            printf("Couldn't open file\n");
            
        } else
        { // ASSERT: Opened file for reading.
            int k;
            int charindex;
            int index;
            int firstIndex, secondIndex;
            int finalArrayIndex;
            int foundIndex;
            int wordCount;
            
            char *word = malloc (sizeof(char) * 101);
            char c;
            
            wordCount = 0;
            c = 0;
            //ASSERT: Count the number of words in the document.
            while (c != EOF)
            {
                c = fscanf(file, "%s", word);
                wordCount++;
            }
            
            
            //RESET File and free pointers
            free(word);
            rewind(file);
            
            // INITIALIZE: Create the first array
            WordArray *first = malloc( sizeof(WordArray) * wordCount);
            charindex = 0;
            index = 0;
            while (charindex != EOF)
            {
                charindex = fscanf(file, "%s", word);
                strcpy(first[index].word, word);
                first[index].freq = 0;
                index++;
            }
            
            WordArray *words = malloc( sizeof(WordArray) * wordCount);
            finalArrayIndex = 0;
            int countNumberOfOccurances;
            int distinctWords = 0;
            for (firstIndex = 0; firstIndex < wordCount - 1; firstIndex++) {
                countNumberOfOccurances = 0;
                // Count the occurances in the word.
                for (secondIndex = 0; secondIndex < wordCount - 1; secondIndex++) {
                    
                    if (strcmp(first[firstIndex].word, first[secondIndex].word) == 0) {
                        countNumberOfOccurances++;
                    }
                }
                int found;
                for (foundIndex = 0; foundIndex <= firstIndex; foundIndex++) {
                    found = 0;
                    if (strcmp(words[foundIndex].word, first[firstIndex].word) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    strcpy(words[finalArrayIndex].word, first[firstIndex].word);
                    words[finalArrayIndex].freq = countNumberOfOccurances;
                    finalArrayIndex++;
                }
            }
            
            // FINISHED FIRST ARRAY
            free(first);
            
            // int size = sizeof(words) / sizeof(word[0]);
            qsort(words, wordCount, sizeof(WordArray), compare);
            
            printf("%s: %d %s %s %s\n", argv[i], finalArrayIndex, words[0].word, words[1].word, words[2].word);
            
            // FINISHED WITH WORDS ARRAY
            free(words);
            rewind(file);
            fclose(file);
        }
    }
}
