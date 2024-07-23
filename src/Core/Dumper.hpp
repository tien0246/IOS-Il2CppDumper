#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <types.h>

namespace Dumper
{
    enum DumpStatus : int8
    {
        NONE = -1,
        SUCCESS = 0,
        ERROR = 1,
        ERROR_SYMBOLS = 2,
        ERROR_FRAMEWORK = 3
    };

    extern DumpStatus status;  // Declare the variable

    void init();

    DumpStatus dump(const std::string &dir, const std::string &headers_dir);

    extern void *domain;  // Declare the variable

    std::vector<void *> getAssemblies();

    std::vector<void *> getClasses(void *image);
    
    std::vector<void *> getMethods(void *klass);

    std::vector<void *> getFields(void *klass);

}
