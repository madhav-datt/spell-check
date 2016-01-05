# autocorrect-lib
Autocorrect Library - probabilistic model based word spelling corrections. C language Dictionary Library designed to use the English dictionary in Trie data structures for fast autocorrection of incorrectly spelled words.

## Library Contents and Functions

* `int check_word_frequency (const char* word)` - Checks if word is present in word data trie. Returns word frequency if word is in present in trie, else returns 0.
* `bool hash_words (void)` - Loads word frequency data from file into hash table. Returns true if successful else false. To be called only once before any of the other functions can be used.
* `char* correct_word (const char* word)` - Returns word with highest probability value as per words_hash_table as suggested correction for misspelled word. To be called for every misspelled word in file.
* `bool unload_words (void)` - Unloads word frequency data from memory.  Returns true if successful else false. The above function can't be called once the file has been unloaded.

### Using Probability based correction

`char* correct_word (const char* word)` finds words with edit distance = 1 and returns word with highest probability value as per `words_hash_table` as suggested correction for misspelled word. 

Edit distance between two words is defined as the number of edits to turn one into the other. Can be a deletion (remove one letter), a transposition (swap adjacent letters), an alteration (change one letter to another) or an insertion (add a letter).

## Using the library

### Downloading the library

    $ wget https://github.com/madhav-datt/autocorrect-lib/archive/autocorrect-lib.v1.0.zip
    $ unzip autocorrect-lib.v1.0.zip
    $ rm -f autocorrect-lib.v1.0.zip

### Using the library in a program

Include the following header file in your program:

    #include "/Downloads/autocorrect-lib.v1.0/lib/autocorrect.h"
    
## TODOs

* Autocorrect error correction probability improvement.
    * Take into account words with 2 corrections instead of 1.
    * Consider word hash values with frequency > 1 and Edit Distance = 2.

* Optimize library for space. Currently, consumes a lot of memory.

