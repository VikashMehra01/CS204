import { useState } from "react";
import "./App.css";
import RegisterTable from "./registerTable.js";
import MemoryTable from "./memoryTable.js";

export default function App() {
  const [assemblyCode, setAssemblyCode] = useState("");
  const [machineCode, setMachineCode] = useState("");
  const [activeTab, setActiveTab] = useState("input");

  const handleConvert = async () => {
    try {
      const response = await fetch("http://localhost:3001/assemble", {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: assemblyCode, // Send Assembly code to backend
      });

      const machineCode = await response.text();
      setMachineCode(machineCode); // Update the output textarea
      setActiveTab("output"); // Switch to output tab after conversion
    } catch (error) {
      console.error("Error Vikash:", error);
    }
  };

  return (
    <div className="App">
      <div className="header">
        <p>RISC-V Assembler</p>
      </div>

      <div className="tabs">
        <button
          className={activeTab === "input" ? "active" : ""}
          onClick={() => setActiveTab("input")}
        >
          Input
        </button>
        <button
          className={activeTab === "output" ? "active" : ""}
          onClick={() => setActiveTab("output")}
        >
          Output
        </button>
      </div>

      <div className="tab-content">
        {activeTab === "input" && (
          <div className="code ">
            <h1>Assembly Code</h1>
            <textarea
              value={assemblyCode}
              onChange={(e) => setAssemblyCode(e.target.value)}
            ></textarea>
            <button onClick={handleConvert}>Convert to Machine Code</button>
          </div>
        )}

        {activeTab === "output" && (
          <div className="output">
            <div className="machine-code">
              <h1>Machine Code</h1>
              <textarea value={machineCode} readOnly></textarea>
            </div>
            <div className="Register">
              <h1>Register</h1>
              {/* <textarea value={assemblyCode} readOnly></textarea> */}
              <RegisterTable />
            </div>
            <div className="Memory">
              <h1>Memory</h1>
              {/* <textarea value={assemblyCode} readOnly></textarea> */}
              <MemoryTable />
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
