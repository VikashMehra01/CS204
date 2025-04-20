#include "httplib.h"         // Lightweight HTTP library
#include <fstream>           // File handling
#include <iostream>          // Console logging
#include <cstdlib>           // For system() to run assembler
#include <string>            // For string operations
#include <unordered_map>     // For storing key-value pairs
#include <nlohmann/json.hpp> // JSON library (Download from https://github.com/nlohmann/json)

using json = nlohmann::json;
using namespace httplib;
using namespace std;

Server svr;

// File where boolean knobs are stored
static const string BOOL_FILE = "variables.txt";
// Ordered keys for boolean variables
static const vector<string> BOOL_KEYS = {
    "Pipeline",
    "dataForwarding",
    "PrintRegisterCycle",
    "PipelineRegisetCycle",
    "PrintBPU",
};

// Utility: Read key-value pairs from text file (space-separated)
json readFile(const string &filename)
{
    unordered_map<string, string> keyValuePairs;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open " << filename << endl;
        return json{{"error", "File not found"}};
    }
    string key, value;
    while (file >> key >> value)
    {
        keyValuePairs[key] = value;
    }
    return keyValuePairs;
}

// Utility: Read boolean values from variables.txt into JSON
json loadBooleans()
{
    ifstream in(BOOL_FILE);
    json j;
    vector<bool> vals(BOOL_KEYS.size(), false); // Default to false for each boolean

    if (in.is_open())
    {
        string line;
        size_t idx = 0;
        while (getline(in, line) && idx < BOOL_KEYS.size())
        {
            vals[idx] = (line == "1"); // Store boolean value as true for "1", false otherwise
            ++idx;
        }
        in.close();
    }
    // Fill in JSON object with key-value pairs
    for (size_t i = 0; i < BOOL_KEYS.size(); ++i)
    {
        j[BOOL_KEYS[i]] = vals[i];
    }
    return j;
}

// Utility: Save boolean JSON to variables.txt
void saveBooleans(const json &j)
{
    ofstream out(BOOL_FILE);
    if (!out.is_open())
    {
        cerr << "Error: Cannot open " << BOOL_FILE << " for writing" << endl;
        return;
    }

    for (const auto &key : BOOL_KEYS)
    {
        bool val = false;
        if (j.contains(key) && j[key].is_boolean())
        {
            val = j[key].get<bool>();
        }
        out << (val ? "1" : "0") << "\n"; // Write "1" for true, "0" for false
    }

    out.close();
}

// Existing handlers...
json readKVFile(const string &filename)
{
    return readFile(filename);
}

void handleAssemblyRequest(const Request &req, Response &res)
{
    string assemblyCode = req.body;
    ofstream outFile("input.asm");
    outFile << assemblyCode;
    outFile.close();
    system("./main input.asm output.mc FinalOutput.txt memory.mem register.reg PC.pc");
    ifstream inFile("FinalOutput.txt");
    if (!inFile)
    {
        cerr << "Error: Could not open FinalOutput.txt!" << endl;
        res.set_content("Error: Could not read output file", "text/plain");
        return;
    }
    string machineCode((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_content(machineCode, "text/plain");
}

void handleRegisterRequest(const Request &req, Response &res)
{
    json registerData = readKVFile("register.reg");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_content(registerData.dump(), "application/json");
}

void handleRegisterUpdate(const Request &req, Response &res)
{
    json updated;
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    try
    {
        updated = json::parse(req.body);
    }
    catch (...)
    {
        res.status = 400;
        res.set_content("Invalid JSON format", "text/plain");
        return;
    }
    ofstream outFile("register.reg");
    if (!outFile.is_open())
    {
        res.set_content("Error: Could not open register.reg for writing", "text/plain");
        return;
    }
    for (auto &el : updated.items())
    {
        outFile << el.key() << " " << el.value().get<string>() << "\n";
    }
    outFile.close();
    res.set_content("Registers updated successfully", "text/plain");
}

void handleMemoryRequest(const Request &req, Response &res)
{
    json memoryData = readKVFile("memory.mem");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_content(memoryData.dump(), "application/json");
}

void handleMemoryUpdate(const Request &req, Response &res)
{
    json updated;
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    try
    {
        updated = json::parse(req.body);
    }
    catch (...)
    {
        res.status = 400;
        res.set_content("Invalid JSON format", "text/plain");
        return;
    }
    ofstream outFile("memory.mem");
    if (!outFile.is_open())
    {
        res.set_content("Error: Could not open memory.mem for writing", "text/plain");
        return;
    }
    for (auto &el : updated.items())
    {
        outFile << el.key() << " " << el.value().get<string>() << "\n";
    }
    outFile.close();
    res.set_content("Memory updated successfully", "text/plain");
}

// New handlers for boolean knobs
void handleGetBooleans(const Request &req, Response &res)
{
    json j = loadBooleans();
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_content(j.dump(), "application/json");
}

void handleUpdateBooleans(const Request &req, Response &res)
{
    json j;
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    try
    {
        j = json::parse(req.body); // Parse the incoming JSON body
    }
    catch (...)
    {
        res.status = 400;
        res.set_content("Invalid JSON format", "text/plain");
        return;
    }

    saveBooleans(j); // Save the updated booleans to file
    res.set_content("Booleans updated successfully", "text/plain");
}

int main()
{
    cout << "Server starting on http://localhost:3001" << endl;

    // Global CORS handling for OPTIONS requests
    svr.Options(R"(.*)", [](const Request &req, Response &res)
                {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200; });

    // Existing endpoints
    svr.Post("/assemble", handleAssemblyRequest);
    svr.Get("/registers", handleRegisterRequest);
    svr.Post("/update-registers", handleRegisterUpdate);
    svr.Get("/memory", handleMemoryRequest);
    svr.Post("/update-memory", handleMemoryUpdate);
    svr.Get("/get-booleans", handleGetBooleans);
    svr.Post("/update-booleans", handleUpdateBooleans);

    svr.listen("0.0.0.0", 3001);
    return 0;
}
