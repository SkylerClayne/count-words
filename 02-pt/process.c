/****************************************
*       Family Name: Layne              *
*       Given Name: Skyler              *
*       Section: Z                      *
*       Student Number: 212166906       *
*       CSE Login: cse23170             *
*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FUNCTION DECLARATIONS */

int compare(const void *a, const void *b);
int compareFiles(const void *a, const void *b);
int countWords(FILE *file);

/* STRUCTURE DECLARATIONS */

typedef struct WordArray
{
    char word[101];
    int freq;
} WordArray;

typedef struct TopThree
{
    int words;
    char filename[101];
    WordArray wordone;
    WordArray wordtwo;
    WordArray wordthree;
} TopThree;

typedef struct DataSet
{
    int words;
    char filename[101];
    char wordone[101];
    char wordtwo[101];
    char wordthree[101];
} DataSet;

/*
* Compare within each child process
*/
int compare(const void *a, const void *b)
{
    WordArray *wone = (WordArray *)a;
    WordArray *wtwo = (WordArray *)b;
    return (wtwo->freq - wone->freq);
}

/*
* Compare the Files after the children are finished playing!
*/
int compareFiles(const void *a, const void *b)
{
    DataSet *dbone = (DataSet *)a;
    DataSet *dbtwo = (DataSet *)b;
    return (dbtwo->words - dbone->words);
}

/*
* COUNT THE NUMBER OF WORDS IN A FILE.
*/
int countWords(FILE *file)
{
    char c;
    int wc = 0;
    char *word = malloc (sizeof(char) * 101);

    c = 0;
    //ASSERT: Count the number of words in the document.
    while (c != EOF)
    {
        c = fscanf(file, "%s", word);
        wc++;
    }
    free(word);
    rewind(file);

    return wc;
}

/*
* READ ALL WORDS AND STORE THEM IN AN ARRAY of WordArrays
*/
WordArray *wordsToArray(FILE *file, int wordCount)
{
    // VARIABLES
    int charindex, index = 0;
    char *word = malloc (sizeof(char) * 101);

    // INITIALIZE: Create the first array
    WordArray *first = malloc( sizeof(WordArray) * wordCount);

    while (charindex != EOF)
    {
        charindex = fscanf(file, "%s", word);
        strcpy(first[index].word, word);
        first[index].freq = 0;
        index++;
    }
    return first;
}

/*
* GET THE TOP 3 WORDS AND NUMBER OF DISTINCT WORDS
*/
TopThree getTopThree(char *filename, FILE *file, int wordCount, WordArray *first)
{
    // VARIABLES
    int firstIndex, secondIndex;
    int finalArrayIndex;
    int foundIndex;
    int found;


    WordArray *words = malloc( sizeof(WordArray) * wordCount);
    finalArrayIndex = 0;
    int countNumberOfOccurances;

    // COMPARE EVERYWORD AND COUNT THE OCCURANCES
    for (firstIndex = 0; firstIndex < wordCount - 1; firstIndex++)
    {
        countNumberOfOccurances = 0;
        // Count the occurances in the word.
        for (secondIndex = 0; secondIndex < wordCount - 1; secondIndex++)
        {

            if (strcmp(first[firstIndex].word, first[secondIndex].word) == 0)
            {
                countNumberOfOccurances++;
            }
        }
        for (foundIndex = 0; foundIndex <= firstIndex; foundIndex++)
        {
            found = 0;
            if (strcmp(words[foundIndex].word, first[firstIndex].word) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            strcpy(words[finalArrayIndex].word, first[firstIndex].word);
            words[finalArrayIndex].freq = countNumberOfOccurances;
            finalArrayIndex++;

        }
    }

    //ASSERT CREATED AN ARRAY REPEATED WORDS AND THEIR FREQUENCIES
    qsort(words, wordCount, sizeof(WordArray), compare);

    TopThree tt;
    strcpy(tt.wordone.word, words[0].word);
    tt.wordone.freq = words[0].freq;
    strcpy(tt.wordtwo.word, words[1].word);
    tt.wordtwo.freq = words[1].freq;
    strcpy(tt.wordthree.word, words[2].word);
    tt.wordthree.freq = words[2].freq;
    tt.words = finalArrayIndex;
    strcpy(tt.filename, filename);

    //RETURN: A WORDARRAY SORTED FROM INCREASING TO DECREASING FREQUENCIES.
    return tt;
}

int main(int argc, char *argv[])
{
    // VARIABLES
    int pid;
    int fd[2];
    char buffer[101];
    int wordCount;
    char wordcounted[101];
    char wordcountString[101];
    int adding;
    int lol;
    int numbFiles;
    numbFiles = argc - 1;

    // Create a list of dataset.
    DataSet *final = malloc(sizeof(DataSet) * argc);

    if (pipe(fd) < 0)
    {
        printf("pipe error");
    }
    int i;
    // Grab a file for reading from the command line.
    for (i = 1; i <= numbFiles; i++)
    {
        // Attempted to create a child process for each file
        pid = fork();

        if (pid < 0)
        {
            //ASSERT: Fork failed
            fprintf(stderr, "Fork Failed!\n");
            exit(-1);
        }
        else if (pid == 0)    // DO CHILDREN THINGS!
        {
            //ASSERT: Child process created
            FILE *file = fopen( argv[i], "r");
            if (file == 0)
            {
                // ASSERT: Couldn't open the file for reading.
                printf("Couldn't open file\n");
            }
            else
            {
                // ASSERT: Opened file for reading.

                //Count the number of words in the file.
                wordCount = countWords(file);

                WordArray *first = wordsToArray(file, wordCount);
                TopThree tt = getTopThree(argv[i], file, wordCount, first);

                //ASSERT: Passing data through the pipe
                close(fd[0]);
                sprintf(wordcounted, "%d", tt.words); //turn the word count into a char*
                write(fd[1], wordcounted, sizeof(buffer));//char* of word count.
                write(fd[1], tt.filename, sizeof(buffer)); // Name of file
                write(fd[1], tt.wordone.word, sizeof(buffer)); // First most common word
                write(fd[1], tt.wordtwo.word, sizeof(buffer)); // Second most common word
                write(fd[1], tt.wordthree.word, sizeof(buffer)); // Third most common word

                // ASSERT: finished with "first" array
                free(first);
                fclose(file);
                exit(0);

            }
        }
    }

    if (pid > 0)
    {
        //ASSERT: Parent process reached

        wait(NULL);
        close(fd[1]);

        //ASSERT: loop over all the files and get their data from the pipe
        for (adding = 0; adding < numbFiles; adding++)
        {
            read(fd[0], wordcountString, sizeof(buffer));
            final[adding].words = atoi(wordcountString);
            read(fd[0], final[adding].filename, sizeof(buffer));
            read(fd[0], final[adding].wordone, sizeof(buffer));
            read(fd[0], final[adding].wordtwo, sizeof(buffer));
            read(fd[0], final[adding].wordthree, sizeof(buffer));
        }

        // ASSERT: Sort reconstructed array
        qsort(final, numbFiles, sizeof(DataSet), compareFiles);

        //ASSERT: Print the filename, number of distinct and their top 3 words
        for (lol = 0; lol < argc - 1; lol++)
        {
            printf("%s: %d %s %s %s\n", final[lol].filename, final[lol].words, final[lol].wordone, final[lol].wordtwo, final[lol].wordthree);
        }
    }
    free(final);
}
