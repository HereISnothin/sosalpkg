#include "sosalpkg.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: sosalpkg <command> [options]" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "update") {
        SosalPkg::update();
    } else if (command == "install" && argc == 3) {
        SosalPkg::install(argv[2]);
    } else if (command == "remove" && argc == 3) {
        SosalPkg::remove(argv[2]);
    } else if (command == "installed") {
        SosalPkg::installed();
    } else if (command == "list") {
        SosalPkg::list();
    } else if (command == "search" && argc == 3) {
        SosalPkg::search(argv[2]);
    } else {
        std::cout << "Unknown command" << std::endl;
    }

    return 0;
}