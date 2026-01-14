#include "ClassCatalog.h"

void ClassCatalog::addClass(const ClassInfo& info) {
    classes_[info.code] = info;
}

bool ClassCatalog::hasClass(const std::string& code) const {
    return classes_.find(code) != classes_.end();
}

const ClassInfo* ClassCatalog::getClass(const std::string& code) const {
    auto it = classes_.find(code);
    if (it == classes_.end()) {
        return nullptr;
    }
    return &it->second;
}