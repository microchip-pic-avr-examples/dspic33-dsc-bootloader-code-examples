# ====================================
# Argument Variables
# ====================================
# ------------------------------------
# Argument(1): compilerDir
# 
# Description:
#   Path to the compiler bin directory (absolute or relative)
#   e.g. - "/opt/microchip/xc-dsc/v3.21/bin"
#   NOTE - surround with quotes if there are spaces in the path
# ------------------------------------
export compilerDir="$1"

# ------------------------------------
# Argument(2): projectDir
# 
# Description:
#   Path to the root project folder (absolute or relative)
#   e.g. - "home/user/MPLABXProjects/app.x"
#   e.g. - ../..
#   NOTE - surround with quotes if there are spaces in the path
# ------------------------------------
export projectDir="$2"

# ------------------------------------
# Argument(3): imageDir
# 
# Description:
#   Relative path from the project root to the .hex file
#   e.g. - dist/default/production
#   NOTE - surround with quotes if there are spaces in the path
# ------------------------------------
export imageDir="$3"

# ------------------------------------
# Argument(4): imageName
# 
# Description:
#   Name of the .hex file to use
#   e.g. - app.X.production.hex
# ------------------------------------
export imageName="$4"

# ------------------------------------
# Argument(5): isDebug
# 
# Description:
#   String - "true" or "false" string if the project is being build in debug mode
# ------------------------------------
export isDebug="$5"

# ====================================
# Local Variables
# ====================================
export reset="[0m"
export cyan="[46m"
export red="[41m"
export private_key_path="../../../../../boot.X/mdfu/keystore/private_key.pem"

# ====================================
# Functions
# ====================================

# ------------------------------------
# Function: MissingPython
# 
# Description: Prints out a message with RED background that python is not installed and exits the script.
# ------------------------------------
MissingPython() 
{
    echo $red Python was not found! Install Python3, add it to the system path, and close/reopen MPLAB X. $reset
    exit 1
}

# ------------------------------------
# Function: MissingOpenSSL
# 
# Description: Prints out a message with RED background that OpenSSL is not installed and exits the script.
# ------------------------------------
MissingOpenSSL() 
{
    echo $red OpenSSL was not found! Install OpenSSL, add it to the system path, and close/reopen MPLAB X. $reset
    echo $red For OpenSSL installs please check https://wiki.openssl.org/index.php/Binaries $reset
    exit 1
}

# ------------------------------------
# Function: MissingPrivateKey
# 
# Description: Prints out a message with RED background that the private key is not 
# present and exits the script. See the private_key_path local variable defined above 
# for the location where the private key is expected. 
# ------------------------------------
MissingPrivateKey()
{
    echo $red The private key was not found! $reset
    echo $red See the README.md at the root of this demo for additional details on key generation. $reset
    exit 1
}

# ------------------------------------
# Function: Error
# 
# Description: Prints out a message with RED background that an error has occurred processing and signing the header and exits the script.
# ------------------------------------
Error() 
{
    echo ${red} An error has occurred. The application header stuffing and signing process did not complete. $reset
    rm "$projectDir/$imageDir/signed_image.bin"
    exit 1
}

# ====================================
# Script Main
# ====================================
if [ $isDebug == "true" ]; then
    echo -e "$cyan NOTE: You have built in DEBUG mode, therefore a .hex file was not generated. If you would like to generate a .hex file, please build in production mode. For more information, please open MCC and click on the question mark symbol next to the 16-bit Bootloader Library under Resource Management. $reset"
    exit 0
fi

# Check that the required tools are installed
python3 -V || MissingPython
openssl version || MissingOpenSSL

# Check that the private key is present 
if ! [ -e $private_key_path ]; then
    MissingPrivateKey
fi

# If all required tools and files are present, continue with the signing process

# Check to see if MPLAB X(R) XC16 or XCDSC is in use.
# Create OBJ_CPY variable to point to whichever compiler is in use.
export OBJ_CPY="$compilerDir/xc16-objcopy"
if [ -e "$compilerDir/xc-dsc-objcopy" ]; then
    export OBJ_CPY="$compilerDir/xc-dsc-objcopy"
fi

# Fill in unimplemented memory locations in the application space 
hexmate r80B000-0x844FFF,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/filled.hex" -FILL=w1:0x00@0x80B000:0x844FFF || Error

# generate a .hex file for just the header (offset to address 0)
hexmate r80B060-0x80B1FFs-80B060,"$projectDir/$imageDir/filled.hex" -O"$projectDir/$imageDir/header.hex" || Error

# Generate the binary file of the header
$OBJ_CPY -I ihex -O binary "$projectDir/$imageDir/header.hex" "$projectDir/$imageDir/header.bin" || Error

# generate a .hex file for just the code (offset to address 0)
hexmate r80B200-0x844FFFs-80B200,"$projectDir/$imageDir/filled.hex" -O"$projectDir/$imageDir/data.hex" || Error

# Generate the binary file of the image data
$OBJ_CPY -I ihex -O binary "$projectDir/$imageDir/data.hex" "$projectDir/$imageDir/data.bin" || Error

# generate a .hex file for just the header (offset to address 0)
hexmate r80B000-0x844FFFs-80B000,"$projectDir/$imageDir/filled.hex" -O"$projectDir/$imageDir/presigned_image.hex" || Error

# Generate the binary file of the header
$OBJ_CPY -I ihex -O binary "$projectDir/$imageDir/presigned_image.hex" "$projectDir/$imageDir/signed_image.bin" || Error

echo ..............................
echo Hashing image data
echo ..............................
# Hash the application code
openssl dgst -sha384 "$projectDir/$imageDir/data.bin" || Error
openssl dgst -sha384 -binary -out "$projectDir/$imageDir/data.hash.bin" "$projectDir/$imageDir/data.bin" || Error

# inject the code digest into the application header
python3 update_header_value.py update_header_value --header_bin "$projectDir/$imageDir/header.bin" --type_code 0x3 --value_bin "$projectDir/$imageDir/data.hash.bin" || Error

echo Successfully injected the digest of the application code into the header

echo ..............................
echo Generating signature for image header
echo ..............................
# Sign application header binary file
openssl dgst -sha384 "$projectDir/$imageDir/header.bin" || Error
openssl dgst -sha384 -sign $private_key_path -out "$projectDir/$imageDir/signature.der" "$projectDir/$imageDir/header.bin" || Error

# Export signature value
python3 signing_tool.py -export "$projectDir/$imageDir/signature.der" "$projectDir/$imageDir/signature.bin" 384 || Error

echo Successfully generated signature file: "$projectDir/$imageDir/signature.bin"

echo .

echo Signature:
openssl asn1parse -in "$projectDir/$imageDir/signature.bin" -inform der
echo .

echo ..............................
echo Creating final binary file
echo ..............................

python3 bin_tool.py inject_bin --out_bin "$projectDir/$imageDir/signed_image.bin" --offset 0x0 --in_bin "$projectDir/$imageDir/signature.bin" || Error
python3 bin_tool.py inject_bin --out_bin "$projectDir/$imageDir/signed_image.bin" --offset 0x60 --in_bin "$projectDir/$imageDir/header.bin" || Error
python3 bin_tool.py inject_bin --out_bin "$projectDir/$imageDir/signed_image.bin" --offset 0x200 --in_bin "$projectDir/$imageDir/data.bin" || Error

echo Fully signed image generated: "$projectDir/$imageDir/signed_image.bin"
# Add the completed header back into the application .bin file 
# python3 bin_tool.py inject_bin --out_bin "$projectDir/$imageDir/app.bin" --offset 0x7F3000 --in_bin "$projectDir/$imageDir/header.bin" --addr 0x80B000 || Error

# Clean up temp files 
rm "$projectDir/$imageDir/header.hex" || Error
rm "$projectDir/$imageDir/header.bin" || Error

rm "$projectDir/$imageDir/data.hex" || Error
rm "$projectDir/$imageDir/data.bin" || Error
rm "$projectDir/$imageDir/data.hash.bin" || Error

rm "$projectDir/$imageDir/signature.der" || Error
rm "$projectDir/$imageDir/signature.bin" || Error

rm "$projectDir/$imageDir/filled.hex" || Error
rm "$projectDir/$imageDir/presigned_image.hex" || Error