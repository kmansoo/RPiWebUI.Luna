#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "LunaSystemCall.h"

std::string Luna_system_exec(const std::string& command) {
    char buffer[128];
    std::string result;

#ifdef _WIN32
    std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
#endif

    if (!pipe) 
        std::cout << "pipe == null" << std::endl;

    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }

    return result;
}
