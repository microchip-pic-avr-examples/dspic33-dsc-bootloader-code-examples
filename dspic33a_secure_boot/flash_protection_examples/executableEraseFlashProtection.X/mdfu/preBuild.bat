@echo off

REM ====================================
REM Local Variables
REM ====================================
Set reset=[0m
Set red=[41m

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
REM Function: Error
REM 
REM Description: Prints out a message with RED background that an error has occurred while generating the keystore files and exits the script.
REM ------------------------------------
:Error
echo %red% An error has occurred. The keystore and key creation did not complete. %reset%
exit /b -1

REM ====================================
REM Script Main
REM ====================================
:scriptStart

REM Check that the required tools are installed
python -V || goto MissingPython
openssl version || goto MissingOpenSSL

REM If all required tools are present, create the required key/keystore files 

REM Creates the keystore and demo key pair if not already present 
python ..\..\..\boot.X\mdfu\create_demo_key_files.py || goto Error