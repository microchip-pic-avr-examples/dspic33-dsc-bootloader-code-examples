#!/bin/python3

"""
This script is used to take input .bin files and place them into an output .bin at a specific offset: 
    * The signature of the application header
    * The application header information
    * The all remaining appliation data
""" 
import os
import sys
import traceback
import argparse
import subprocess

def file_exists(filename):
    if os.path.isfile(filename):
        return True
    else:
        print("File does not exist: " + filename +'\n')
        return False

def inject_data(bin_file, offset, data):
    """Insert data bytes into a bin_file.

    Keyword arguments:
    bin_file -- bin file containing the data to be hashed
    offset -- offset in the output bin where to write the incoming bin data
    data -- the byte data to be injected
    """
    if file_exists(bin_file):
        with open(bin_file, 'r+b') as file:
            file.seek(offset)
            file.write(data)
    else:
        sys.exit(-1)

def read_data(bin_file):
    """Read the data from a bin file and return the byte object.  

    Keyword arguments:
    bin_file -- bin file containing the data to be read
    """
    if file_exists(bin_file):
        with open(bin_file, 'rb') as file:
            bin_data = file.read()
            return bin_data
    else:
        sys.exit(-1)

def perform_inject_bin(args):
    """Inject data from one .bin file into another.

    Keyword arguments:
    args -- args passed in via command line 
    """

    injection_data = read_data(args.in_bin)
    inject_data(args.out_bin, args.offset, injection_data)
    
# Main
if __name__ == "__main__":

    # Defined this class to support verbose help on argument error.
    class MyParser(argparse.ArgumentParser):
        def error(self, message):
            sys.stderr.write('error: %s\n' % message)
            self.print_help()
            sys.exit(2)

    try:
        # Specify arguments.
        parser = MyParser(description=__doc__)
        subparsers = parser.add_subparsers(dest='choice')

        # subparser for .bin file combination   
        inject_bin_parser = subparsers.add_parser('inject_bin', help='Inject the data from the second .bin file into the first .bin file at the specified address.')
        inject_bin_parser.add_argument('--out_bin', help='The .bin file into which data will be injected.', required=True)
        inject_bin_parser.add_argument('--offset', help='The code offset for bin file addressing.', type=lambda x: int(x,16), required=True)
        inject_bin_parser.add_argument('--in_bin', help='The .bin file containing data to be injected.', required=True)
       
        args = parser.parse_args()

        if args.choice == 'inject_bin':
            perform_inject_bin(args)
            sys(exit(0))

    except Exception as ex:
        print ("\n", traceback.format_exc())
        print(sys.argv)
        parser.print_help()

        sys.exit(1)
