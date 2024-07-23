#include "Il2cpp.hpp"
#include "Dumper.hpp"
#include "Util.hpp"

#include <unistd.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <fmt/core.h>

MemoryInfo getBaseAddress(const std::string &fileName)
{
    MemoryInfo _info;

    const uint32_t imageCount = _dyld_image_count();

    for (uint32_t i = 0; i < imageCount; i++)
    {
        const char *name = _dyld_get_image_name(i);
        if (!name)
            continue;

        std::string fullpath(name);

        if (fullpath.length() < fileName.length() || fullpath.compare(fullpath.length() - fileName.length(), fileName.length(), fileName) != 0)
            continue;

        _info.index = i;
        _info.header = _dyld_get_image_header(i);
        _info.name = _dyld_get_image_name(i);
        _info.address = _dyld_get_image_vmaddr_slide(i);

        break;
    }
    return _info;
}

namespace Variables
{
    MemoryInfo info;

    namespace IL2CPP
    {
        const void *(*il2cpp_assembly_get_image)(const void *assembly) = nullptr;
        void *(*il2cpp_domain_get)() = nullptr;
        void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size) = nullptr;
        const char *(*il2cpp_image_get_name)(void *image) = nullptr;
        void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name) = nullptr;
        const char *(*il2cpp_class_get_name)(void *klass) = nullptr;
        const char *(*il2cpp_class_get_namespace)(void *klass) = nullptr;
        void *(*il2cpp_class_get_methods)(void *klass, void **iter) = nullptr;
        void *(*il2cpp_class_get_fields)(void *klass, void **iter) = nullptr;
        const char *(*il2cpp_method_get_name)(void *method) = nullptr;
        const char *(*il2cpp_field_get_name)(void *field) = nullptr;
        Il2CppType *(*il2cpp_field_get_type)(void *field) = nullptr;
        size_t (*il2cpp_field_get_offset)(void *field) = nullptr;
        void (*il2cpp_field_static_get_value)(void *field, void *value) = nullptr;
        void (*il2cpp_field_static_set_value)(void *field, void *value) = nullptr;
        void *(*il2cpp_string_new)(const char *str) = nullptr;
        void *(*il2cpp_string_new_utf16)(const wchar_t *str, int32_t length) = nullptr;
        uint16_t *(*il2cpp_string_chars)(void *str) = nullptr;
        char *(*il2cpp_type_get_name)(void *type) = nullptr;
        void *(*il2cpp_method_get_param)(void *method, uint32_t index) = nullptr;
        void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount) = nullptr;
        void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name) = nullptr;
        size_t (*il2cpp_image_get_class_count)(const Il2CppImage *image) = nullptr;
        Il2CppClass *(*il2cpp_image_get_class)(const Il2CppImage *image, size_t index) = nullptr;
        int32_t (*il2cpp_method_get_param_count)(void *method) = nullptr;
        void *(*il2cpp_method_get_return_type)(void *method) = nullptr;
        void *(*il2cpp_class_from_type)(void *type) = nullptr;

        void processAttach(const char *dir)
        {

            // File logfile(logdir + "/logs.txt", "w");
            // if (!logfile.ok()) {
            //     Dumper::status = Dumper::DumpStatus::ERROR;
            // }

            void *handle = dlopen(dir, RTLD_LAZY);
            while (!handle)
            {

                Dumper::status = Dumper::DumpStatus::ERROR_FRAMEWORK;

                //fmt::print(logfile, "Can't find framework, try again...\n");

                handle = dlopen(dir, RTLD_LAZY);
                sleep(1);
            }

            //fmt::print(logfile, "Framework found, start to extract symbols...\n");

            info = getBaseAddress("UnityFramework");

            Variables::IL2CPP::il2cpp_assembly_get_image = reinterpret_cast<const void *(*)(const void *)>(dlsym(handle, "il2cpp_assembly_get_image"));
            Variables::IL2CPP::il2cpp_domain_get = reinterpret_cast<void *(*)()>(dlsym(handle, "il2cpp_domain_get"));
            Variables::IL2CPP::il2cpp_domain_get_assemblies = reinterpret_cast<void **(*)(const void *, size_t *)>(dlsym(handle, "il2cpp_domain_get_assemblies"));
            Variables::IL2CPP::il2cpp_image_get_name = reinterpret_cast<const char *(*)(void *)>(dlsym(handle, "il2cpp_image_get_name"));
            Variables::IL2CPP::il2cpp_class_from_name = reinterpret_cast<void *(*)(const void *, const char *, const char *)>(dlsym(handle, "il2cpp_class_from_name"));
            Variables::IL2CPP::il2cpp_class_get_methods = reinterpret_cast<void *(*)(void *, void **)>(dlsym(handle, "il2cpp_class_get_methods"));
            Variables::IL2CPP::il2cpp_class_get_fields = reinterpret_cast<void *(*)(void *, void **)>(dlsym(handle, "il2cpp_class_get_fields"));
            Variables::IL2CPP::il2cpp_class_get_name = reinterpret_cast<const char *(*)(void *)>(dlsym(handle, "il2cpp_class_get_name"));
            Variables::IL2CPP::il2cpp_class_get_namespace = reinterpret_cast<const char *(*)(void *)>(dlsym(handle, "il2cpp_class_get_namespace"));
            Variables::IL2CPP::il2cpp_method_get_name = reinterpret_cast<const char *(*)(void *)>(dlsym(handle, "il2cpp_method_get_name"));
            Variables::IL2CPP::il2cpp_field_get_name = reinterpret_cast<const char *(*)(void *)>(dlsym(handle, "il2cpp_field_get_name"));
            Variables::IL2CPP::il2cpp_field_get_type = reinterpret_cast<Il2CppType *(*)(void *)>(dlsym(handle, "il2cpp_field_get_type"));
            Variables::IL2CPP::il2cpp_field_get_offset = reinterpret_cast<size_t (*)(void *)>(dlsym(handle, "il2cpp_field_get_offset"));
            Variables::IL2CPP::il2cpp_field_static_get_value = reinterpret_cast<void (*)(void *, void *)>(dlsym(handle, "il2cpp_field_static_get_value"));
            Variables::IL2CPP::il2cpp_field_static_set_value = reinterpret_cast<void (*)(void *, void *)>(dlsym(handle, "il2cpp_field_static_set_value"));

            Variables::IL2CPP::il2cpp_string_new = reinterpret_cast<void *(*)(const char *)>(dlsym(handle, "il2cpp_string_new"));
            Variables::IL2CPP::il2cpp_string_new_utf16 = reinterpret_cast<void *(*)(const wchar_t *, int32_t)>(dlsym(handle, "il2cpp_string_new_utf16"));
            Variables::IL2CPP::il2cpp_string_chars = reinterpret_cast<uint16_t *(*)(void *)>(dlsym(handle, "il2cpp_string_chars"));

            Variables::IL2CPP::il2cpp_type_get_name = reinterpret_cast<char *(*)(void *)>(dlsym(handle, "il2cpp_type_get_name"));
            Variables::IL2CPP::il2cpp_method_get_param = reinterpret_cast<void *(*)(void *, uint32_t)>(dlsym(handle, "il2cpp_method_get_param"));

            Variables::IL2CPP::il2cpp_class_get_method_from_name = reinterpret_cast<void *(*)(void *, const char *, int)>(dlsym(handle, "il2cpp_class_get_method_from_name"));
            Variables::IL2CPP::il2cpp_class_get_field_from_name = reinterpret_cast<void *(*)(void *, const char *)>(dlsym(handle, "il2cpp_class_get_field_from_name"));

            Variables::IL2CPP::il2cpp_image_get_class_count = reinterpret_cast<size_t (*)(const Il2CppImage *)>(dlsym(handle, "il2cpp_image_get_class_count"));
            Variables::IL2CPP::il2cpp_image_get_class = reinterpret_cast<Il2CppClass *(*)(const Il2CppImage *, size_t)>(dlsym(handle, "il2cpp_image_get_class"));

            Variables::IL2CPP::il2cpp_method_get_param_count = reinterpret_cast<int32_t (*)(void *)>(dlsym(handle, "il2cpp_method_get_param_count"));
            Variables::IL2CPP::il2cpp_method_get_return_type = reinterpret_cast<void *(*)(void *)>(dlsym(handle, "il2cpp_method_get_return_type"));
            Variables::IL2CPP::il2cpp_class_from_type = reinterpret_cast<void *(*)(void *)>(dlsym(handle, "il2cpp_class_from_type"));

            if (!(Variables::IL2CPP::il2cpp_assembly_get_image &&
                  Variables::IL2CPP::il2cpp_domain_get &&
                  Variables::IL2CPP::il2cpp_domain_get_assemblies &&
                  Variables::IL2CPP::il2cpp_image_get_name &&
                  Variables::IL2CPP::il2cpp_class_from_name &&
                  Variables::IL2CPP::il2cpp_class_get_methods &&
                  Variables::IL2CPP::il2cpp_class_get_fields &&
                  Variables::IL2CPP::il2cpp_class_get_name &&
                  Variables::IL2CPP::il2cpp_class_get_namespace &&
                  Variables::IL2CPP::il2cpp_method_get_name &&
                  Variables::IL2CPP::il2cpp_field_get_name &&
                  Variables::IL2CPP::il2cpp_field_get_type &&
                  Variables::IL2CPP::il2cpp_field_get_offset &&
                  Variables::IL2CPP::il2cpp_field_static_get_value &&
                  Variables::IL2CPP::il2cpp_field_static_set_value &&
                  Variables::IL2CPP::il2cpp_string_new &&
                  Variables::IL2CPP::il2cpp_string_new_utf16 &&
                  Variables::IL2CPP::il2cpp_string_chars &&
                  Variables::IL2CPP::il2cpp_type_get_name &&
                  Variables::IL2CPP::il2cpp_method_get_param &&
                  Variables::IL2CPP::il2cpp_class_get_method_from_name &&
                  Variables::IL2CPP::il2cpp_class_get_field_from_name &&
                  Variables::IL2CPP::il2cpp_image_get_class_count &&
                  Variables::IL2CPP::il2cpp_image_get_class &&
                  Variables::IL2CPP::il2cpp_method_get_param_count &&
                  Variables::IL2CPP::il2cpp_method_get_return_type &&
                  Variables::IL2CPP::il2cpp_class_from_type))
            {
                Dumper::status = Dumper::DumpStatus::ERROR_SYMBOLS;
                //fmt::print(logfile, "Error on extracting symbols...\n");
                //fmt::print(logfile, "Recommended action: Use IDA to find symbols address\n");
            }
            //fmt::print(logfile, "Found all symbols, proceeding with the dumper.\n");
        }
    }
}
