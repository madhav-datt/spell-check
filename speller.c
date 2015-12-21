/**
 * speller.c
 *
 * Implements a spell-checker.
 */

#include <ctype.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdlib.h>

#include "dictionary.h"
#undef calculate
#undef getrusage

// default dictionary
// assumed that "Dictionary.txt" file is saved in save directory as program binary executable file
#define DICTIONARY "Dictionary.txt"

// prototype
double calculate (const struct rusage* b, const struct rusage* a);

int main (int argc, char* argv[])
{
    char* dictionary;
    char* text;

    // check for correct number of args
    if (argc != 2 && argc != 3)
    {
        printf("Usage: speller [dictionary_file] text_file\n");
        return 1;
    }

    // structs for timing data
    struct rusage before, after;

    // benchmarks
    double time_load = 0.0, time_check = 0.0, time_size = 0.0, time_unload = 0.0;

    // determine dictionary and text file to use
    if (argc == 3)
    {
        dictionary = argv[1];
        text = argv[2];
    }
    else
    {
        // default dictionary is used
        dictionary = DICTIONARY;
        text = argv[1];
    }

    // load dictionary
    getrusage (RUSAGE_SELF, &before);
    bool loaded = load (dictionary);
    getrusage (RUSAGE_SELF, &after);

    // abort if dictionary not loaded
    if (!loaded)
    {
        printf ("Could not load dictionary file - %s.\n", dictionary);
        return 1;
    }

    // calculate time to load dictionary
    time_load = calculate (&before, &after);

    FILE* text_file = fopen (text, "r");
    if (text_file == NULL)
    {
        printf ("Could not open text file - %s.\n", text);
        unload ();
        return 1;
    }

    // prepare to report misspellings
    printf ("\nMisspelled Words\n\n");

    // prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH + 1];

    // spell-check each word in text
    for (int c = fgetc (text_file); c != EOF; c = fgetc (text_file))
    {
        // allow only alphabetical characters and apostrophes
        if (isalpha (c) || (c == '\'' && index > 0))
        {
            // append character to word
            word[index] = c;
            index++;

            // ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // consume remainder of alphabetical string
                while ((c = fgetc (text_file)) != EOF && isalpha (c));

                // prepare for new word
                index = 0;
            }
        }

        // ignore words with numbers
        else if (isdigit (c))
        {
            // consume remainder of alphanumeric string
            while ((c = fgetc (text_file)) != EOF && isalnum (c));

            // prepare for new word
            index = 0;
        }

        // we must have found a whole word
        else if (index > 0)
        {
            // terminate current word
            word[index] = '\0';

            // update counter
            words++;

            // check word's spelling
            getrusage (RUSAGE_SELF, &before);
            bool misspelled = !check(word);
            getrusage (RUSAGE_SELF, &after);

            // update benchmark
            time_check += calculate (&before, &after);

            // print word if misspelled
            if (misspelled)
            {
                printf ("%s\n", word);
                misspellings++;
            }

            // prepare for next word
            index = 0;
        }
    }

    // check whether there was an error
    if (ferror (text_file))
    {
        fclose (text_file);
        printf ("Error reading %s.\n", text);
        unload ();
        return 1;
    }

    // close text file
    fclose (text_file);

    // determine dictionary's size
    getrusage (RUSAGE_SELF, &before);
    unsigned int n = size();
    getrusage (RUSAGE_SELF, &after);

    // calculate time to determine dictionary's size
    time_size = calculate (&before, &after);

    // unload dictionary
    getrusage (RUSAGE_SELF, &before);
    bool unloaded = unload ();
    getrusage (RUSAGE_SELF, &after);

    // abort if dictionary not unloaded
    if (!unloaded)
    {
        printf ("Could not unload dictionary - %s.\n", dictionary);
        return 1;
    }

    // calculate time to unload dictionary
    time_unload = calculate(&before, &after);

    // report benchmarks
    printf ("\nWORDS MISSPELLED:                        %d\n", misspellings);
    printf ("WORDS IN DICTIONARY:                     %d\n", n);
    printf ("WORDS IN TEXT:                           %d\n", words);
    printf ("Time in loading dictionary file:         %.2f seconds\n", time_load);
    printf ("Time in checking text:                   %.2f seconds\n", time_check);
    printf ("Time in finding size of dictionary:      %.2f seconds\n", time_size);
    printf ("Time in unloading dictionary file:       %.2f seconds\n", time_unload);
    printf ("TIME IN TOTAL:                           %.2f seconds\n\n", time_load + time_check + time_size + time_unload);

    return 0;
}

/**
 * Returns number of seconds between b and a.
 */
double calculate (const struct rusage* b, const struct rusage* a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a -> ru_utime.tv_sec * 1000000 + a -> ru_utime.tv_usec) -
                 (b -> ru_utime.tv_sec * 1000000 + b -> ru_utime.tv_usec)) +
                ((a -> ru_stime.tv_sec * 1000000 + a -> ru_stime.tv_usec) -
                 (b -> ru_stime.tv_sec * 1000000 + b -> ru_stime.tv_usec)))
                / 1000000.0);
    }
}
