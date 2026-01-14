#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Graph.h"
#include "ClassCatalog.h"
#include "StudentManager.h"

class CampusCompass {
public:
    CampusCompass();

    // Load edges.csv and classes.csv
    bool ParseCSV(const std::string& edges_filepath,
                  const std::string& classes_filepath);

    // Parse one command line, execute it, and print required output
    // Returns true if the command keyword was recognized,
    // false if the command keyword itself was invalid.
    bool ParseCommand(const std::string& command);

private:
    // -----------------------------
    // Internal Data Storage
    // -----------------------------
    Graph graph_;                 // campus walk graph
    ClassCatalog catalog_;        // all classes + locations
    StudentManager students_;     // all students enrolled in system

    // -----------------------------
    // Validation Helpers
    // -----------------------------
    bool isValidUFID(const std::string& ufid) const;
    bool isValidName(const std::string& name) const;
    bool isValidClassCode(const std::string& code) const;

    // -----------------------------
    // Parsing Helpers
    // -----------------------------
    int parseTimeToMinutes(const std::string& t) const;

    // -----------------------------
    // CSV Loader Helpers
    // -----------------------------
    bool loadEdges(const std::string& path);
    bool loadClasses(const std::string& path);

    // -----------------------------
    // Command-specific helpers
    // -----------------------------
    bool handleInsert(const std::string& fullLine);
    void handlePrintShortestEdges(const std::string& ufid) const;
    void handlePrintStudentZone(const std::string& ufid) const;
    void handleVerifySchedule(const std::string& ufid) const;
};