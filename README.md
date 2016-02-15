# spell-check
**Automatically find** and **fix spelling errors** in your READMEs and other documents that you write without a word processor. spell-check is a fast command line application to spell-check large text files (_books, Github files, assignments etc._) and autocorrect misspelled words based on a probabilistic model. The program is optimized for speed and can check over 1 million words in less than 1 second.

## Checking Documents
### Download and Install from source

    $ wget https://github.com/madhav-datt/spell-check/archive/spell-check-v2.0.zip
    $ unzip spell-check-v2.0.zip
    $ rm -f spell-check-v2.0.zip
    $ cd spell-check-v2.0.zip/src
    $ make

### Running spellchecker

    $ cd spell-check-v2.0.zip/src
    $ ./spellchecker /path/to/file/file_to_be_checked.txt
    
### Output
    
The program will output a list of all the misspelled words along with suggested corrections, and file checking benchmarks.

![Sample spell-check output](https://github.com/madhav-datt/spell-check/blob/master/resources/output.png)

_`oficiel`_, which was intended to be _`official`_ has no suggested correction because it has an edit distance of more than 1 from a correctly spelled word. Read more about this [here](https://github.com/madhav-datt/spell-check/blob/master/autocorrect-lib/AUTOCORR-lib.md).

## Benchmarks

Both speed and accuracy benchmarks give an approximate value that has been averaged over multiple input text files and documents.

### Spellcheck Speed

Optimized for speed - can spellcheck over *1 Million words in less than 1 second*.

| **Misspelled Words** | **Words in Dictionary** | **Words in Text Document** | **Time in Loading Data** |
|:--------------------:|:-----------------------:|:--------------------------:|:------------------------:|
| 122.5                |  368895                 |  10237257.5                |      0.34 seconds        |


| **Time in Checking Text** | **Time in Correcting Text** | **Time in Unloading Data** | **Total Time** |
|:-------------------------:|:---------------------------:|:------------------------:|:----------------------:|
|      0.56 seconds         |          0.01 seconds       |    0.15 seconds            |    0.98 seconds     |

![Sample speed benchmark](https://github.com/madhav-datt/spell-check/blob/master/resources/speed_benchmark.png)

### Autocorrect Accuracy

Calculated on inputs from Roger Mitton's Birkbeck spelling error corpus from the Oxford Text Archive. On a development set of 250 test cases (including context based mistakes for correctly spelled words) the spell-check program has an **accuracy of around 66 %** and close to **80 % for misspelled words with an edit distance equal to one.**

## Word Frequency Data Details

Read about the data, sources, processing raw word data, word frequency, probabilistic word correction etc. [here](https://github.com/madhav-datt/spell-check/blob/master/autocorrect-lib/AUTOCORR-lib.md).

## Known Issues

* No context based/grammar checking - 

    > **Their** is nothing to be done here.

    will be treated as a correct sentence and not be changed to

    > **There** is nothing to be done here.

* Words with edit distances greater than 1 cannot be corrected - _`oficiel`_, won't be corrected to _`official`_.
* Doesn't fix spelling errors due to missing spaces.

    > historicaldata

    will be found as misspelled, but won't be corrected to

    > historical data

* Please report bugs and issues [here](https://github.com/madhav-datt/spell-check/issues).
