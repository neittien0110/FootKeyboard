import { SerialContext } from "../SerialContext";
import "./DeviceSetting.css";
import React, { useContext, useState } from "react";



function DeviceSetting() {
  const { connectToSerial} = useContext(SerialContext);
  const [deviceName, setDeviceName] = useState("");
  const [delayTime, setDelayTime] = useState("");
  const [button1, setButton1] = useState("");
  const [button2, setButton2] = useState("");
  const [button3, setButton3] = useState("");
  const [button4, setButton4] = useState("");
  const handleUpdate = async ()=>{
    await connectToSerial(deviceName,delayTime,button1,button2,button3,button4);
  }
  return (
    <div>
      <h2 className="title">Cấu hình Bluetooth</h2>

      {/* Thông tin thiết bị */}
      <div className="device-info">
        <div className="info-item">
          <label>Tên thiết bị:</label>
          <input type="text"placeholder="Nhập tên thiết bị" value={deviceName}
          onChange={(e) => setDeviceName(e.target.value)}/>
        </div>
        <div className="info-item">
          <label>Độ trễ giữa các phím:</label>
          <input type="number"placeholder="Nhập số" value={delayTime}
          onChange={(e) => setDelayTime(e.target.value)} />
        </div>
      </div>

      {/* Cấu hình nút bấm */}
      <div className="button-config">
        <h3>Cấu hình nút bấm</h3>
        <label>Nút bấm 1:</label>
        <div className="info-item">
          <input type="text" placeholder="Nhập mã cấu hình button. Ví dụ: {SHIFT}H{~SHIFT}ello"value={button1}
          onChange={(e) => setButton1(e.target.value)} />
        </div>
        <label>Nút bấm 2:</label>
        <div className="info-item">
          <input type="text" placeholder="Nhập mã cấu hình button. Ví dụ: {SHIFT}H{~SHIFT}ello"value={button2}
          onChange={(e) => setButton2(e.target.value)} />
        </div>
        <label>Nút bấm 3:</label>
        <div className="info-item">
          <input type="text" placeholder="Nhập mã cấu hình button. Ví dụ: {SHIFT}H{~SHIFT}ello"value={button3}
          onChange={(e) => setButton3(e.target.value)} />
        </div>
        <label>Nút bấm 4:</label>
        <div className="info-item">
          <input type="text" placeholder="Nhập mã cấu hình button. Ví dụ: {SHIFT}H{~SHIFT}ello" value={button4}
          onChange={(e) => setButton4(e.target.value)}/>
        </div>
      </div>
      <div className="actions">
        <button className="check-btn" onClick={handleUpdate}>Cập nhật thiết lập cho nút bấm</button>
      </div>
      {/* Tính năng nâng cao */}
      {/* <div className="advanced">
        <h3>Receive message:</h3>
        <div>
          <textarea value={receivedData} readOnly placeholder="Nhận tin nhắn..." />
        </div>
      </div> */}

      {/* Các nút hành động */}
      
    </div>
  );
}

export default DeviceSetting;
