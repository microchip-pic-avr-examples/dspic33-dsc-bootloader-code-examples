#!/bin/python3

# This file runs the postbuild process to generate and insert CRC and signature data, and create
# binary output for M-CRPS-compliant projects.

import os
import sys
import traceback
import argparse
import asndecode

# Add a system path for import as the asndecode script is shared.
sys.path.append("..")

def console_print(quiet, msg):
    if quiet is False:
        print(msg)

def file_exists(file_name):
    if os.path.isfile(file_name):
        return True
    else:
        print("File does not exist: " + file_name +'\n')
        return False

def find_signature(data, data_len, quiet=True):
    """Find and return the signature data in the ASN.1 format datastream.

    Keyword arguments:
    data -- ASN.1 format datastream containing the signature.
    data_len -- length of the datastream passed in.
    quiet -- boolean indicating if the console should print out debug messages. 
    """
    sig_data = None
    sig_r = None
    sig_s = None
    offset = 0

    console_print(quiet, "Raw signature: %s" % ' '.join('%02x' % x for x in data))

    while offset < data_len:
        data_type, size, value = asndecode.getobject(data[offset:], quiet)

        # Advance by the amount of data parsed.
        offset += size

        # Capture the data for data type 0x02 (integer).
        if data_type == 0x02:
            # This is part of the signature.  If the length is odd, there is a 00 pad
            # in front of the r or s portion.
            data_offset = 0
            if (size & 1) != 0:
                data_offset = 1

            if sig_r is None:
                sig_r = value[data_offset:]
            else:
                sig_s = value[data_offset:]

    console_print(quiet, "Signature R-value: %s" % ' '.join('%02x' % x for x in sig_r))
    console_print(quiet, "Signature S-value: %s" % ' '.join('%02x' % x for x in sig_s))

    if sig_r is not None and sig_s is not None:
        sig_data = sig_r + sig_s

    return sig_data

def create_export_file(der_file, bin_file, sha_size, quiet=True):
    """Parse out the signature from the signature file and store the 
    signature in binary file format.

    Keyword arguments:
    der_file -- filename of the DER file containing signature to convert to binary.
    bin_file -- The bin file containing the binary version of the signature. 
    sha_size -- 256 or 384 depending on what is needed.
    quiet -- boolean indicating if the console should print out debug messages.
    """
    console_print(quiet, "Exporting %s signature..." % sha_size)

    signature = None
    result = 0

    # Generate the hash over the binary file.
    if not file_exists(der_file):
        print("ERROR Can not open signature file. \n")
        return -1
    
    if (( sha_size=='256') or (sha_size=='384')):
        with open(der_file, 'rb') as f:
            sig_bytes = f.read()
        f.close()        

        signature = find_signature(sig_bytes, len(sig_bytes),quiet)
        sig_len = len(signature)
       
        # Verify that the length of the signature matches the expected length for the sha type
        if ( ((sha_size == '256') and (sig_len == 64)) or ((sha_size == '384') and (sig_len == 96)) ) :
            with open(bin_file, 'wb+') as f:
                f.write(signature)
            f.close()        
        else:
            # The signature is the hash was wrong.  Give user some debug.
            if (sha_size == '256'):
                print("ERROR: Sha256 was expecting signature length of 64.  Actual length is %s\n" % (sig_len))
            else:    
                print("ERROR: Sha384 was expecting signature length of 96.  Actual length is %s\n" % (sig_len))
            result = -1

    else:
        # The signature is the hash.
        print("ERROR: Unknown signature length: %s" % (sha_size))
        result = -1
 
    return result

# Main
if __name__ == "__main__":

    # Defined this class to support verbose help on argument error.
    class MyParser(argparse.ArgumentParser):
        def error(self, message):
            sys.stderr.write('error: %s\n' % message)
            self.print_help()
            sys.exit(2)

    def exit_prog(msg):
        print(msg)
        sys.exit(1)

    try:
        # Specify arguments.
        parser = MyParser(description=__doc__)

        #The user must select one of the 3 main options.
        main_group = parser.add_mutually_exclusive_group(required=True)
        main_group.add_argument('-export', '--export', help='Export der signature to bin file -export <DER_filename> <bin_filename> <ECDSA_Length 256/384> ', type=str, nargs=3, required=False)

        parser.add_argument('-v', '--verbose', help='Verbose Log ',  default=False, action='store_true', required=False)
       
        options = parser.parse_args()
        if options.verbose:
            print(sys.argv)
            
        if options.export is not None:
            sys.exit(create_export_file(options.export[0] , options.export[1],  options.export[2], not options.verbose))
 
        # if neither of the two cases above were run, print out error
        print(sys.argv)
        parser.print_help()
    
        sys.exit(-1)

    except Exception as ex:
        print ("\n", traceback.format_exc())
        print(sys.argv)
        parser.print_help()

        sys.exit(1)
