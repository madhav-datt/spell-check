#
# Text extraction and pre-processing from PDFs
# Contains methods to extract text from PDFs into txt files
# Uses multi-threading for performance purposes
# Makes calls to C program for each text file to be spellchecked and autocorrected
#
# Copyright (C)   2016    Madhav Datt
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
#

from __future__ import print_function
import re
import sys
from math import ceil
from multiprocessing import Process
from os import listdir, system, popen, stat, chdir
from os.path import isfile, isdir, isabs, join

# Expected upper-bound conversion time per page in seconds
page_time = 0.457
default_kill_time = 30

def convert_single_file(directory, file):
    """
    Extract text from passed PDF file to text file in the same directory
    :param directory: Path to directory containing PDFs to be converted. Only absolute paths/hard links only
    :param file: Path to PDF file to be converted. Only absolute paths/hard links only
    :return: txt_file_path Path to converted text file
    """

    # Create name for file as "pdf_name.txt"
    file_name = file[0:len(file) - 4] + ".txt"
    pdf_file_path = join(directory, file)
    txt_file_path = join(directory, file_name)

    # Skip files if "pdf_name.txt" already exists
    # Assume previous conversion attempt successful
    if isfile(txt_file_path) and stat(txt_file_path).st_size > 0:
        return

    # Convert PDF to txt with call to pdf2txt.py script
    cmd_pdf2txt = "python2.7 src/to_text/pdf2txt.py -o {directory_path}/{txt_file_name} " \
                  "-t text {pdf_file_name} 2>/dev/null".format(directory_path=directory,
                                                               txt_file_name=file_name,
                                                               pdf_file_name=pdf_file_path)

    # Set kill-time limit based on number of pages
    cmd_num_page = "pdfinfo {pdf_file_name} | grep 'Pages' | awk '{statement}' 2>/dev/null".format(
        pdf_file_name=pdf_file_path,
        statement="{print $2}")

    try:
        kill_time = ceil(int(popen(cmd_num_page).read().strip()) * page_time)
    except ValueError:
        kill_time = default_kill_time

    try:
        # Run command on separate thread with call to system as a process
        converter_process = Process(target=system, name="pdf2txt_convert", args=(cmd_pdf2txt,))
        converter_process.start()

        # Wait a maximum of kill_time seconds for system command with call to script to execute
        converter_process.join(kill_time)

        # Terminate thread if active
        # Image based/without text layer conversion will be terminated
        if converter_process.is_alive():
            converter_process.terminate()
            converter_process.join()
            system("rm -f {txt_file_name}".format(txt_file_name=file_name))
    except Exception:
        pass

    return txt_file_path

def pdf_to_txt(directory):
    """
    Extract text from all PDFs from directory to text files in the same directory
    :param directory: Path to directory containing PDFs to be converted. Only absolute paths/hard links only
    """

    for file in listdir(directory):
        directory_file_path = join(directory, file)
        if isfile(directory_file_path) and re.match("^.*[\.]pdf$", file):
            convert_single_file(directory_file_path)


if __name__ == '__main__':
    # sys.argv[1] must be an absolute path to a file or directory to be spellchecked and autocorrected
    file_path = sys.argv[1]

    # List of files to be spellchecked and autocorrected
    files_list = []

    print("spellcheck Copyright (C) 2016 by Madhav Datt")
    print("Website: https://github.com/madhav-datt/spell-check")
    print("See the GNU General Public Licence for license details.")
    print("\n")

    # Batch processing mode - spellcheck multiple files together
    # Directory path is passed as parameter
    if isdir(file_path):
        pdf_to_txt_subdir(sys.argv)
        for file in listdir(file_path):
            full_file_path = join(directory, file)

            # Pick all txt files in directory
            if isfile(full_file_path) and re.match("^.*[\.]txt$", file):
                files_list.append(full_file_path)

    # Individual file spellcheck mode
    elif isfile(file_path):
        file_extension = file_path[-4:]
        if file_extension == ".pdf":
            files_list = [convert_single_file("/", file_path)]
        elif file_extension == ".txt":
            files_list = [file_path]
        else:
            print("File type not supported. Run spellcheck --help for usage instructions")
            sys.exit()

    # Unrecognized/unsupported argument passed
    else:
        print("Usage: spellcheck /path/to/file_name.txt")
        print("Usage: spellcheck /path/to/file_name.pdf")
        print("Usage: spellcheck /path/to/directory_name")

    chdir("../src/")
    for file in files_list:
        cmd_autocorrect = "./spellcheck {check_file}".format(check_file=file)
        print("File: ", file)
        system(cmd_autocorrect)
        print("")
