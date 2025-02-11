import { useState } from 'react'
import myAppLogo from '/serial-port-svgrepo-com.svg'  /// import 1 ảnh trong /public/*.svg
import myToolHub from '/toolhub-favicon.ico'          /// import 1 ảnh trong /public/*.ico
import './App.css'
import './style.css'
import './serial_events'

import DeviceSetting from "./component/DeviceSetting/DeviceSetting"
import ListCodeButton from "./component/ListCodeButton/ListCodeButton"
import ScanDevice from "./component/ScanDevice/ScanDevice"
import { SerialProvider } from "./component/SerialContext"

function App() {
  /// Biến đếm 
  const [count, setCount] = useState(0);
  /// Đổi title của trang web trên thanh bar của trình duyệt
  document.title = import.meta.env.VITE_APP_TITLE;
  /// Đổi favicon trong file index.html. Done

  return (
    <>
      <div>
        <div id="titleDiv">
          <span id="title">Web Serial API Demo</span>
        </div>
        <div id="outDiv">
          <textarea id="term_window" readOnly={false}>Hellorld!</textarea>
        </div>
        <div id="inDiv">
          <textarea id="term_input"></textarea>
          <button id="send" disabled={false}>Send</button>
          <button id="clear" disabled={false}>Clear</button>
        </div>
        <div id="setDiv">
          <button id="openclose_port">Open</button>
          <div>
            <span>Baud Rate: </span>
            <select id="baud_rate">
              <option value="9600">9600</option>
              <option value="19200">19200</option>
              <option value="38400">38400</option>
              <option value="57600">57600</option>
              <option value="115200">115200</option>
            </select><button id="change" disabled={false}>Change</button>
          </div>
          <span id="port_info">Disconnected</span>
        </div>
        <script src="./sparkfunx.github.io_files/script.js.download"></script>
      </div>
      <SerialProvider>
        <div className="container">
          <div className="scan-device">
            <ScanDevice />
            <div>
              <a href="https://toolhub.app" target="_blank">
                <img src={myToolHub} className="logo" alt="Toolhub logo" />
              </a>
              <a href="./">
                <img src={myAppLogo} className="logo react" alt="Web Serial Commander logo" />
              </a>
            </div>
            <div className="card">
              <button onClick={() => setCount((count) => count + 1)}>
                count is {count}
              </button>
            </div>
          </div>
          <div className="device-setting"> <DeviceSetting /></div>
          <div className="list-code-button"> <ListCodeButton /></div>
        </div>
      </SerialProvider>
    </>
  )
}

export default App
