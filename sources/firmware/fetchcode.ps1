#Nạp firmware lên thiết bị.
#Tham số dòng lệnh duy nhất là cổng COm. Ví dụ fetchcode.ps1 COM8

#exit 0 
$BUILD_PATH=".\.pio\build\esp32-wroom-32"

python -m esptool --chip esp32 --port $args[0] write_flash -z `
    0x1000 $BUILD_PATH\bootloader.bin `
    0x8000 $BUILD_PATH\partitions.bin `
    0x10000 $BUILD_PATH\firmware.bin
