export reset="[0m"
export cyan="[46m"
export compilerDir="$1"
export projectDir="$2"
export imageDir="$3"
export imageName="$4"
export isDebug="$5"
if [ "$isDebug" = "true" ]; then
echo -e "$cyan NOTE: You have built in DEBUG mode, therefore a .hex file was not generated. If you would like to generate a .hex file, please build in production mode. For more information, please open MCC and click on the question mark symbol next to the 16-bit Bootloader Library under Resource Management. $reset"
exit 0
fi
export compilerDir="$1"
export projectDir="$2"
export imageDir="$3"
export imageName="$4"
export OBJ_CPY="$compilerDir/xc16-objcopy"
if [ -e "$compilerDir/xc-dsc-objcopy" ]; then
export OBJ_CPY="$compilerDir/xc-dsc-objcopy"
fi
# Blank signature location
hexmate r0-EFFF,"$projectDir/$imageDir/$imageName" rF080-FFFFFFFF,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/temp_original_copy.X.production.hex" -FILL=w1:0x00,0x00,0x00,0x00@0xF000:0xF07F

# Fill in unimplemented flash locations
hexmate r0-FFFFFFFF,"$projectDir/$imageDir/temp_original_copy.X.production.hex" -O"$projectDir/$imageDir/temp_original_copy.X.production.hex" -FILL=w1:0xFF,0xFF,0xFF,0x00@0xE000:0xB5FFF

# Generate application binary image
hexmate rE000-B5FFFs-E000,"$projectDir/$imageDir/temp_original_copy.X.production.hex" -O"$projectDir/$imageDir/temp_original_copy.X.production.hex"
$OBJ_CPY -I ihex -O binary "$projectDir/$imageDir/temp_original_copy.X.production.hex" "$projectDir/$imageDir/icsp_inhibit.X.production.bin"

rm "$projectDir/$imageDir/temp_original_copy.X.production.hex"

# Sign binary file
java -jar signing_tool.jar -sign "$projectDir/$imageDir/icsp_inhibit.X.production.bin" "../../../boot.X/private_key.pem" "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.der"

export compilerDir="$1"
export projectDir="$2"
export imageDir="$3"
export imageName="$4"
export OBJ_CPY="$compilerDir/xc16-objcopy"
if [ -e "$compilerDir/xc-dsc-objcopy" ]; then
export OBJ_CPY="$compilerDir/xc-dsc-objcopy"
fi
# Export signature value
java -jar signing_tool.jar -export "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.der" "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.bin"

# Covert signature to .hex format
$OBJ_CPY -I binary -O ihex "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.bin" "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex"

# Copy signature .hex into application .hex file application header
hexmate r0-1sF000,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF002-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-EFFF,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r2-3sF002,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF006-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F003,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r4-5sF004,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF00A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F007,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r6-7sF006,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF00E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F00B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r8-9sF008,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF012-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F00F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate rA-BsF00A,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF016-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F013,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate rC-DsF00C,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF01A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F017,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate rE-FsF00E,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF01E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F01B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r10-11sF010,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF022-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F01F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r12-13sF012,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF026-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F023,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r14-15sF014,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF02A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F027,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r16-17sF016,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF02E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F02B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r18-19sF018,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF032-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F02F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r1A-1BsF01A,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF036-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F033,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r1C-1DsF01C,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF03A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F037,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r1E-1FsF01E,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF03E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F03B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r20-21sF020,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF042-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F03F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r22-23sF022,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF046-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F043,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r24-25sF024,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF04A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F047,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r26-27sF026,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF04E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F04B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r28-29sF028,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF052-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F04F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r2A-2BsF02A,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF056-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F053,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r2C-2DsF02C,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF05A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F057,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r2E-2FsF02E,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF05E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F05B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r30-31sF030,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF062-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F05F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r32-33sF032,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF066-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F063,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r34-35sF034,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF06A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F067,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r36-37sF036,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF06E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F06B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r38-39sF038,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF072-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F06F,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r3A-3BsF03A,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF076-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F073,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r3C-3DsF03C,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF07A-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F077,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"
hexmate r3E-3FsF03E,"$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex" rF07E-FFFFFFFF,"$projectDir/$imageDir/$imageName" r0-F07B,"$projectDir/$imageDir/$imageName" -O"$projectDir/$imageDir/$imageName"

rm "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.der"

rm "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.hex"
rm "$projectDir/$imageDir/icsp_inhibit.X.production.bin.signature.bin"