#include "CampusCompass.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <limits>

// -----------------------------
// Constructor
// -----------------------------
CampusCompass::CampusCompass() {}

// -----------------------------
// Validation Helpers
// -----------------------------
bool CampusCompass::isValidUFID(const std::string& ufid) const {
    if (ufid.size() != 8) return false;
    for (char c : ufid) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

bool CampusCompass::isValidName(const std::string& name) const {
    if (name.empty()) return false;
    for (char c : name) {
        if (!std::isalpha(static_cast<unsigned char>(c)) && c != ' ')
            return false;
    }
    return true;
}

bool CampusCompass::isValidClassCode(const std::string& code) const {
    if (code.size() != 7) return false;

    // First 3 must be capital letters
    for (int i = 0; i < 3; i++) {
        if (!std::isupper(static_cast<unsigned char>(code[i]))) return false;
    }
    // Last 4 must be digits
    for (int i = 3; i < 7; i++) {
        if (!std::isdigit(static_cast<unsigned char>(code[i]))) return false;
    }
    return true;
}

// -----------------------------
// "HH:MM" → minutes
// -----------------------------
int CampusCompass::parseTimeToMinutes(const std::string& t) const {
    if (t.size() != 5 || t[2] != ':') return 0;
    int h = std::stoi(t.substr(0, 2));
    int m = std::stoi(t.substr(3, 2));
    return h * 60 + m;
}

// -----------------------------
// CSV Loader: edges.csv
// -----------------------------
bool CampusCompass::loadEdges(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line)) return false;  // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id1Str, id2Str, name1, name2, timeStr;

        if (!std::getline(ss, id1Str, ',')) return false;
        if (!std::getline(ss, id2Str, ',')) return false;
        if (!std::getline(ss, name1, ',')) return false;
        if (!std::getline(ss, name2, ',')) return false;
        if (!std::getline(ss, timeStr, ',')) return false;

        int id1 = std::stoi(id1Str);
        int id2 = std::stoi(id2Str);
        int time = std::stoi(timeStr);

        graph_.addEdge(id1, id2, time);
    }
    return true;
}

// -----------------------------
// CSV Loader: classes.csv
// -----------------------------
bool CampusCompass::loadClasses(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line)) return false;  // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string code, locStr, startStr, endStr;

        if (!std::getline(ss, code, ',')) return false;
        if (!std::getline(ss, locStr, ',')) return false;
        if (!std::getline(ss, startStr, ',')) return false;
        if (!std::getline(ss, endStr, ',')) return false;

        ClassInfo info;
        info.code = code;
        info.locationId = std::stoi(locStr);
        info.startMinutes = parseTimeToMinutes(startStr);
        info.endMinutes = parseTimeToMinutes(endStr);

        catalog_.addClass(info);
    }
    return true;
}

// -----------------------------
// Public CSV Loader (called by main)
// -----------------------------
bool CampusCompass::ParseCSV(const std::string& edges_filepath,
                             const std::string& classes_filepath) {
    bool ok1 = loadEdges(edges_filepath);
    bool ok2 = loadClasses(classes_filepath);
    return ok1 && ok2;
}

// -----------------------------
// handleInsert: parses full line with quoted name
// -----------------------------
bool CampusCompass::handleInsert(const std::string& fullLine) {
    // Find first and second quote
    std::size_t firstQuote = fullLine.find('"');
    std::size_t secondQuote = fullLine.find('"', firstQuote + 1);
    if (firstQuote == std::string::npos || secondQuote == std::string::npos) {
        return false;
    }

    std::string name = fullLine.substr(firstQuote + 1, secondQuote - firstQuote - 1);

    // Remaining part after the name
    std::string rest = fullLine.substr(secondQuote + 1);
    std::stringstream ss(rest);

    std::string ufid;
    int residenceId;
    int numClasses;
    if (!(ss >> ufid >> residenceId >> numClasses)) {
        return false;
    }

    if (numClasses < 1 || numClasses > 6) {
        return false;
    }

    std::vector<std::string> codes;
    std::string code;
    while (ss >> code) {
        codes.push_back(code);
    }

    if (static_cast<int>(codes.size()) != numClasses) {
        return false;
    }

    // Validation
    if (!isValidName(name) || !isValidUFID(ufid)) {
        return false;
    }
    if (students_.hasStudent(ufid)) {
        return false;
    }

    for (const auto& c : codes) {
        if (!isValidClassCode(c)) {
            return false;
        }
        if (!catalog_.hasClass(c)) {
            return false;
        }
    }

    Student s;
    s.name = name;
    s.ufid = ufid;
    s.residenceLocationId = residenceId;
    s.classes = codes;

    return students_.addStudent(s);
}

// -----------------------------
// printShortestEdges helper
// -----------------------------
void CampusCompass::handlePrintShortestEdges(const std::string& ufid) const {
    const Student* s = students_.getStudent(ufid);
    if (!s) {
        std::cout << "unsuccessful\n";
        return;
    }

    // Run Dijkstra from residence
    std::unordered_map<int, int> dist;
    std::unordered_map<int, int> parent;
    graph_.dijkstra(s->residenceLocationId, dist, parent);

    const int INF = std::numeric_limits<int>::max();

    // Sort class codes
    std::vector<std::string> codes = s->classes;
    std::sort(codes.begin(), codes.end());

    std::cout << "Name: " << s->name << "\n";

    for (const auto& code : codes) {
        const ClassInfo* ci = catalog_.getClass(code);
        if (!ci) {
            // If the class doesn't exist in catalog treat as unreachable
            std::cout << code << " | Total Time: -1\n";
            continue;
        }
        int loc = ci->locationId;
        int totalTime = -1;

        auto it = dist.find(loc);
        if (it != dist.end() && it->second != INF) {
            totalTime = it->second;
        }

        std::cout << code << " | Total Time: " << totalTime << "\n";
    }
}

// -----------------------------
// printStudentZone helper
// -----------------------------
void CampusCompass::handlePrintStudentZone(const std::string& ufid) const {
    const Student* s = students_.getStudent(ufid);
    if (!s) {
        std::cout << "unsuccessful\n";
        return;
    }

    // Dijkstra from residence
    std::unordered_map<int, int> dist;
    std::unordered_map<int, int> parent;
    graph_.dijkstra(s->residenceLocationId, dist, parent);

    const int INF = std::numeric_limits<int>::max();

    // Collect vertices used in shortest paths to all reachable classes
    std::unordered_set<int> vertexSet;
    vertexSet.insert(s->residenceLocationId);

    for (const auto& code : s->classes) {
        const ClassInfo* ci = catalog_.getClass(code);
        if (!ci) continue;

        int loc = ci->locationId;

        auto it = dist.find(loc);
        if (it == dist.end() || it->second == INF) {
            // class unreachable: skip
            continue;
        }

        // Reconstruct path from class location back to residence
        int curr = loc;
        while (curr != -1) {
            if (!vertexSet.insert(curr).second) {
                curr = parent[curr];
                continue;
            }
            auto pit = parent.find(curr);
            if (pit == parent.end()) break;
            curr = pit->second;
        }
    }

    // Build vector from vertexSet
    std::vector<int> vertices(vertexSet.begin(), vertexSet.end());

    // MST on induced subgraph
    int cost = graph_.mstCost(vertices);

    std::cout << "Student Zone Cost For " << s->name << ": " << cost << "\n";
}

// -----------------------------
// verifySchedule helper
// -----------------------------
void CampusCompass::handleVerifySchedule(const std::string& ufid) const {
    const Student* s = students_.getStudent(ufid);
    if (!s) {
        std::cout << "unsuccessful\n";
        return;
    }

    if (s->classes.size() <= 1) {
        std::cout << "unsuccessful\n";
        return;
    }

    struct ScheduledClass {
        std::string code;
        int start;
        int end;
        int locationId;
    };

    std::vector<ScheduledClass> sc;
    for (const auto& code : s->classes) {
        const ClassInfo* ci = catalog_.getClass(code);
        if (!ci) continue; // skip if missing
        sc.push_back({code, ci->startMinutes, ci->endMinutes, ci->locationId});
    }

    if (sc.size() <= 1) {
        std::cout << "unsuccessful\n";
        return;
    }

    std::sort(sc.begin(), sc.end(), [](const ScheduledClass& a, const ScheduledClass& b) {
        return a.start < b.start;
    });

    std::cout << "Schedule Check for " << s->name << ":\n";

    const int INF = std::numeric_limits<int>::max();

    for (std::size_t i = 0; i + 1 < sc.size(); ++i) {
        const auto& c1 = sc[i];
        const auto& c2 = sc[i + 1];

        int gap = c2.start - c1.end;

        // Shortest path from c1.locationId to c2.locationId
        std::unordered_map<int, int> dist;
        std::unordered_map<int, int> parent;
        graph_.dijkstra(c1.locationId, dist, parent);

        int travel = INF;
        auto it = dist.find(c2.locationId);
        if (it != dist.end()) {
            travel = it->second;
        }

        bool canMake = (travel != INF && gap >= travel);

        std::cout << c1.code << " - " << c2.code << " ";
        if (canMake) {
            std::cout << "Can make it!\n";
        } else {
            std::cout << "Cannot make it!\n";
        }
    }
}

// -----------------------------
// Main command parser
// -----------------------------
bool CampusCompass::ParseCommand(const std::string& command) {
    if (command.empty()) {
        std::cout << "unsuccessful\n";
        return false;
    }

    std::stringstream ss(command);
    std::string cmd;
    ss >> cmd;

    if (cmd == "insert") {
        bool ok = handleInsert(command);
        std::cout << (ok ? "successful\n" : "unsuccessful\n");
        return true;
    }
    else if (cmd == "remove") {
        std::string ufid;
        if (!(ss >> ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        bool ok = students_.removeStudent(ufid);
        std::cout << (ok ? "successful\n" : "unsuccessful\n");
        return true;
    }
    else if (cmd == "dropClass") {
        std::string ufid, code;
        if (!(ss >> ufid >> code)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid) || !isValidClassCode(code) || !catalog_.hasClass(code)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        bool ok = students_.dropClass(ufid, code);
        std::cout << (ok ? "successful\n" : "unsuccessful\n");
        return true;
    }
    else if (cmd == "replaceClass") {
        std::string ufid, c1, c2;
        if (!(ss >> ufid >> c1 >> c2)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid) ||
            !isValidClassCode(c1) ||
            !isValidClassCode(c2) ||
            !catalog_.hasClass(c2)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!catalog_.hasClass(c1)) {
            std::cout << "unsuccessful\n";
            return true;
        }

        bool ok = students_.replaceClass(ufid, c1, c2);
        std::cout << (ok ? "successful\n" : "unsuccessful\n");
        return true;
    }
    else if (cmd == "removeClass") {
        std::string code;
        if (!(ss >> code)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidClassCode(code)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        int count = students_.removeClassFromAll(code);
        std::cout << count << "\n";
        return true;
    }
    else if (cmd == "toggleEdgesClosure") {
        int N;
        if (!(ss >> N) || N < 0) {
            std::cout << "unsuccessful\n";
            return true;
        }
        std::vector<int> ids;
        int x;
        while (ss >> x) {
            ids.push_back(x);
        }
        if (static_cast<int>(ids.size()) != 2 * N) {
            std::cout << "unsuccessful\n";
            return true;
        }
        for (int i = 0; i < 2 * N; i += 2) {
            int u = ids[i];
            int v = ids[i + 1];
            graph_.toggleEdge(u, v);
        }
        std::cout << "successful\n";
        return true;
    }
    else if (cmd == "checkEdgeStatus") {
        int u, v;
        if (!(ss >> u >> v)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        std::string status = graph_.edgeStatus(u, v);
        std::cout << status << "\n";
        return true;
    }
    else if (cmd == "isConnected") {
        int u, v;
        if (!(ss >> u >> v)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        bool ok = graph_.isConnected(u, v);
        std::cout << (ok ? "successful\n" : "unsuccessful\n");
        return true;
    }
    else if (cmd == "printShortestEdges") {
        std::string ufid;
        if (!(ss >> ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        handlePrintShortestEdges(ufid);
        return true;
    }
    else if (cmd == "printStudentZone") {
        std::string ufid;
        if (!(ss >> ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        handlePrintStudentZone(ufid);
        return true;
    }
    else if (cmd == "verifySchedule") {
        std::string ufid;
        if (!(ss >> ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        if (!isValidUFID(ufid)) {
            std::cout << "unsuccessful\n";
            return true;
        }
        handleVerifySchedule(ufid);
        return true;
    }

    // Unknown command
    std::cout << "unsuccessful\n";
    return false;
}