import sys
import os
import argparse
import time
import serial         # pip install pyserial

#------------------------------------------------------------------------------------------------------------
args = None

def commandline_parser(): 
    """Bộ phân tích tham số dòng lệnh
    Returns:
        (fontpath,character): Đường dẫn file font cần xem, kí tự cần xem
    """    
    # Tạo bộ phân tích tham số dòng lệnh
    parser = argparse.ArgumentParser(
        description="Chương trình gửi các lệnh ra serial",
        epilog='Official website: https://toolhub.app/serial-commands',
    )
    
    # Tham số bắt buộc: -i <file path>
    parser.add_argument("-c", "--com", required=True, help="Tên cổng com. Ví dụ /dev/ttyUSB0, COM3, ttyUSB0, ttyACM0")

    # Tham số bắt buộc: -o <file path>
    parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Tốc độ truyền. Mặc định 115200. Ví dụ 115200, 9600.")

    # Tham số tùy chọn: -c <kí tự, hoặc mã unicode ở hệ 16, hoặc mã unicode ở hệ 10)
    parser.add_argument("-f", "--file", required=True, type=str, help="Tên file chứa lệnh serial. Mỗi dòng 1 lệnh. Ví dụ: command.txt")
    
    # Tham số tùy chọn: -v (mặc định là False)
    parser.add_argument("-v", "--verbose", action='store_true', help="Hiển thị chi tiết thông tin thực hiện")

    # Phân tích tham số
    global args
    args = parser.parse_args()    

    # Kiểm tra tham số hợp lệ
    if not os.path.isfile(args.file):
        print(f"Error: file chứa các lệnh {args.file} để gửi qua serial không tồn tại")
        sys.exit(1)
    return        

#------------------------------------------------------------------------------------------------------------
#  main
#------------------------------------------------------------------------------------------------------------
        
def main():    
    global args    
    # Phân tích tham số dòng lệnh
    commandline_parser()    
    
    if args.verbose:
        # Hiển thị các bảng dữ liệu của font
        #     glyf - Chứa thông tin về glyphs
        #     cmap - Ánh xạ Unicode với glyph
        #     name - Chứa tên font và metadata
        #     head - Chứa thông tin tổng quan về font
        print(f"Serial Port: {args.com}, baudrate={args.baudrate}")
        
    # Kết nối cổng serial
    myserial = serial.Serial(args.com, args.baudrate) #
    if not myserial.is_open:
        print(f"Error: Không thể mở cổng {args['com']}")
        sys.exit(1) 

    # Đọc từng dòng của file lệnh
    fcmd = open(args.file, 'r')
    for line in fcmd:
        # Ghi lệnh ra cổng serial
        myserial.write(line.encode('utf-8'))
        myserial.flush
        print(f"-->: {line}")
            
        # Đợi 0.5 giây để thiết bị phản hồi
        time.sleep(0.5)
        
        # Đọc phản hồi từ thiết bị
        buf = myserial.read_all() 
        utf8_str = buf.decode('utf-8', errors='ignore')
        print(f"<--: {utf8_str}")        
    # Đóng file lệnh
    fcmd.close()

    # Đóng cổng serial  
    myserial.close() 
    return 0    
if __name__ == "__main__":
    main()        