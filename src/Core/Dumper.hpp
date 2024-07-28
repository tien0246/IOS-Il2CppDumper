#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <types.h>
#include <unordered_set>

#include "Il2cpp.hpp"
#include "Util.hpp"
#include "il2cpp-tabledefs.h"
#include "../../includes/nlohmann/json.hpp"
#include "config.h"

using json = nlohmann::json;

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

    void Log(const char *fmt, ...);

    std::string uint16ToString(uint16_t *str);

    std::string toHexUnicode(char c);

    std::string convertNonAlnumToHexUnicode(const std::string& input);

    namespace GenScript {
        extern json jsonData;
        extern File scriptFile;
        extern std::unordered_set<uint64_t> dataOffsets;

        void init();
        void save();
        void addMethod(uint64_t addr, std::string namespaze, std::string klass, std::string method);
    }
}
