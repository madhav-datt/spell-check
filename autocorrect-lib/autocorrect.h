/**
 * autocorrect.h
 * 
 * Header file with the functionality of the autocorrect feature
 *
 * AUTOCORR_LENGTH_MAX - store maximum permitted word length
 * AUTOCORR_check_word - check frequency of word in trie
 * AUTOCORR_upload - add words and frequencies to trie
 * AUTOCORR_correct_word - give correct spelling of misspelled word
 * AUTOCORR_size_data - give number of unique words in word frequency data
 * AUTOCORR_unload_words - unload word hash table to free memory
 *
 * Copyright (C)   2016    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#ifndef AUTOCORRECT_H
#define AUTOCORRECT_H

#include <stdbool.h>

// Assumed maximum word length in dictionary
// (pneumonoultramicroscopicsilicovolcanoconiosis)
#define AUTOCORR_LENGTH_MAX 45

/**
 *
 * Checks if word is present in word data trie.
 * Returns word frequency if word is in present in trie, else returns -1.
 *
 */
int AUTOCORR_check_word (const char* word);

/**
 *
 * Loads word frequency data from file into trie data structure. 
 * Returns true if successful else false.
 *
 */
bool AUTOCORR_upload (void);

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
char* AUTOCORR_correct_word (const char* word);

/**
 *
 * Gives size of word frequncy data
 * Returns number of unique words in dictionary/word frequency data if loaded else 0 if not yet loaded.
 *
 */
unsigned int AUTOCORR_size_data (void);

/**
 *
 * Unloads word data from memory. 
 * Frees allocated memory blocks.
 *
 */
void AUTOCORR_unload_words (void);

#endif // AUTOCORRECT_H
