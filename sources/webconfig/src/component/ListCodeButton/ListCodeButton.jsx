import React from "react";
import "./ListCodeButton.css";
import { useActionState } from "react";

function ListCodeButton() {
  const data = [
    {label:"Ctrl", action: "{CTRL}" },
    {label:"Shift", action: "{SHIFT}" },
    {label:"Alt", action: "{ALT}" },
    {label:"Ctrl (release)", action: "{~CTRL}" },
    {label:"Shift (release)", action: "{~SHIFT}" },
    {label:"Alt (release)", action: "{~ALT}" },    
    {label:"Windows", action: "{WINDOWS}" },
    {label:"Up Arrow", action: "{UP}" },
    {label:"Down Arrow",action: "{DOWN}"},
    {label:"Left Arrow",action:"{LEFT}"},
    {label:"Right Arrow",action:"{RIGHT}"},
    {label:"Back Space",action:"{BACKSPACE}"},
    {label:"Tab",action:"{TAB}"},
    {label:"Return",action:"{RETURN}"},
    {label:"Esc",action:"{ESC}"},
    {label:"Insert",action:"{INSERT}"},
    {label:"Print screen",action:"{PRTSC}"},
    {label:"Delete",action:"{DELETE}"},
    {label:"Page Up",action:"{PGUP}"},
    {label:"Page Down",action:"{PGDN}"},
    {label:"Home",action:"{HOME}"},
    {label:"End",action:"{END}"},
    {label:"Caps Lock",action:"{CPLK}"},
    {label:"F1",action:"{F1}"},
    {label:"F2",action:"{F2}"},
    {label:"F3",action:"{F3}"},
    {label:"F4",action:"{F4}"},
    {label:"F5",action:"{F5}"},
    {label:"F6",action:"{F6}"},
    {label:"F7",action:"{F7}"},
    {label:"F8",action:"{F8}"},
    {label:"F9",action:"{F9}"},
    {label:"F10",action:"{F10}"},
    {label:"F11",action:"{F11}"},
    {label:"F12",action:"{F12}"},
    {label:"Enter",action:"{ENTER}"}
  ];

  return (
    <div className="table-container">
      <h2 className="table-title">Bảng mã</h2>
      <table className="styled-table">
        <thead>
          <tr>
            <th>Nút bấm</th>
            <th>Code</th>
          </tr>
        </thead>
        <tbody>
          {data.map((row, index) => (
            <tr key={index} className={index % 2 === 0 ? "even" : "odd"}>
              <td>{row.label}</td>
              <td>{row.action}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

export default ListCodeButton;
