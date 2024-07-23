
#include <cstdio>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <string>
#include <utility>
#include <functional>

#include <hash/hash.h>

class File
{
public:
    FILE *file;

    File() : file(nullptr) {}
    File(const std::string &path, const char *mode)
    {
        file = fopen(path.c_str(), mode);
    }
    ~File()
    {
        if (file)
        {
            fclose(file);
        }
    }
	inline void open(const std::string &path, const char *mode) { file = fopen(path.c_str(), mode); }
	inline void close() { if (file) { fclose(file); file = nullptr; } }

    bool ok() const { return file != nullptr; }
    operator FILE *() { return file; }
};