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
    extern void *domain;  // Declare the variable
    extern std::string dumpDir;  // Declare the variable

    void init();

    DumpStatus dump(const std::string &dir, const std::string &headers_dir);

    std::string dumpField(void *klass);

    std::string dumpProperty(void *klass);

    std::string dumpMethod(void *klass);

    std::string getMethodModifier(uint32_t flags);

    std::string getClassName(void *klass);


    std::vector<void *> getAssemblies();

    std::vector<void *> getClasses(void *image);
    
    std::vector<void *> getMethods(void *klass);

    std::vector<void *> getFields(void *klass);

    void Log(const char *fmt, ...);

    std::string uint16ToString(uint16_t *str);
}
