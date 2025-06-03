import os
import sys
import json
import subprocess

keystore_directory = "keystore"
keystore_file = "keystore.json"
keystore_path = os.path.join(keystore_directory, keystore_file)

generated_keystore_directory = "../"
generated_keystore_file = "generated_keystore.S"
generated_keystore_path = os.path.join(generated_keystore_directory, generated_keystore_file)

DEMO_KEY_WARNING = "NOTE - THE GENERATED KEYS FOR THIS DEMO ARE NOT SECURE AND ARE FOR DEMO PURPOSES ONLY!! Do not use these keys in production. For production purposes, generate secure key pairs using a secure key generation system, such as an HSM. See demo documentation for more details."

YELLOW = '\033[43m'
RED = '\033[41m'
RESET = '\033[0m'

generated_keystore_description = """
/* 
********************************************************************************
*        !!THIS FILE IS AUTO GENERATED AND SHOULD NOT BE MANUALLY EDITED!!     *
*                                                                              *
* For details on how to update the key contained in this file, see the         *
* README.md located at the root of this demo.                                  *
********************************************************************************

File auto-generated based on the parameters contained in keystore.json. 
Contains the public key used for verifying the authenticity and integrity of the 
signature used to sign the application code. 
 */
"""
config_file_includes = '#include "mdfu/mdfu_config.h"'
section_directive = '.section .public_key, code, address(MDFU_CONFIG_KEYSTORE_DATA_ORIGIN), keep'

def file_exists(file_path):
    """
    Check if the provided file exists.

    Parameters:
    file path: path to the file

    Returns:
    bool: True if the file exists, False otherwise.
    """
    return os.path.exists(file_path)

def run(command):
    try:
        return subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    except subprocess.CalledProcessError as e:
        print(f"An error occurred: {e.stderr.decode().strip()}") 

def get_public_key_hex(pem_file):
    """
    Extract the public key to a byte list of hexadecimal values. 

    Parameters:
    pem file: public key in .pem format

    Returns:
    list: public key as a list of byte values
    """
    command = [
        "openssl", "pkey", "-pubin", "-in", pem_file, "-outform", "DER", "-out", "ec_temp.der"
    ]

    run(command)

    with open('ec_temp.der', 'rb') as f:
        der_data = f.read()
    
    compression_byte = der_data[23]
    pub_key_vals = der_data[23:]
    os.remove('ec_temp.der')

    if compression_byte != 0x04:
       print("Compressed public keys are not currently supported. Please provide an uncompressed key.")
       sys.exit(-1) 

    return list(pub_key_vals)

def get_key_field(index, field):
    """
    Get the specified field of public key file as specified by the keystore.

    Parameters:
    index: index of the key 
    field: field name of the item to get

    Returns:
    str: field value
    """
    field_value = ""

    with open(keystore_path, 'r') as file:
        data = json.load(file)
        keys = data.get('keys', []);

        if index < len(keys):
            key = keys[index]
            field_value = key.get(field, "")

    return field_value

def get_key_type(index):
    return get_key_field(index,"type");

def get_key_name(index):
    return get_key_field(index,"name");

def get_key_path(index):
    return get_key_field(index,"path");

def get_key_demo(index):
    return get_key_field(index,"demo");

def write_hex_to_s_file(byte_list, file_name):
    """
    Write a byte list of hex values to a .S file using .long directive. 

    Parameters:
    byte list: list of byte values to write to the file
    file_name: name of the .S file to write to

    Returns:
    None
    """
    with open(file_name, 'w') as file:
        file.write(generated_keystore_description + "\n")
        file.write(config_file_includes + "\n")
        file.write(section_directive + "\n")
        file.write("\n");

        key_index = 0;
        file.write("    ; " + get_key_name(key_index) + " (" + get_key_type(key_index) + ")\n");
        
        if get_key_demo(key_index) == "true":
            file.write(f"#warning \"{DEMO_KEY_WARNING}\"\n")
            print(f"{YELLOW}{DEMO_KEY_WARNING}{RESET}")

        # Iterate over the byte list in chunks of 4 bytes
        for i in range(0, len(byte_list), 4):
            # Extract up to 4 bytes, padding with zeros if necessary
            chunk = byte_list[i:i+4]
            while len(chunk) < 4:
                chunk.append(0)

            # Combine the bytes into a single 32-bit value in little-endian format
            long_value = (chunk[3] << 24) | (chunk[2] << 16) | (chunk[1] << 8) | chunk[0]
            file.write(f"    .long 0x{long_value:08x}\n")

def exit(error_message):
    sys.exit(f"{RED}{error_message}{RESET}");

def generate_keystore_source():
    """
    Generate the keystore source file from the associated keystore files.

    Parameters:
    None

    Returns:
    None
    """
    public_key_path = os.path.join(keystore_directory, get_key_path(0))

    if file_exists(public_key_path):
        write_hex_to_s_file(get_public_key_hex(public_key_path), generated_keystore_path)
    else:
        exit("The public key file specified by the keystore does not exist. Exiting.")

def main():
    if file_exists(keystore_path):
        print("Creating the generated keystore.")
        generate_keystore_source()
    else:
        exit("The keystore file does not exist. Exiting.")

if __name__ == "__main__":
    main()