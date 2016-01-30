/**
 * autocorrect.c
 *
 * Implementation of autocorrect library functionalities
 * Based on a probabilistic model for word correction
 * 
 * AUTOCORR_check_word - check frequency of word in trie
 * AUTOCORR_upload - add words and frequencies to trie
 * AUTOCORR_correct_word - give correct spelling of misspelled word
 * AUTOCORR_size_data - give number of unique words in word frequency data
 * AUTOCORR_unload_words - unload word hash table to free memory
 * unload_rec - recursively unloads trie from memory
 * 
 * Based on probability theory from http://norvig.com/spell-correct.html
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

/**
 * TODO:
 * 1. Consider words with frequency > 1 and Edit Distance = 2.
 * 2. Add text from files checked by user into word data. Process and update word frequency file.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "autocorrect.h"

// Word frequnecy data file
#define WORD_DATA "../autocorrect-lib/data-files/word_data.txt"

// Define each node of the trie with 27 children nodes
typedef struct node
{
    int frequency;
    
    // 0-25 for lowercase a-z, 26 for apostrophe (')
    struct node* next[27]; 
} node;

// For number of unique words stored in the word frequency data loaded
// Gives size of word frequnecy data
static int number_dict_words = 0;

// Initialize trie to hold word data and frequencies in memory
node* word_freq = NULL;

/**
 *
 * Checks if word is present in word data trie.
 * Returns word frequency if word is in present in trie, else returns -1.
 *
 */
int AUTOCORR_check_word (const char* word)
{
    int index, freq;
    node* tmp = word_freq;

    int word_len = strlen (word);

    // Ignore terminal single quote character as part of word during spellcheck
    if (word[word_len - 1] == '\'')
        word_len--;

    for (int i = 0; i < word_len; i++)
    {
        //printf ("%d", tmp -> frequency);
        // Check if word is not just a prefix of a dictionary word
        if (i == (word_len - 1) && tmp -> frequency == 0)
            return -1;

        else
            // Frequency of word in trie
            freq = tmp -> frequency;

        // Handle ' as a special character
        if (word[i] == '\'')
            index = 26;

        // Handle letters of the alphabet
        // Index between 0 - 25
        else
            index = (int)tolower (word[i]) - 'a';

        // Handle error cases
        if (index > 26 || index < 0)
            continue;
        
        // Check for character in trie
        if (tmp -> next[index] == NULL)
            return -1;

        else
            tmp = tmp -> next[index];
    }
    return freq;
}


/**
 *
 * Loads word frequency data from file into trie data structure. 
 * Returns true if successful else false.
 *
 */
bool AUTOCORR_upload (void)
{
	FILE* word_data_fp;
	word_data_fp = fopen (WORD_DATA, "r");

	// File not opened
    if (word_data_fp == NULL)
        return false;

    // Initialize trie for dictionary
    if ((word_freq = calloc (1, sizeof (node))) == NULL)
    {
        printf ("Out of memory. Dictionary could not be loaded.\n");
        return false;            
    }
    for (int i = 0; i < 27; i++)
        word_freq -> next[i] = NULL;
    word_freq -> frequency = 0;

    node* tmp = word_freq;

  	// Prepare to read words from word data file
    int index = 0;
    char word_str[AUTOCORR_LENGTH_MAX + 1];
    int word_count;

    while (fscanf (word_data_fp, "%d", &word_count) && fscanf (word_data_fp, "%s", word_str) != EOF)
    {
        int word_len = strlen (word_str);

        for (int i = 0; i < word_len; i++)
        {
            // Set word as present in dictionary
            // Set frequency from file data
            if (i == word_len - 1)
                tmp -> frequency = word_count;

            // Handle ' as a special character
            if (word_str[i] == '\'')
                index = 26;

            // Handle letters of the alphabet
            // Neglect character case
            else if (isalpha (word_str[i]))
                index = (int)tolower (word_str[i]) - 'a';

            // Handle error cases
            if (index > 26 || index < 0)
                continue;

            // Insert character from word read from dictionary into Trie
            if (tmp -> next[index] == NULL)
            {
                // Create and initialize new next nodes for subsequent letter
                if ((tmp -> next[index] = calloc (1, sizeof (node))) == NULL)
                {
                    printf ("Out of memory. Dictionary could not be loaded.\n");
                    return false;
                }

                tmp = tmp -> next[index];
                for (int j = 0; j < 27; j++)
                    tmp -> next[j] = NULL;
                tmp -> frequency = 0;
            }
            else
                tmp = tmp -> next[index];
        }

        // Insert next word from dictionary
        tmp = word_freq;
        number_dict_words++; // Dictionary word count
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

// Correct replacement for incorrect word
char* word_cor = NULL;

/**
 *
 * Edit distance between two words: number of edits to turn one into the other.
 * Can be deletion (remove one letter), transposition (swap adjacent letters), 
 * alteration (change one letter to another) or insertion (add a letter)
 *
 * Finds words with edit distance = 1
 * Returns word with highest probability value as per word frequency data trie as suggested
 * correction for misspelled word.
 * Returns NULL in case of error.
 *
 */
char* AUTOCORR_correct_word (const char* word)
{
    int word_len = strlen (word);

    // Words with edit distance of 1 from misspelled word
    char word_edit_dist1 [AUTOCORR_LENGTH_MAX + 2];
    int word_edit_dist1_prob;

    // Prepare for next work check
    free (word_cor);
    word_cor = NULL;

    // Intialize correct word string
    if ((word_cor = calloc (AUTOCORR_LENGTH_MAX + 2, sizeof (char))) == NULL)
    {
        printf ("Out of memory. Autocorrect could not be run.\n");
        return NULL; 
    }

    int word_cor_prob = 0;

    /**
     * Deletions (remove one letter)
     * Length of word = word_len; Length of word_edit_dist1 = word_len - 1
     */
    for (int i = 0; i < word_len; i++)
    {
        // Word with letter at i-th index removed
        for (int j = 0; j < i; j++)
            word_edit_dist1[j] = word[j];

        for (int j = i + 1; j < word_len; j++)
            word_edit_dist1[j - 1] = word[j];

        // Terminate word
        word_edit_dist1[word_len - 1] = '\0';

        // Choose word with highest probability value as per word data frequency trie
        word_edit_dist1_prob = AUTOCORR_check_word (word_edit_dist1);
        if (word_edit_dist1_prob > word_cor_prob)
        {
            strcpy (word_cor, word_edit_dist1);
            word_cor_prob = word_edit_dist1_prob;
        }
    }

    /**
     * Transposition (swap adjacent letters)
     * Length of word = word_len; Length of word_edit_dist1 = word_len
     */
    for (int i = 0; i < word_len - 1; i++)
    {
        char tmp;
        
        strcpy (word_edit_dist1, word);

        // Swap letters word[i] and word[i + 1]
        tmp = word_edit_dist1[i + 1];
        word_edit_dist1[i + 1] = word_edit_dist1[i];
        word_edit_dist1[i] = tmp;

        // Choose word with highest probability value as per word data frequency trie
        word_edit_dist1_prob = AUTOCORR_check_word (word_edit_dist1);
        if (word_edit_dist1_prob > word_cor_prob)
        {
            strcpy (word_cor, word_edit_dist1);
            word_cor_prob = word_edit_dist1_prob;
        }
    }

    /**
     * Alteration (change one letter to another)
     * Length of word = word_len; Length of word_edit_dist1 = word_len
     */
    for (int i = 0; i < word_len; i++)
    {
        for (char c = 'a'; c <= 'z'; c++)
        {
            // Word with letter at i-th index changed to c
            for (int j = 0; j < i; j++)
                word_edit_dist1[j] = word[j];

            // Letter at i-th index altered
            word_edit_dist1[i] = c;

            for (int j = i + 1; j < word_len; j++)
                word_edit_dist1[j] = word[j];

            // Terminate word
            word_edit_dist1[word_len] = '\0';

            // Choose word with highest probability value as per word data frequency trie
            word_edit_dist1_prob = AUTOCORR_check_word (word_edit_dist1);
            if (word_edit_dist1_prob > word_cor_prob)
            {
                strcpy (word_cor, word_edit_dist1);
                word_cor_prob = word_edit_dist1_prob;
            }
        } 
    }

    /**
     * Insertion (add a letter)
     * Length of word = word_len; Length of word_edit_dist1 = word_len + 1
     */
    for (int i = 0; i < word_len + 1; i++)
    {
        for (char c = 'a'; c <= 'z'; c++)
        {
            // Word with letter added at i-th index
            for (int j = 0; j < i; j++)
                word_edit_dist1[j] = word[j];

            // Insert letter at i-th index
            word_edit_dist1[i] = c;

            for (int j = i; j < word_len; j++)
                word_edit_dist1[j + 1] = word[j];

            // Choose word with highest probability value as per word data frequency trie
            word_edit_dist1_prob = AUTOCORR_check_word (word_edit_dist1);
            if (word_edit_dist1_prob > word_cor_prob)
            {
                strcpy (word_cor, word_edit_dist1);
                word_cor_prob = word_edit_dist1_prob;
            }
        }
    }

    // Handle empty strings - no replacement word found
    if (word_cor[0] == '\0')
        return NULL;

    return word_cor;
}

/**
 *
 * Gives size of word frequncy data
 * Returns number of unique words in dictionary/word frequency data if loaded else 0 if not yet loaded.
 *
 */
unsigned int AUTOCORR_size_data (void)
{
    return number_dict_words;
}

/**
 *
 * Recursive function to unload trie from memory.
 *
 */
void unload_recr (node* dict_rem)
{
    // Recursively freeing allocated memory
    for (int i = 0; i < 27; i++)
        if (dict_rem -> next[i] != NULL)
            unload_recr (dict_rem -> next[i]);

    free (dict_rem);
    dict_rem = NULL;
}

/**
 *
 * Unloads word data from memory.
 * Frees allocated memory blocks.
 *
 */
void AUTOCORR_unload_words (void)
{
    free (word_cor);
    word_cor = NULL;

    unload_recr (word_freq);
}
