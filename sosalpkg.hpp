#pragma once
#include <string>

namespace SosalPkg {
    void update();
    void install(const std::string& packageName);
    void remove(const std::string& packageName);
    void list();
    void search(const std::string& term);
    void installed();
}