import React, { useEffect, useState } from "react";

function RegisterTable() {
  const [registers, setRegisters] = useState({});
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    fetch("http://localhost:3001/registers")
      .then((response) => {
        if (!response.ok) throw new Error("Network response was not ok");
        return response.json();
      })
      .then((data) => {
        setRegisters(data);
        setLoading(false);
      })
      .catch((error) => {
        console.error("Error fetching register data:", error);
        setError("Failed to load register data");
        setLoading(false);
      });
  }, []);

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
        }}
      >
        <table
          style={{
            width: "100%",
            borderCollapse: "collapse",
            tableLayout: "fixed",
          }}
        >
          <thead
            style={{
              padding: "10px",
              background: "#ddd",
              position: "sticky",
              top: 0,
              zIndex: 2,
              textAlign: "left",
            }}
          >
            <tr>
              <th
                style={{
                  padding: "10px",
                  borderBottom: "1px solid #000",
                  width: "30%",
                  textAlign: "center",
                  fontWeight: "bold",
                  background: "#f8f9fa",
                }}
              >
                Register
              </th>
              <th
                style={{
                  padding: "10px",
                  borderBottom: "1px solid #000",
                  width: "70%",
                  textAlign: "center",
                  fontWeight: "bold",
                  background: "#f8f9fa",
                }}
              >
                Value
              </th>
            </tr>
          </thead>
          <tbody>
            {Object.entries(registers).map(([key, value]) => (
              <tr key={key}>
                <td style={{ padding: "8px", border: "1px solid #ccc" }}>
                  {key}
                </td>
                <td style={{ padding: "8px", border: "1px solid #ccc" }}>
                  {value}
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}

export default RegisterTable;
