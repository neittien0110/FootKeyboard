import { SerialContext } from '../SerialContext';
import React, { useContext } from "react";
import './ScanDevice.css'

const ScanDevice = () => {
  const { connectToSerial, receivedData, output } = useContext(SerialContext);
  const data = [
    
  ];
  return (
    <div className="ScanDevice">
      <table className="ScanDevice-table">
        <thead>
          <tr>
            <th>Foot Keyboard</th>
          </tr>
        </thead>
        <tbody>
          {data.map((item, index) => (
            <tr key={index}>
              <td>{item}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <div>
      <div>
        <h2>Danh sách lệnh:</h2>
        TK=mã cấu hình tương ứng với khoảng thời gian trễ giữa các phím kí tự gửi đi
        A?=mã xem toàn bộ thông tin cáu hình
        00=mã cấu hình tương ứng với chân PIN_PEDAL00
        01=mã cấu hình tương ứng với chân PIN_PEDAL01
        02=mã cấu hình tương ứng với chân PIN_PEDAL02
        03=mã cấu hình tương ứng với chân PIN_PEDAL03        
      </div>
      <div>
        <h2>Khôi phục cấu hình xuất xưởng</h2>
          <ol>
          <li>Tắt điện thiết bị **Foot Keyboard**.</li>
          <li>Bấm phím **pedal 02** và giữ nguyên trạng thái bấm. Lưu ý KHÔNG bấm nút BOOT.</li>
          <li>Bật công tắc để khởi động.</li>
          <li>Bấm nút BOOT. Giữ cho tới khi đèn led xanh nháy 3 lần và tắt, lặp đi đi lặp</li>
          </ol>
            Đã xong.
      </div>
      <div>
      <h2>Chế độ Self Test</h2>
        <ol>
        <li>Trên máy tính, mở phần mềm soạn thảo bất kì. Đặt con trò bàn phím vào ô soạn thảo.</li>
        <li>Tắt điện thiết bị **Foot Keyboard**.</li>
        <li>Bấm phím **pedal 01** và giữ nguyên trạng thái bấm. Lưu ý KHÔNG bấm nút BOOT.</li>
        <li>Bật công tắc để khởi động.</li>
        <li>Bấm nút BOOT. Giữ cho tới khi hết đèn led xanh nháy (báo hiệu đang thực hiện kết nối bluetooth)</li>
        <li>Quan sát nội dung hiển thị trên phần mềm soạn thảo, hiển thị giống với video dưới đây.</li>
        </ol>
      </div>
      </div>
    </div>
  );
}

export default ScanDevice