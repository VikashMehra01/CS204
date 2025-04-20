import { useState, useEffect } from "react";
import "./App.css";
import RegisterTable from "./registerTable.js";
import MemoryTable from "./memoryTable.js";

export default function App() {
  const [assemblyCode, setAssemblyCode] = useState("");
  const [machineCode, setMachineCode] = useState("");
  const [activeTab, setActiveTab] = useState("input");
  const [PC, setPC] = useState(0);
  const [booleans, setBooleans] = useState({
    Pipeline: false,
    dataForwarding: false,
    PrintRegisterCycle: false,
    PipelineRegisetCycle: false,
    PrintBPU: false,
    SpecificInstruction: false,
    cycles: false,
  });

  const [theme, setTheme] = useState("light");

  const toggleTheme = () => {
    setTheme((prevTheme) => (prevTheme === "light" ? "dark" : "light"));
  };
  console.log("Powered by Vikash Mehra");
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

  useEffect(() => {
    const fetchBooleans = async () => {
      try {
        const response = await fetch("http://localhost:3001/get-booleans");
        const data = await response.json();
        setBooleans(data);
      } catch (error) {
        console.error("Error loading booleans variables:", error);
      }
    };
    fetchBooleans();
  }, []);

  const handleToggle = (key) => {
    const updatedBooleans = { ...booleans, [key]: !booleans[key] };
    setBooleans(updatedBooleans);
    console.log("SpecificInstruction:", booleans.SpecificInstruction);
  };
  const handleSaveBooleans = async () => {
    try {
      await fetch("http://localhost:3001/update-booleans", {
        // Corrected the endpoint
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          ...booleans,
          PC: booleans.SpecificInstruction ? PC : 0,
        }),
      });

      alert("Saved!");
    } catch (error) {
      console.error("Error saving booleans:", error);
    }
  };

  return (
    <div className={`App ${theme}`}>
      <div className="header">
        {/* <button className="theme-toggle" onClick={toggleTheme}>
          {theme === "light" ? "🌙" : "☀️"}
        </button> */}
        <p>RISC-V 32</p>
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
          <div className="Input">
            <div className="code ">
              <h1>Assembly Code</h1>
              <textarea
                value={assemblyCode}
                onChange={(e) => setAssemblyCode(e.target.value)}
              ></textarea>
              <button onClick={handleConvert}>Simulate</button>
            </div>
            <div className="Knobs">
              <h1>Settings</h1>
              <div className="Knobs-cover">
                <div>
                  {Object.entries(booleans)
                    .filter(([key, val]) => typeof val === "boolean")
                    .map(([key, value]) => (
                      <div className="knob" key={key}>
                        <label htmlFor={key}>{key}</label>
                        <input
                          type="checkbox"
                          id={key}
                          checked={value}
                          onChange={() => handleToggle(key)}
                        />
                      </div>
                    ))}
                </div>

                {booleans.SpecificInstruction && (
                  <div className="knob">
                    <label htmlFor="PC">PC:</label>
                    <input
                      type="number"
                      id="PC"
                      value={PC}
                      onChange={(e) => setPC(parseInt(e.target.value))}
                    />
                  </div>
                )}
              </div>
              <button onClick={handleSaveBooleans}>Save</button>
            </div>
          </div>
        )}

        {activeTab === "output" && (
          <div className="output">
            <div className="machine-code">
              <h1>Machine Code</h1>
              <textarea value={machineCode} readOnly>
                style=
                {{
                  border: "1px solid #ccc",
                  backgroundColor: "#f0f0f0",
                  resize: "none",
                  outline: "none",
                  color: "#333",
                  fontFamily: "monospace",
                }}
              </textarea>
            </div>
            <div className="Register">
              <h1>Register</h1>
              <RegisterTable />
            </div>
            <div className="Memory">
              <h1>Memory</h1>
              <MemoryTable />
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
