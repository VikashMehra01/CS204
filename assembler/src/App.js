import { useState } from "react";
import "./App.css";

export default function App() {
  const [assemblyCode, setAssemblyCode] = useState("");
  const [machineCode, setMachineCode] = useState("");

  const assembleCode = async () => {
    const response = await fetch("http://localhost:5000/assemble", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ code: assemblyCode }),
    });

    const data = await response.json();
    setMachineCode(data.machineCode || "Error assembling code");
  };

  return (
    <div className="App">
      <h1>Assembler</h1>
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
