#include "StudentManager.h"

#include <algorithm>

bool StudentManager::hasStudent(const std::string& ufid) const {
    return students_.find(ufid) != students_.end();
}

bool StudentManager::addStudent(const Student& s) {
    if (hasStudent(s.ufid)) {
        return false;
    }
    students_[s.ufid] = s;
    return true;
}

bool StudentManager::removeStudent(const std::string& ufid) {
    auto it = students_.find(ufid);
    if (it == students_.end()) {
        return false;
    }
    students_.erase(it);
    return true;
}

bool StudentManager::dropClass(const std::string& ufid, const std::string& classCode) {
    auto it = students_.find(ufid);
    if (it == students_.end()) {
        return false;
    }

    Student& s = it->second;
    auto cit = std::find(s.classes.begin(), s.classes.end(), classCode);
    if (cit == s.classes.end()) {
        return false; // student doesn't have this class
    }

    s.classes.erase(cit);

    // If student now has 0 classes, remove them entirely
    if (s.classes.empty()) {
        students_.erase(it);
    }

    return true;
}

bool StudentManager::replaceClass(const std::string& ufid,
                                  const std::string& classCodeFrom,
                                  const std::string& classCodeTo) {
    auto it = students_.find(ufid);
    if (it == students_.end()) {
        return false;
    }

    Student& s = it->second;

    auto fromIt = std::find(s.classes.begin(), s.classes.end(), classCodeFrom);
    if (fromIt == s.classes.end()) {
        return false; // doesn't have classCodeFrom
    }

    if (std::find(s.classes.begin(), s.classes.end(), classCodeTo) != s.classes.end()) {
        return false; // already has classCodeTo
    }

    *fromIt = classCodeTo;
    return true;
}

int StudentManager::removeClassFromAll(const std::string& classCode) {
    int count = 0;

    for (auto it = students_.begin(); it != students_.end();) {
        Student& s = it->second;

        bool hadClass = false;

        auto& vec = s.classes;
        auto newEnd = std::remove(vec.begin(), vec.end(), classCode);
        if (newEnd != vec.end()) {
            hadClass = true;
            vec.erase(newEnd, vec.end());
        }

        if (hadClass) {
            ++count;
        }

        if (vec.empty()) {
            it = students_.erase(it);
        } else {
            ++it;
        }
    }

    return count;
}

const Student* StudentManager::getStudent(const std::string& ufid) const {
    auto it = students_.find(ufid);
    if (it == students_.end()) {
        return nullptr;
    }
    return &it->second;
}

Student* StudentManager::getStudentMutable(const std::string& ufid) {
    auto it = students_.find(ufid);
    if (it == students_.end()) {
        return nullptr;
    }
    return &it->second;
}