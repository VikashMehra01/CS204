import React, { useEffect, useState } from "react";

function RegisterTable() {
  const [memory, setmemory] = useState({});
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    fetch("http://localhost:3001/memory")
      .then((response) => {
        if (!response.ok) throw new Error("Network response was not ok");
        return response.json();
      })
      .then((data) => {
        setmemory(data);
        setLoading(false);
      })
      .catch((error) => {
        console.error("Error fetching register data:", error);
        setError("Failed to load register data");
        setLoading(false);
      });
  }, []);

  // Handle input changes
  const handleInputChange = (key, newValue) => {
    setmemory((prev) => ({
      ...prev,
      [key]: newValue,
    }));
  };

  // Send updated data to backend
  const handleSave = () => {
    fetch("http://localhost:3001/update-memory", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(memory),
    })
      .then((response) => {
        if (!response.ok) throw new Error("Failed to update memory");
        return response.text();
      })
      .then(() => alert("memory updated successfully!"))
      .catch((error) => {
        console.error("Error updating register data:", error);
        alert("Failed to update memory");
      });
  };

  if (loading) return <p>Loading...</p>;
  if (error) return <p style={{ color: "red" }}>{error}</p>;

  return (
    <div
      style={{
        textAlign: "center",
        height: "96.5%",
        width: "100%",
        backgroundOrigin: "border-box",
      }}
    >
      <div
        style={{
          width: "90%",
          maxHeight: "100%",
          overflow: "auto",
          margin: "auto",
          border: "1px solid #ccc",
          borderRadius: "5px",
          scrollbarWidth: "none",
        }}
      >
        <table
          style={{
            width: "100%",
            borderCollapse: "collapse",
            tableLayout: "fixed",
          }}
        >
          {/* Sticky Header */}
          <thead
            style={{
              padding: "10px",
              background: "#ddd",
              position: "sticky",
              top: 0,
              zIndex: 2,
              textAlign: "left", // Ensures alignment
              width: "100%",
            }}
          >
            <tr>
              <th
                style={{
                  padding: "10px",
                  borderBottom: "1px solid #000",
                  width: "30%",
                }}
              >
                Register
              </th>
              <th
                style={{
                  padding: "10px",
                  borderBottom: "1px solid #000",
                  width: "70%",
                }}
              >
                Value
              </th>
            </tr>
          </thead>
          <tbody>
            {Object.entries(memory).map(([key, value]) => (
              <tr key={key}>
                <td style={{ padding: "8px", border: "1px solid #ccc" }}>
                  {key}
                </td>
                <td style={{ padding: "8px", border: "1px solid #ccc" }}>
                  <input
                    type="text"
                    value={value}
                    onChange={(e) => handleInputChange(key, e.target.value)}
                    style={{ width: "100%", border: "none" }}
                  />
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
      {/* Save Button */}
      {/* <button
        onClick={handleSave}
        style={{
          marginTop: "10px",
          padding: "8px 12px",
          background: "green",
          color: "white",
          border: "none",
          cursor: "pointer",
        }}
      >
        Save Changes
      </button> */}
    </div>
  );
}

export default RegisterTable;
