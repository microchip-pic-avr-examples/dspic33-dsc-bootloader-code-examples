# ====================================
# Argument Variables
# ====================================
# ------------------------------------
# Argument(1): compilerDir
# 
# Description:
#   Path to the compiler bin directory (absolute or relative)
#   e.g. - "C:\Program Files\Microchip\xc-dsc\v3.10\bin"
#   NOTE - surround with quotes if there are spaces in the path
# ------------------------------------
export compilerDir="$1"

# ------------------------------------
# Argument(2): projectDir
# 
# Description:
#   Path to the root project folder (absolute or relative)
#   e.g. - "C:\My Projects\app.X"
#   e.g. - ..\..
#   NOTE - surround with quotes if there are spaces in the path
# ------------------------------------
export projectDir="$2"

# ------------------------------------
# Argument(3): imageDir
# 
# Description:
#   Relative path from the project root to the .hex file
#   e.g. - dist\default\production
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

# ====================================
# Functions
# ====================================

# ====================================
# Script Main
# ====================================
if [ $isDebug = "true" ]; then
echo -e "$cyan NOTE: You have built in DEBUG mode, therefore a .hex file was not generated. If you would like to generate a .hex file, please build in production mode. For more information, please open MCC and click on the question mark symbol next to the 16-bit Bootloader Library under Resource Management. $reset"
exit 0
fi

# The format of the unified hex file name is imageName.unified.hex when the IDE generated file is imageName.hex
# For the unified hex file name, remove "hex" from the end of the file name and add "unified.hex" to the end
export unifiedImageName=$(echo "$imageName" | sed 's/.\{3\}$//')unified.hex

# Check to see if MPLAB X(R) XC16 or XCDSC is in use.
# Create OBJ_CPY variable to point to whichever compiler is in use.
export OBJ_CPY="$compilerDir/xc16-objcopy"
if [ -e "$compilerDir/xc-dsc-objcopy" ]; then
export OBJ_CPY="$compilerDir/xc-dsc-objcopy"
fi

# Fill in unimplemented memory locations in the application image, calculate the CRC of the application space, and place this value in the header 
hexmate r0-FFFFFFFF,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805080-81FFFF@805024w-4g5p814141AB

# Fill in unimplemented memory locations in the application header, calculate the CRC of the application header, and prepend this value to the header
hexmate r0-FFFFFFFF,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805004-80507F@805000w-4g5p814141AB

# Shift the image to address 0 and cut only the application portion
hexmate r805000-81FFFFs-805000,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/bin.hex"

# Create the .bin file for MDFU from the application .hex file 
$OBJ_CPY -I ihex -O binary "$projectDir/$imageDir/bin.hex" "$projectDir/$imageDir/app.X.production.bin"

if [ -f "$projectDir/$imageDir/$unifiedImageName" ]; then
    # Fill in unimplemented memory locations in the application image, calculate the CRC of the application space, and place this value in the header 
    hexmate r0-FFFFFFFF,"$projectDir/$imageDir/$unifiedImageName" -O"$projectDir/$imageDir/$unifiedImageName" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805080-81FFFF@805024w-4g5p814141AB

    # Fill in unimplemented memory locations in the application header, calculate the CRC of the application header, and prepend this value to the header
    hexmate r0-FFFFFFFF,"$projectDir/$imageDir/$unifiedImageName" -O"$projectDir/$imageDir/$unifiedImageName" -FILL=w1:0x00@0x805000:0x81FFFF +-CK=805004-80507F@805000w-4g5p814141AB
fi
