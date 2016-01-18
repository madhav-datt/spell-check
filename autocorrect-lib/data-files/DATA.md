# Word Data for Probabilistic Model

## Source

The file `big.txt` is a concatenation of several public domain English language eBooks from [Project Gutenberg](https://www.gutenberg.org/). The file has *1941332 words*.
 
## Word Frequency Data Model

The autocorrect program uses `word_data.txt` file to find and correct misspelled words. Each line of the file contains information about a unique word in the format `frequency_in_file word` and is sorted in decreasing order of word frequency.

To generate the `word_data.txt` file from `big.txt` file, run:

    $ chmod 755 form_data
    $ ./form_data

The above shell script converts all letters to lowercase, removes punctuation and special characters, groups and sorts words and removes leading whitespaces to give data in the required format.

