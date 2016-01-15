/**
 * speller.c
 *
 * Implements spell-checker.
 * Checks spelling of text document contents word-by-word.
 * Autocorrects incorrectly spelled words from text document.
 *
 * Copyright (C)   2015    Madhav Datt
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

/**
 * TODO:
 * 1. Implement automatic replacement of misspelled words with corrected spellings in text file. 
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdlib.h>

#include "autocorrect.h"

#undef calculate
#undef getrusage

#define MICRO_TO_S 1000000.0

// Returns number of seconds between b and a
double calculate (const struct rusage* b, const struct rusage* a);

int main (int argc, char* argv[])
{
    char* text;

    // Check for correct number of args
    if (argc != 2)
    {
        printf("Usage: spellcheck text_file\n");
        return 1;
    }

    // Structs for timing data
    struct rusage before, after;

    // Benchmarks for autocorrect and dictionary function times
    double time_load = 0.0, time_correct = 0.0, time_check = 0.0, time_size = 0.0, time_unload = 0.0;

    // Determine text file to use
    text = argv[1];

    // Load word data
    getrusage (RUSAGE_SELF, &before);
    bool loaded_table = AUTOCORR_upload ();
    getrusage (RUSAGE_SELF, &after);

    // Calculate time to load word data
    time_load = calculate (&before, &after);

    // Abort if word data not loaded
    if (!loaded_table)
    {
        printf ("Could not load word data file.\n");
        return 1;
    }

    FILE* text_file = fopen (text, "r");
    if (text_file == NULL)
    {
        printf ("Could not open text file - %s.\n", text);
        AUTOCORR_unload_words ();
        return 1;
    }

    // Prepare to report misspellings
    printf ("\nMisspelled Words\n\n");

    // Prepare to spell-check
    int index = 0, misspellings = 0, num_words = 0;
    char word[AUTOCORR_LENGTH_MAX + 1];

    // Spell-check each word in text
    for (int c = fgetc (text_file); c != EOF; c = fgetc (text_file))
    {
        // Allow only lowercase alphabetical characters and apostrophes
        if (isalpha (c) || (c == '\'' && index > 0))
        {
            // Append character to word
            word[index] = tolower (c);
            index++;

            // Ignore alphabetical strings too long to be words
            if (index > AUTOCORR_LENGTH_MAX)
            {
                // Consume remainder of alphabetical string
                while ((c = fgetc (text_file)) != EOF && isalpha (c));

                // Prepare for new word
                index = 0;
            }
        }

        // Ignore words with numbers
        else if (isdigit (c))
        {
            // Consume remainder of alphanumeric string
            while ((c = fgetc (text_file)) != EOF && isalnum (c));

            // Prepare for new word
            index = 0;
        }

        // Whole word found
        else if (index > 0)
        {
            // Terminate current word string
            word[index] = '\0';

            // Update counter
            num_words++;

            // Check word's spelling
            getrusage (RUSAGE_SELF, &before);
            int check = AUTOCORR_check_word (word);
            getrusage (RUSAGE_SELF, &after);

            // Update benchmark
            time_check += calculate (&before, &after);

            // Print word if misspelled
            if (check == -1)
            {
                // Check word's correction
                getrusage (RUSAGE_SELF, &before);
                char* word_corrected = AUTOCORR_correct_word (word);
                getrusage (RUSAGE_SELF, &after);

                // Update benchmark
                time_correct += calculate (&before, &after);

                if (word_corrected == NULL)
                    printf ("%-45s No suggested correction\n", word);
                else
                    printf ("%-45s Suggested Correction: %s\n", word, word_corrected);

                misspellings++;
            }

            // Prepare for next word
            index = 0;
        }
    }

    // Check whether there was an error
    if (ferror (text_file))
    {
        fclose (text_file);
        printf ("Error reading %s.\n", text);
        AUTOCORR_unload_words ();
        return 1;
    }

    // Close text file
    fclose (text_file);

    // Determine dictionary's size
    getrusage (RUSAGE_SELF, &before);
    unsigned int n = AUTOCORR_size_data ();
    getrusage (RUSAGE_SELF, &after);

    // Calculate time to determine dictionary's size
    time_size = calculate (&before, &after);

    // Unload word data file
    getrusage (RUSAGE_SELF, &before);
    AUTOCORR_unload_words ();
    getrusage (RUSAGE_SELF, &after);

    // Calculate time to unload word data
    time_unload = calculate(&before, &after);

    // Report benchmarks
    printf ("\nWORDS MISSPELLED:                        %d\n", misspellings);
    printf ("WORDS IN DICTIONARY:                     %d\n", n);
    printf ("WORDS IN TEXT:                           %d\n", num_words);
    printf ("Time in loading word data file:         %.2f seconds\n", time_load);
    printf ("Time in checking text:                   %.2f seconds\n", time_check);
    printf ("Time in correcting text:                 %.2f seconds\n", time_correct);
    printf ("Time in finding size of word data:      %.2f seconds\n", time_size);
    printf ("Time in unloading word data:            %.2f seconds\n", time_unload);
    printf ("TIME IN TOTAL:                           %.2f seconds\n\n", time_load + time_check + time_size + time_unload + time_correct);

    return 0;
}

/**
 *
 * Returns number of seconds between b and a.
 *
 */
double calculate (const struct rusage* b, const struct rusage* a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a -> ru_utime.tv_sec * MICRO_TO_S + a -> ru_utime.tv_usec) -
                 (b -> ru_utime.tv_sec * MICRO_TO_S + b -> ru_utime.tv_usec)) +
                ((a -> ru_stime.tv_sec * MICRO_TO_S + a -> ru_stime.tv_usec) -
                 (b -> ru_stime.tv_sec * MICRO_TO_S + b -> ru_stime.tv_usec)))
                / MICRO_TO_S);
    }
}
