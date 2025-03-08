import { useState } from "react";
import "./App.css";

export default function App() {
  const [assemblyCode, setAssemblyCode] = useState("");
  const [machineCode, setMachineCode] = useState("");

  const handleConvert = async () => {
    try {
      const response = await fetch("http://localhost:3001/assemble", {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: assemblyCode, // Send Assembly code to backend
      });

      const machineCode = await response.text();
      setMachineCode(machineCode); // Update the output textarea
    } catch (error) {
      console.error("Error Vikash:", error);
    }
  };

  return (
    <div className="App">
      <div className="header">
        <p>RISC-V Assembler</p>
        <div className="do">
          <button onClick={handleConvert}>Convert to Machine Code</button>
        </div>
      </div>

      {/* <button onClick={assembleCode}>Assemble</button> */}
      <div className="Assembler">
        <div className="code">
          <h1>Assembly Code</h1>
          <textarea
            value={assemblyCode}
            onChange={(e) => setAssemblyCode(e.target.value)}
          ></textarea>
        </div>

        <div className="code">
          <h1>Machine Code</h1>
          <textarea value={machineCode} readOnly></textarea>
        </div>
      </div>
    </div>
  );
}
