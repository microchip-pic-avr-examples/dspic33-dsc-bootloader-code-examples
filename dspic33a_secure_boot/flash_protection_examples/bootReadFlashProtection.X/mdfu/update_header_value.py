#!/bin/python3

"""
This script fills the application partition during the post build process with: 
    * The SHA(256/384) hash of the application code space
    * The public key used for signature verification
    * The signature of the application header
""" 
import os
import sys
import traceback
import argparse
import subprocess
import struct


def file_exists(filename):
    if os.path.isfile(filename):
        return True
    else:
        print("File does not exist: " + filename +'\n')
        return False

def get_type_code_offset(header, type_code):
    """ Get the offset into the header in bytes that the input type code

    Keyword arguments:
    header -- A raw byte object of the header file to be scanned
    type_code -- This is the type that we're searching the header for
    type_code_offset -- Returns the offset in bytes to get back the matching entry 
    """
    # Define the format for 32-bit integer values
    int_format = 'I'  # Unsigned 32-bit integer
    int_size = struct.calcsize(int_format)

    if type_code == 0 :
        print(f"Type code 0 end of header has no value")
        sys.exit(-1)
    # Search TLV list for the matching type code
    type_code_offset = 0
    while type_code_offset < len(header) - int_size * 2:
        # Read the type code at the current offset
        current_type_code = struct.unpack_from(int_format, header, type_code_offset)[0]
        if current_type_code == type_code:
            break
        elif current_type_code != 0x00000000:
            # Get the length field for this value which will be +4 bytes from the current type_code_offset
            current_length = struct.unpack_from(int_format, header, (type_code_offset+4))[0]
            # Add the length of the value +4 bytes for the current type_code +4 bytes for the length field
            type_code_offset = type_code_offset + 8 + (current_length)
        else:
            print(f"Type code not found in header.")
            sys.exit(-1)
    return type_code_offset

def check_size(header, type_code_offset, new_value):
    # Define the format for 32-bit integer values
    int_format = 'I'  # Unsigned 32-bit integer
    int_size = struct.calcsize(int_format)

    # Length of current value is +4 bytes from the type_code_offset
    current_length = struct.unpack_from(int_format, header, (type_code_offset+4))[0]
    if(len(new_value)!=current_length):
        print(f"Value length doesn't match header length allocated")
        sys.exit(-1)

def get_value_offset(type_code_offset):
    """Get the offset into the header to the value we're replacing.

    type_code_offset -- This is the offset to the type code for the value entry. 
    """
    # The value offset will be a hex string equal to the typecode offset +4 for the type code, and +4 for the length
    return hex(type_code_offset + 8)

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

def perform_update_header_value(args):
    """Inject data from one .bin file into another.

    Keyword arguments:
    args -- args passed in via command line 
    """
    new_value = read_data(args.value_bin)
    header = read_data(args.header_bin)
    type_code_offset = get_type_code_offset(header, args.type_code)
    check_size(header, type_code_offset, new_value)
    value_offset = get_value_offset(type_code_offset)
    subprocess.run(['python', 'bin_tool.py', 'inject_bin', '--out_bin', args.header_bin, '--offset', value_offset,  '--in_bin', args.value_bin], 
            check=True,
            capture_output=True,
            text=True
        )

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
        update_header_parser = subparsers.add_parser('update_header_value', help='Update the data from the input value .bin file into the header .bin file for the specified type code entry.')
        update_header_parser.add_argument('--header_bin', help='The header .bin file that the new value will be written into.', required=True)
        update_header_parser.add_argument('--type_code', help='The type code of the header value to be updated.', type=lambda x: int(x,16), required=True)
        update_header_parser.add_argument('--value_bin', help='The new value in a .bin file format.', required=True)
       
        args = parser.parse_args()

        if args.choice == 'update_header_value':
            perform_update_header_value(args)
            sys(exit(0))

    except Exception as ex:
        print ("\n", traceback.format_exc())
        print(sys.argv)
        parser.print_help()

        sys.exit(1)
