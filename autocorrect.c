/**
 * autocorrect.c
 *
 * Based on probability theory from http://norvig.com/spell-correct.html
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "autorun.h"

#define LENGTH_MAX 45
#define WORD_DATA "word_data.txt"

/**
 *
 * Experimentally selected hash table size.
 * Ensure no collisions for default dictionary/word data used
 *
 */
#define TABLE_SIZE 800011

int* words_hash_table;

/**
 *
 * Hash function for word data strings
 * djb2 by Dan Bernstein <http://cr.yp.to/djb.html>
 *
 */
unsigned long hash (unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c; 

    return (hash % TABLE_SIZE);
}

/**
 *
 * Loads word frequency data from file into hash table. 
 * Returns true if successful else false.
 *
 */
bool hash_words (void)
{
	FILE* word_data_fp;
	word_data_fp = fopen (WORD_DATA, "r");

	// Word date file not opened
    if (word_data_fp == NULL)
        return false;

    // Initialize hash table
    // words_hash_table[hash (word)] holds word frequency of "word" in text
    if ((words_hash_table = calloc (TABLE_SIZE, sizeof (int))) == NULL)
    {
        printf ("Out of memory. Dictionary could not be loaded.\n");
        return false; 
    }

    for (int i = 0; i < TABLE_SIZE; i++)
        // Smoothing Process - assume word count 1 for each unseen word
        words_hash_table[i] = 1;

  	// Prepare to read words from word data file
    int index = 0;
    char word[LENGTH_MAX + 1];

    // Hash each word in text
    for (int c = fgetc (word_data_fp); c != EOF; c = fgetc (word_data_fp))
    {
        // Allow only alphabetical characters and apostrophes
        if (isalpha (c) || (c == '\'' && index > 0))
        {
            // Append character to word
            word[index] = c;
            index++;

            // Ignore alphabetical strings too long to be words
            if (index > LENGTH_MAX)
            {
                // Consume remainder of alphabetical string
                while ((c = fgetc (word_data_fp)) != EOF && isalpha (c));

                // Prepare for new word
                index = 0;
            }
        }

        // Ignore words with numbers
        else if (isdigit (c))
        {
            // Consume remainder of alphanumeric string
            while ((c = fgetc (word_data_fp)) != EOF && isalnum (c));

            // Prepare for new word
            index = 0;
        }

        // Whole word found - increase count in hash table
        else if (index > 0)
        {
            // Terminate current word string
            word[index] = '\0';

            words_hash_table[hash (word)]++;

            // Prepare for next word
            index = 0;
        }
    }

    // If end of file not reached before loop termination and error indicator is not set
    if (feof (word_data_fp) && !ferror (word_data_fp))
    {
        fclose (word_data_fp);
        return true;
    }

	fclose (word_data_fp);
	return false;
}