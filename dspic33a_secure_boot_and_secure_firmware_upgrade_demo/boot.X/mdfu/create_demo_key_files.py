import os
import subprocess

keystore_directory = "keystore"
keystore_file = "keystore.json"
keystore_path = os.path.join(keystore_directory, keystore_file)

private_key_file = "private_key.pem"
private_key_path = os.path.join(keystore_directory, private_key_file)

public_key_file = "public_key.pem"
public_key_path = os.path.join(keystore_directory, public_key_file)

keystore_json = """
{
    "format": "1.0.0",
    "keys": [
        {
            "name": "Executable Partition Code Signing Public Key",
            "type": "secp384r1",
            "path": "public_key.pem",
            "demo": "true"
        }
    ]
}
"""

def path_exists(path):
    """
    Check if the provided file path or directory path exists.

    Parameters:
    str: path to check

    Returns:
    bool: True if the file or directory exists, False otherwise.
    """
    return os.path.exists(path)

def run(command):
    try:
        subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        print(f"An error occurred: {e.stderr.decode().strip()}") 

def create_private_key():
    """
    Create a new p384 private key file

    Parameters:
    None

    Returns:
    None
    """

    command = [
        "openssl", "ecparam", "-genkey", "-name", "secp384r1", "-noout", "-out", private_key_path
    ]

    run(command)

def create_public_key():
    """
    Create a associated public key file from previously created private key file

    Parameters:
    None

    Returns:
    None
    """

    command = [
        "openssl", "ec", "-in", private_key_path, "-pubout", "-out", public_key_path
    ]

    run(command)

def create_keystore_json():
    """
    Create a JSON that describes the keystore structure

    Parameters:
    None

    Returns:
    None
    """

    with open(keystore_path, 'w') as file:
        file.write(keystore_json)

def main():
    if not path_exists(keystore_path):
        print("Creating a new demo key pair and keystore")
        
        if not path_exists(keystore_directory):
            os.mkdir(keystore_directory)

        create_private_key()
        create_public_key()
        create_keystore_json()

if __name__ == "__main__":
    main()