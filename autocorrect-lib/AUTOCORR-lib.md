# autocorrect-lib
Autocorrect Library - probabilistic model based word spelling corrections. C language Dictionary Library designed to use the English dictionary in Trie data structures for fast autocorrection of incorrectly spelled words.

## Library Contents and Functions

* `int AUTOCORR_LENGTH_MAX` - Stores maximum permitted word length that can be checked and corrected using the library.
* `int AUTOCORR_check_word (const char* word)` - Checks if word is present in word data trie. Returns `word_frequency` if word is in present in trie, else returns `-1`.
* `bool AUTOCORR_upload (void)` - Loads word frequency data from file into a Trie data structure. Returns true if successful else false. To be called only once before any other functions from the library can be used.
* `char* AUTOCORR_correct_word (const char* word)` - Returns word with highest probability value as per words_hash_table as suggested correction for misspelled word. To be called for every misspelled word in file.
* `int AUTOCORR_size_data (void)` - Returns size of word data file, that is, number of words in the frequency data file.
* `void AUTOCORR_unload_words (void)` - Unloads word frequency data from memory. None of the above functions can be called once the file has been unloaded.

### Probability based Correction

`char* AUTOCORR_correct_word (const char* word)` finds words with edit distance = 1 and returns word with highest probability value as per `word_data.txt` as suggested correction for the misspelled word. 

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

* ~~Optimize library for space. Currently, consumes a lot of memory.~~

* Learning from user inputs
    * Add text from files checked by user into word data.
    * Process and update word frequency file.
