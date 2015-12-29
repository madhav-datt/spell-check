/**
 * autocorrect.h
 * 
 * Header file with the functionality of the autocorrect feature.
 *
 * hash_words - add words and frequencies to hash table
 * correct - give correct spelling of misspelled word
 * unload_table - unload word hash table to free memory
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#ifndef AUTOCORRECT_H
#define AUTOCORRECT_H

#include <stdbool.h>

/**
 *
 * Loads word frequency data from file into hash table. 
 * Returns true if successful else false.
 *
 */
bool hash_words (void);

/**
 *
 * Edit distance between two words: number of edits to turn one into the other.
 * Can be deletion (remove one letter), transposition (swap adjacent letters), 
 * alteration (change one letter to another) or insertion (add a letter)
 *
 * Finds words with edit distance = 1
 * Returns word with highest probability value as per words_hash_table as suggested
 * correction for misspelled word.
 *
 */
char* correct_word (const char* word);

/**
 *
 * Unloads word hash table from memory.  Returns true if successful.
 * Frees allocated memory blocks.
 *
 */
bool unload_words (void);

#endif // AUTOCORRECT_H
