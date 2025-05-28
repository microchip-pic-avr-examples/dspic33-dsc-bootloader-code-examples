@echo off

REM ====================================
REM Argument Variables
REM ====================================
REM ------------------------------------
REM Argument(1): compilerDir
REM 
REM Description:
REM   Path to the compiler bin directory (absolute or relative)
REM   e.g. - "C:\Program Files\Microchip\xc-dsc\v3.10\bin"
REM   NOTE - surround with quotes if there are spaces in the path
REM ------------------------------------
Set compilerDir=%1

REM ------------------------------------
REM Argument(2): projectDir
REM 
REM Description:
REM   Path to the root project folder (absolute or relative)
REM   e.g. - "C:\My Projects\app.X"
REM   e.g. - ..\..
REM   NOTE - surround with quotes if there are spaces in the path
REM ------------------------------------
Set projectDir=%~2

REM ------------------------------------
REM Argument(3): imageDir
REM 
REM Description:
REM   Relative path from the project root to the .hex file
REM   e.g. - dist\default\production
REM   NOTE - surround with quotes if there are spaces in the path
REM ------------------------------------
Set imageDir=%~3

REM ------------------------------------
REM Argument(4): imageName
REM 
REM Description:
REM   Name of the .hex file to use
REM   e.g. - app.X.production.hex
REM ------------------------------------
Set imageName=%~4

REM ------------------------------------
REM Argument(5): isDebug
REM 
REM Description:
REM   String - "true" or "false" string if the project is being build in debug mode
REM ------------------------------------
Set isDebug=%5

REM ====================================
REM Local Variables
REM ====================================
Set reset=[0m
Set cyan=[46m
Set red=[41m
Set private_key_path="..\..\..\boot.X\mdfu\keystore\private_key.pem"

REM Jump to start of the script logic
goto scriptStart

REM ====================================
REM Functions
REM ====================================

REM ------------------------------------
REM Function: MissingPython
REM 
REM Description: Prints out a message with RED background that python is not installed and exits the script.
REM ------------------------------------
:MissingPython
echo %red% Python was not found! Install Python3, add it to the system path, and close/reopen MPLAB X. %reset%
exit /b -1

REM ------------------------------------
REM Function: MissingOpenSSL
REM 
REM Description: Prints out a message with RED background that OpenSSL is not installed and exits the script.
REM ------------------------------------
:MissingOpenSSL
echo %red% OpenSSL was not found! Install OpenSSL, add it to the system path, and close/reopen MPLAB X. %reset%
echo %red% For OpenSSL installs please check https://wiki.openssl.org/index.php/Binaries %reset%
exit /b -1

REM ------------------------------------
REM Function: MissingPrivateKey
REM 
REM Description: Prints out a message with RED background that the private key is not 
REM present and exits the script. See the private_key_path local variable defined above 
REM for the location where the private key is expected. 
REM ------------------------------------
:MissingPrivateKey
echo %red% The private key was not found! %reset%
echo %red% See the README.md at the root of this demo for additional details on key generation. %reset%
exit /b -1

REM ------------------------------------
REM Function: Error
REM 
REM Description: Prints out a message with RED background that an error has occurred processing and signing the header and exits the script.
REM ------------------------------------
:Error
echo %red% An error has occurred. The executable header stuffing and signing process did not complete. %reset%
del "%projectDir%\%imageDir%\signed_image.bin"
exit /b -1

REM ====================================
REM Script Main
REM ====================================
:scriptStart
if %isDebug%=="true" (
echo %cyan% NOTE: You have built in DEBUG mode, therefore a .hex file was not generated. If you would like to generate a .hex file, please build in production mode. For more information, please open MCC and click on the question mark symbol next to the 16-bit Bootloader Library under Resource Management. %reset%
exit 0
)

REM Check that the required tools are installed
python -V || goto MissingPython
openssl version || goto MissingOpenSSL

REM Check that the private key is present 
if not EXIST %private_key_path% (
goto MissingPrivateKey
)

REM If all required tools and files are present, continue with the signing process

REM Check to see if MPLAB X(R) XC16 or XCDSC is in use.
REM Create OBJ_CPY variable to point to whichever compiler is in use.
Set OBJ_CPY=%compilerDir%\xc16-objcopy.exe
if EXIST %compilerDir%\xc-dsc-objcopy.exe (
Set OBJ_CPY=%compilerDir%\xc-dsc-objcopy.exe
)

REM Fill in unimplemented memory locations in the executable space 
hexmate r80B000-0x844FFF,"%projectDir%\%imageDir%\%imageName%" -O"%projectDir%\%imageDir%\filled.hex" -FILL=w1:0x00@0x80B000:0x844FFF || goto Error

REM generate a .hex file for just the header (offset to address 0)
hexmate r80B060-0x80B1FFs-80B060,"%projectDir%\%imageDir%\filled.hex" -O"%projectDir%\%imageDir%\header.hex" || goto Error

REM Generate the binary file of the header
%OBJ_CPY% -I ihex -O binary "%projectDir%\%imageDir%\header.hex" "%projectDir%\%imageDir%\header.bin" || goto Error

REM generate a .hex file for just the code (offset to address 0)
hexmate r80B200-0x844FFFs-80B200,"%projectDir%\%imageDir%\filled.hex" -O"%projectDir%\%imageDir%\data.hex" || goto Error

REM Generate the binary file of the image data
%OBJ_CPY% -I ihex -O binary "%projectDir%\%imageDir%\data.hex" "%projectDir%\%imageDir%\data.bin" || goto Error

REM generate a .hex file for just the header (offset to address 0)
hexmate r80B000-0x844FFFs-80B000,"%projectDir%\%imageDir%\filled.hex" -O"%projectDir%\%imageDir%\presigned_image.hex" || goto Error

REM Generate the binary file of the header
%OBJ_CPY% -I ihex -O binary "%projectDir%\%imageDir%\presigned_image.hex" "%projectDir%\%imageDir%\signed_image.bin" || goto Error

echo ..............................
echo Hashing image data
echo ..............................
REM Hash the executable code
openssl dgst -sha384 "%projectDir%\%imageDir%\data.bin" || goto Error
openssl dgst -sha384 -binary -out "%projectDir%\%imageDir%\data.hash.bin" "%projectDir%\%imageDir%\data.bin" || goto Error

REM inject the code digest into the executable header
python update_header_value.py update_header_value --header_bin "%projectDir%\%imageDir%\header.bin" --type_code 0x3 --value_bin "%projectDir%\%imageDir%\data.hash.bin" || goto Error
echo.

echo ..............................
echo Generating signature for image header
echo ..............................
REM Sign executable header binary file
openssl dgst -sha384 "%projectDir%\%imageDir%\header.bin" || goto Error
openssl dgst -sha384 -sign %private_key_path% -out "%projectDir%\%imageDir%\signature.der" "%projectDir%\%imageDir%\header.bin" || goto Error

REM Export signature value
python signing_tool.py -export "%projectDir%\%imageDir%\signature.der" "%projectDir%\%imageDir%\signature.bin" 384 || goto Error
echo.

echo Successfully generated signature file: "%projectDir%\%imageDir%\signature.bin"
echo.

echo Signature:
openssl asn1parse -in "%projectDir%\%imageDir%\signature.der" -inform DER
echo.

echo ..............................
echo Creating final binary file
echo ..............................

python bin_tool.py inject_bin --out_bin "%projectDir%\%imageDir%\signed_image.bin" --offset 0x0 --in_bin "%projectDir%\%imageDir%\signature.bin" || goto Error
python bin_tool.py inject_bin --out_bin "%projectDir%\%imageDir%\signed_image.bin" --offset 0x60 --in_bin "%projectDir%\%imageDir%\header.bin" || goto Error
python bin_tool.py inject_bin --out_bin "%projectDir%\%imageDir%\signed_image.bin" --offset 0x200 --in_bin "%projectDir%\%imageDir%\data.bin" || goto Error

echo Fully signed image generated: "%projectDir%\%imageDir%\signed_image.bin"

REM Clean up temp files 
del "%projectDir%\%imageDir%\header.hex" || goto Error
del "%projectDir%\%imageDir%\header.bin" || goto Error

del "%projectDir%\%imageDir%\data.hex" || goto Error
del "%projectDir%\%imageDir%\data.bin" || goto Error
del "%projectDir%\%imageDir%\data.hash.bin" || goto Error

del "%projectDir%\%imageDir%\signature.der" || goto Error
del "%projectDir%\%imageDir%\signature.bin" || goto Error

del "%projectDir%\%imageDir%\filled.hex" || goto Error
del "%projectDir%\%imageDir%\presigned_image.hex" || goto Error