/**
 * dictionary.h
 * 
 * Header file with the functionality of the dictionary.
 * Trie based Dictionary library.
 *
 * LENGTH - store maximum permitted word length
 * load - load dictionary into memory
 * check - check if a word is present in the dictionary
 * size - give number of words in dictionary
 * unload - unload dictionary to free memory
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Assumed maximum word length in dictionary
// (pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

/**
 *
 * Checks if word is present in the dictionary - ie. is correctly spelled.
 * Returns true if word is in dictionary else false.
 *
 */
bool check (const char* word);

/**
 *
 * Loads dictionary into memory.  Returns true if successful else false.
 * Uses Tries as data structure to store dictionary in memory.
 *
 */
bool load (const char* dictionary);

/**
 *
 * Gives size of dictionary
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 *
 */
unsigned int size (void);

/**
 *
 * Unloads dictionary from memory.  Returns true if successful else false.
 * Frees allocated memory blocks.
 *
 */
bool unload (void);

#endif // DICTIONARY_H
