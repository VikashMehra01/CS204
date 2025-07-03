# 🚀 RISC-V Assembler & Simulator

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20MacOS-lightgrey)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B)
![Frontend](https://img.shields.io/badge/frontend-React-brightgreen?logo=react)

> A powerful RISC-V assembler and simulator with an interactive GUI for assembling, executing, and visualizing low-level code execution.

---

## 🧠 About the Project

This project is a full-stack **RISC-V Assembler and Simulator** built using **C++** for the backend and **React.js** for the frontend. It allows users to:
- Write RISC-V assembly code.
- Assemble it into 32-bit machine code.
- Simulate the instruction execution.
- View real-time updates of registers and memory — all through a beautiful interface.

Built as an educational and practical tool, it’s ideal for students learning systems programming or RISC-V architecture.

---

## ✨ Features

- 🛠️ **Assembler** — Converts RISC-V instructions to binary machine code.
- 🧪 **Simulator** — Emulates instruction execution step-by-step with accurate memory and register updates.
- 💻 **React GUI** — Clean, user-friendly interface to input, simulate, and debug code.
- 📊 **Real-Time Visualization** — Displays current register values, memory changes, and program counter status.

---


## ⚙️ Requirements

Before starting, make sure your system has:

- ✅ Node.js and npm  
- ✅ A C++17 compatible compiler (`g++`, `clang++`, or MSVC)
- ✅ [Nlohmann JSON](https://github.com/nlohmann/json) library installed (header-only)
- ✅ Linux, Windows, or MacOS (tested on all)

---

## 🧰 Installation & Setup

Clone the repo and follow the steps below:

### 1️⃣ Clone the Repository

```bash
git clone https://github.com/VikashMehra01/CS204.git
cd CS204

# Backend Setup (Assembler & Simulator)

cd backend
# Compile the backend (assumes main.cpp as entry)
g++ -std=c++17 -o server main.cpp
# Start the backend
./server
```

# Frontend Setup (React GUI)

Open a new terminal:
```
cd frontend
npm install     # Install frontend dependencies
npm start       # Launch the development server
```
💡 How to Use

    Open your browser and navigate to http://localhost:3000

    Paste your RISC-V assembly code into the editor.

    Click on Assemble & Simulate.

    Watch your code come to life:
