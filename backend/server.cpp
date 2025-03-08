#include "httplib.h" // Lightweight HTTP library
#include <fstream>   // File handling
#include <iostream>  // Console logging
#include <cstdlib>   // For system() to run assembler
#include <string>    // For string operations
using namespace httplib;
using namespace std;

Server svr;

void handleAssemblyRequest(const Request &req, Response &res)
{
    string assemblyCode = req.body; // Get Assembly code from frontend

    // Save received assembly code to a file
    ofstream outFile("input.asm");
    outFile << assemblyCode;
    outFile.close();

    // Run the assembler (which generates 'output.mc' machine code file)
    system("./Assembler input.asm output.mc");

    // Read the machine code output file
    ifstream inFile("output.mc");
    if (!inFile) {
        cerr << "Error: Could not open output.mc file!" << endl;
        res.set_content("Error: Could not read output file", "text/plain");
        return;
    }
    
    string machineCode((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    // Send the machine code back to the frontend
    res.set_content(machineCode, "text/plain");
}

int main()
{
    cout << "Server running on http://localhost:3001" << endl;

    svr.Post("/assemble", handleAssemblyRequest); // Handle POST request
    svr.listen("0.0.0.0", 3001);                  // Start server on port 3001
}
