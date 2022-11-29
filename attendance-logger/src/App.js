import logo from './logo.svg';
import { useState, useRef } from 'react';
import data from './data.json';

function App() {
  const [attendedIDs, setAttendedIDs] = useState([]);
  const [webSocketIP, setWebSocketIP] = useState("");
  const ws = useRef(0);
  const [rfidData, setRfidData] = useState({...data.rfidData});
  const connectToSocket=()=>{
    ws.current = new WebSocket(`ws://${webSocketIP}/ws`)
    ws.current.onopen = () => {console.log("Connected to "+webSocketIP) }
    ws.current.onclose = () => {console.log("Connection Closed"); ws.current.close()}
    ws.current.onmessage = (e) => { 
      setAttendedIDs(old => [e.data.trim(), ...old])
      setRfidData(old =>{
        return {  [e.data.trim()] : {
            name: "Student Name",
            id : "900xxxXXX",
          },
          ...old
        }
      });
    }
  
  }
  return (
    <div className="h-screen bg-gray-900">
      <div className="flex justify-start items-center flex-col h-full">
        <div className="flex my-20">
          <input type="text" placeholder='ESP32 IP Address' className="text-blue-500 w-200 h-10  text-center" onChange={e=>setWebSocketIP(e.target.value)} />
          <button className='bg-violet-500 border-white text-white h-10 mx-4 rounded-md p-2' onClick={()=>connectToSocket()} >Connect</button>
        </div>
        <div className='flex items-start justify-center'>
          <div className="grid-cols-1">
              {
              Object.keys(rfidData).map(key => (
                <div key={key} className={`flex gap-3 m-4 rounded-lg text-white ${attendedIDs.includes(key)? "bg-green-400" : "bg-red-400"} p-3`}>
                  <div className="flex">
                      {rfidData[key].name}
                  </div>
                  <div className="flex">
                      {rfidData[key].id}
                  </div>
                </div>
              ))
              }
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
