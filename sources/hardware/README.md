# FOOT KEYBOARD - HARDWARE

## Mục lục

- [Thiết kế 1: Breadboard và ESP32-C3 Supermini](#thiết-kế-1-breadboard-và-esp32-c3-supermini)
- [Thiết kế 2: PCB riêng và ESP32-WROOM-32](#thiết-kế-2-pcb-riêng-và-esp32-wroom-32)

Xem thêm

- [Firmware](../firmware/README.md)
- [WebConfig](../webconfig/README.md)

## Thiết kế 1: Breadboard và ESP32-C3 Supermini

   ![3D](https://github.com/user-attachments/assets/72c055d4-a4e6-4ac9-821f-4be8d76b2dc9)

### BOM

1. [esp32-c3 super mini](https://neittien0110.github.io/MCU/ESP32/ESP32-C3_SuperMini.html)
2. [Mạch sạc 1 cell Lithium](https://neittien0110.github.io/linhkiendientu/EasyEDA_Library.html#m%E1%BA%A1ch-s%E1%BA%A1c)
3. [Pedal](https://shopee.vn/C%C3%B4ng-T%E1%BA%AFc-B%C3%A0n-%C4%90%E1%BA%A1p-Ch%C3%A2n-C%C3%B4ng-T%E1%BA%AFc-M%C3%A1y-May-250VAC-10A-i.358472761.12374007715?xptdk=ac8c7bcf-f454-4ec1-91f1-773025543759)
4. Dây nguồn DC đực, để nối vào pedal
5. Đầu jack DC-005

### Schematic

![Schematic](https://github.com/user-attachments/assets/334572a6-c68d-4de8-aa1c-cc1e22acd1bf)\
![PCB](https://github.com/user-attachments/assets/870edede-d334-481e-a6ae-598f84755eab)\
[File thiết kế Schematic và PCB](https://u.easyeda.com/account/project?project=7fc17ecfdb504504bb8d578da7a98fa2)

## Thiết kế 2: PCB riêng và ESP32-WROOM-32

  ![image](https://github.com/user-attachments/assets/02218e85-df2a-460f-8be7-4fe8c98f1e93)

- Version 1.0
- Version 2.0:
  - Fix-bug: chưa đấu chéo hai chân TXD RXD của CH340 với ESP32
  - Fix-bug: Thiếu nút RST và BOOT
- Version 3.0
  - Upgraded: thay module sạc 5V mini bằng module TP4056H  mini tự vẽ lại để vị trí các lỗ pin phù hợp
- Version 3.4:
  - Upgraded: bổ sung ổn áp 3v3 với AMS1117.
  - Hazard: Nếu không có AMS117 thì WROOM-32, hoặc CH340 ko chạy được do quá áp (>3.6v). Nhưng nếu sử dụng AMS1117 thì khi chạy ESP32 bật tắt led là okay, nhưng chỉ cần bật Bluetooth BLE là ESP báo Brownout detector was triggered và ngay lập tức khởi động lại ESP32.
- Version 3.5: 
  - công tắc gạt có 2 mod độc lập   nguồn trực tiếp+sạc/nguồn pin
- Version 3.6: 
  - bổ sung các khe cắm JP0,JP1,JP2,JP3 để kết nối với các jack nối pedal rời gắn trên hộp, hoặc không sử dụng DC5521
- Version 3.7:
  - Loại đế pin 14250 có khoảng cách giữa 2pin là 29mm
- Version 3.8:
  - Thay đổi thiết kế nguồn cấp: mạch sạc --> ams1117 --> (pin1 dipswitch pin3) <--- lithium
- Version 3.9:
  - Boot là nút to để trở thành phím chức năng




### Schematic

![Schematic](https://github.com/user-attachments/assets/f822c5d4-bf2b-4313-a17f-d63dcbe7d28e)\
![PCB](https://github.com/user-attachments/assets/6755bcad-b7d5-416b-95b8-4818bbe2ea2f)






