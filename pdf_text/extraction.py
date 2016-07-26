#
# Text extraction and pre-processing from PDFs
# Contains methods to extract from PDFs, tokenize and POS tag sentences
# Uses multi-threading for performance purposes
#
# Copyright (C)   2016    Madhav Datt
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
#

import re
import sys
from math import ceil
from multiprocessing import Process
from os import listdir, system, popen, stat
from os.path import isfile, isdir, isabs, join

# Expected upper-bound conversion time per page in seconds
page_time = 0.457
default_kill_time = 30


def pdf_to_txt(directory_path):
    """
    Extract text from all PDFs from directory to text files in the same directory
    :param directory: Path to directory containing PDFs to be converted. Only absolute paths/hard links only
    :return file_list: List of paths to created txt files
    :return count: Number of files converted t.o text
    """

    # Get list of all PDF files in directory
    file_list = []
    directory = directory_path[1]

    for file in listdir(directory):
        if isfile(join(directory, file)) and re.match("^.*[\.]pdf$", file):
            # Create name for file as "pdf_name.txt"
            file_name = file[0:len(file) - 4] + ".txt"
            pdf_file_path = join(directory, file)
            txt_file_path = join(directory, file_name)
            file_list.append(txt_file_path)

            # Skip files if "pdf_name.txt" already exists
            # Assume previous conversion attempt successful
            if isfile(txt_file_path) and stat(txt_file_path).st_size > 0:
                continue

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
                    file_list.remove(txt_file_path)
                    system("rm -f {txt_file_name}".format(txt_file_name=file_name))
            except Exception:
                pass

    return file_list


if __name__ == '__main__':
    files_converted = pdf_to_txt_subdir(sys.argv)
    print(files_converted)
    sys.exit()
