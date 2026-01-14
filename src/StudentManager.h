#pragma once

#include "Student.h"
#include <unordered_map>
#include <string>

class StudentManager {
public:
    StudentManager() = default;

    bool hasStudent(const std::string& ufid) const;

    // Add a new student; returns false if UFID already exists
    bool addStudent(const Student& s);

    // Remove a student completely; returns false if they don't exist
    bool removeStudent(const std::string& ufid);

    // Drop a class from one student
    // - If the student ends up with 0 classes, they are removed.
    // - Returns false if student or class not found.
    bool dropClass(const std::string& ufid, const std::string& classCode);

    // Replace classCodeFrom with classCodeTo for this student
    // - Fails if student doesn't exist
    // - Fails if they don't have classCodeFrom or classCodeTo
    bool replaceClass(const std::string& ufid,
                      const std::string& classCodeFrom,
                      const std::string& classCodeTo);

    // Remove a class from all students
    // - Again, if a student ends up with 0 classes, they are removed
    // - Returns the number of students that had this class removed
    int removeClassFromAll(const std::string& classCode);

    // Read-only lookup
    const Student* getStudent(const std::string& ufid) const;

    // Mutable lookup 
    Student* getStudentMutable(const std::string& ufid);

private:
    // key: UFID (string)
    std::unordered_map<std::string, Student> students_;
};