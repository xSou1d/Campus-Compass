#pragma once

#include <string>
#include <unordered_map>

struct ClassInfo {
    std::string code;
    int locationId;
    int startMinutes;
    int endMinutes;
};

class ClassCatalog {
public:
    ClassCatalog() = default;

    // Add or overwrite a class
    void addClass(const ClassInfo& info);

    // Check if a class exists
    bool hasClass(const std::string& code) const;

    // Get class info (nullptr if not found)
    const ClassInfo* getClass(const std::string& code) const;

private:
    std::unordered_map<std::string, ClassInfo> classes_;
};