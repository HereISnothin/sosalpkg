#include "sosalpkg.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <zlib.h>
#include <map>
#include <sstream>

namespace fs = std::filesystem;

namespace SosalPkg {

const std::string CACHE_DIR = "cache/";
const std::string PACKAGE_GZ = "package_db/Packages.gz";

void ensure_cache_dir() {
    if (!fs::exists(CACHE_DIR)) {
        fs::create_directory(CACHE_DIR);
    }
}

void update() {
    std::cout << "Updating package list..." << std::endl;
    system(("curl -s http://example.com/Packages.gz -o " + PACKAGE_GZ).c_str());
}

void install(const std::string& packageName) {
    std::cout << "Installing " << packageName << "..." << std::endl;
    ensure_cache_dir();
    std::string debPath = CACHE_DIR + packageName + ".deb";
    
    if (!fs::exists(debPath)) {
        std::cout << "Downloading " << packageName << " to cache..." << std::endl;
        std::string cmd = "wget http://example.com/packages/" + packageName + ".deb -O " + debPath;
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Failed to download package." << std::endl;
            return;
        }
    } else {
        std::cout << "Using cached package." << std::endl;
    }

    std::string installCmd = "sudo dpkg -i " + debPath;
    system(installCmd.c_str());
}

void remove(const std::string& packageName) {
    std::cout << "Removing " << packageName << "..." << std::endl;
    std::string cmd = "sudo dpkg -r " + packageName;
    system(cmd.c_str());
}
    std::map<std::string, std::string> parse_package_block(const std::string& block) {
    std::map<std::string, std::string> info;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            while (!val.empty() && (val[0] == ' ' || val[0] == '\t')) val.erase(0, 1);
            info[key] = val;
        }
    }
    return info;
}

void list() {
    gzFile file = gzopen(PACKAGE_GZ.c_str(), "rb");
    if (!file) {
        std::cerr << "Failed to open package list." << std::endl;
        return;
    }

    char buffer[4096];
    std::string content;
    int bytesRead = 0;
    while ((bytesRead = gzread(file, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }
    gzclose(file);

    std::istringstream stream(content);
    std::string block;
    while (std::getline(stream, block, '\n')) {
        std::string line;
        std::string fullBlock;
        while (!block.empty() && block != "\n") {
            fullBlock += block + "\n";
            if (!std::getline(stream, block, '\n')) break;
        }
        auto info = parse_package_block(fullBlock);
        if (!info["Package"].empty()) {
            std::cout << info["Package"];
            if (!info["Version"].empty()) std::cout << " (" << info["Version"] << ")";
            if (!info["Description"].empty()) std::cout << " - " << info["Description"];
            std::cout << std::endl;
        }
    }
}
void search(const std::string& term) {
    gzFile file = gzopen(PACKAGE_GZ.c_str(), "rb");
    if (!file) {
        std::cerr << "Failed to open package list." << std::endl;
        return;
    }

    char buffer[4096];
    std::string content;
    int bytesRead = 0;
    while ((bytesRead = gzread(file, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }
    gzclose(file);

    std::istringstream stream(content);
    std::string block;
    while (std::getline(stream, block, '\n')) {
        std::string line;
        std::string fullBlock;
        while (!block.empty() && block != "\n") {
            fullBlock += block + "\n";
            if (!std::getline(stream, block, '\n')) break;
        }
        auto info = parse_package_block(fullBlock);
        if (!info["Package"].empty() &&
            (info["Package"].find(term) != std::string::npos ||
             info["Description"].find(term) != std::string::npos)) {
            std::cout << info["Package"];
            if (!info["Version"].empty()) std::cout << " (" << info["Version"] << ")";
            if (!info["Description"].empty()) std::cout << " - " << info["Description"];
            std::cout << std::endl;
        }
    }
}

void installed() {
    std::cout << "Installed packages:\n";
    system("dpkg-query -W -f='${Package} ${Version}\n'");
}

}