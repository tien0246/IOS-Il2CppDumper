
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class File {
   public:
    FILE *file;

    File() : file(nullptr) {}
    File(const std::string &path, const char *mode) {
        file = fopen(path.c_str(), mode);
    }
    ~File() {
        if (file) {
            fclose(file);
        }
    }
    inline void open(const std::string &path, const char *mode) { file = fopen(path.c_str(), mode); }
    inline void close() {
        if (file) {
            fclose(file);
            file = nullptr;
        }
    }
    inline void write(const char *data) { fwrite(data, 1, strlen(data), file); }
    inline void write(std::string &data) { fwrite(data.c_str(), 1, data.size(), file); }
    inline void write(std::stringstream &data) { fwrite(data.str().c_str(), 1, data.str().size(), file); }

    bool ok() const { return file != nullptr; }
    operator FILE *() { return file; }
};