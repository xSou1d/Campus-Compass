#pragma once

#include <string>
#include <vector>

struct Student {
    std::string name;
    std::string ufid;
    int residenceLocationId;
    std::vector<std::string> classes;
};