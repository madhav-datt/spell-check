/**
 * autocorrect.c
 *
 * Based on probability theory from http://norvig.com/spell-correct.html
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

/**
 * TODO:
 * 1. Implement text file to store hash values and word frequency. 
 * 2. Read from file if present, and load into hash table to save time.
 * 3. Consider words with hash values with frequency > 1 and Edit Distance = 2.
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

// Hash table for words from word data file
int* words_hash_table = NULL;

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

	// File not opened
    if (word_data_fp == NULL)
        return false;

    // Initialize hash table
    // words_hash_table[hash (word)] holds word frequency of "word" in text
    if ((words_hash_table = calloc (TABLE_SIZE, sizeof (int))) == NULL)
    {
        printf ("Out of memory. Dictionary could not be loaded.\n");
        return false; 
    }

    // Smoothing Process - assume word count 1 for each unseen word
    for (int i = 0; i < TABLE_SIZE; i++)
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


/**
 *
 * Edit distance between two words: the number of edits it would take to turn one into the other
 * Can be deletion (remove one letter), transposition (swap adjacent letters), 
 * alteration (change one letter to another) or insertion (add a letter)
 *
 * Find words with edit distance = 1
 * Return word with highest probability value as per words_hash_table
 *
 */
 void correct (const char* word)
 {
    int word_len = strlen (word);
    char word_edit_dist1 [LENGTH_MAX + 1];
    int word_edit_dist1_prob;

    // Correct replacement for incorrect word
    char word_cor [LENGTH_MAX + 1];
    strcpy (word_cor, word_edit_dist1);
    int word_cor_prob = 1;

    // Deletions (remove one letter)
    for (int i = 0; i < word_len; i++)
    {
        // Word with letter at i-th index removed
        for (int j = 0; j < i; j++)
            word_edit_dist1[j] = word[j];

        for (int j = i + 1; j < word_len; j++)
            word_edit_dist1[j - 1] = word[j];

        // Terminate word
        word_edit_dist1[word_len - 1] = '\0';

        // Choose word with highest probability value as per words_hash_table
        word_edit_dist1_prob = words_hash_table[hash (word_edit_dist1)];
        if (word_edit_dist1_prob > word_cor_prob)
        {
            strcpy (word_cor, word_edit_dist1);
            word_cor_prob = word_edit_dist1_prob;
        }
    }

    // Transposition (swap adjacent letters)
    for (int i = 0; i < word_len - 1; i++)
    {
        char tmp;

        // Swap letters word[i] and word[i + 1]
        strcpy (word_edit_dist1, word);

        tmp = word_edit_dist1[i + 1];
        word_edit_dist1[i + 1] = word_edit_dist1[i];
        word_edit_dist1[i] = tmp;

        // Choose word with highest probability value as per words_hash_table
        word_edit_dist1_prob = words_hash_table[hash (word_edit_dist1)];
        if (word_edit_dist1_prob > word_cor_prob)
        {
            strcpy (word_cor, word_edit_dist1);
            word_cor_prob = word_edit_dist1_prob;
        }
    }

    // Alteration (change one letter to another)
    // Insertion (add a letter)
}