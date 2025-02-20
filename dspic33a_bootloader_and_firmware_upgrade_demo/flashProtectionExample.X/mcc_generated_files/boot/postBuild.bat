Set reset=[0m
Set cyan=[46m
Set compilerDir=%1
Set projectDir=%~2
Set imageDir=%~3
Set imageName=%~4
Set isDebug=%5

REM The format of the unified hex file name is imageName.unified.hex when the IDE generated file is imageName.hex
REM For the unified hex file name, remove "hex" from the end of the file name and add "unified.hex" to the end
Set unifiedImageName=%imageName:~0,-3%unified.hex

if %isDebug%=="true" (
echo %cyan% NOTE: You have built in DEBUG mode, therefore a .hex file was not generated. If you would like to generate a .hex file, please build in production mode. For more information, please open MCC and click on the question mark symbol next to the 16-bit Bootloader Library under Resource Management. %reset%
exit 0
)

Set OBJ_CPY=%compilerDir%\xc16-objcopy.exe
if EXIST %compilerDir%\xc-dsc-objcopy.exe (
    Set OBJ_CPY=%compilerDir%\xc-dsc-objcopy.exe
)

REM Fill in unimplemented memory locations in the application image, calculate the CRC of the application space, and place this value in the header 
hexmate r0-FFFFFFFF,"%projectDir%\%imageDir%\%imageName%" -O"%projectDir%\%imageDir%\%imageName%" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805080-81FFFF@805024w-4g5p814141AB

REM Fill in unimplemented memory locations in the application header, calculate the CRC of the application header, and prepend this value to the header
hexmate r0-FFFFFFFF,"%projectDir%\%imageDir%\%imageName%" -O"%projectDir%\%imageDir%\%imageName%" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805004-80507F@805000w-4g5p814141AB

REM Shift the image to address 0 and cut only the application portion
hexmate r805000-81FFFFs-805000,"%projectDir%\%imageDir%\%imageName%" -O"%projectDir%\%imageDir%\bin.hex"

REM Create the .bin file for MDFU from the application .hex file 
%OBJ_CPY% -I ihex -O binary "%projectDir%\%imageDir%\bin.hex" "%projectDir%\%imageDir%\app.X.production.bin"

if exist "%projectDir%\%imageDir%\%unifiedImageName%" (
    REM Fill in unimplemented memory locations in the application image, calculate the CRC of the application space, and place this value in the header 
    hexmate r0-FFFFFFFF,"%projectDir%\%imageDir%\%unifiedImageName%" -O"%projectDir%\%imageDir%\%unifiedImageName%" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805080-81FFFF@805024w-4g5p814141AB

    REM Fill in unimplemented memory locations in the application header, calculate the CRC of the application header, and prepend this value to the header
    hexmate r0-FFFFFFFF,"%projectDir%\%imageDir%\%unifiedImageName%" -O"%projectDir%\%imageDir%\%unifiedImageName%" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805004-80507F@805000w-4g5p814141AB
)
