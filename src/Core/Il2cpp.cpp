#include "Il2cpp.hpp"
#include "Dumper.hpp"

MemoryInfo getBaseAddress(const std::string &fileName) {
    MemoryInfo _info;

    const uint32_t imageCount = _dyld_image_count();

    for (uint32_t i = 0; i < imageCount; i++) {
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

namespace Variables {
    MemoryInfo info;

    namespace IL2CPP {
        void *(*il2cpp_domain_get)() = nullptr;
        void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size) = nullptr;

        const void *(*il2cpp_assembly_get_image)(const void *assembly) = nullptr;

        const char *(*il2cpp_image_get_name)(void *image) = nullptr;
        size_t (*il2cpp_image_get_class_count)(const Il2CppImage *image) = nullptr;
        Il2CppClass *(*il2cpp_image_get_class)(const Il2CppImage *image, size_t index) = nullptr;

        const char *(*il2cpp_class_get_name)(void *klass) = nullptr;
        const char *(*il2cpp_class_get_namespace)(void *klass) = nullptr;
        int32_t (*il2cpp_class_get_flags)(void *klass) = nullptr;
        void *(*il2cpp_class_get_methods)(void *klass, void **iter) = nullptr;
        void *(*il2cpp_class_get_fields)(void *klass, void **iter) = nullptr;
        void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount) = nullptr;
        void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name) = nullptr;
        void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name) = nullptr;
        void *(*il2cpp_class_from_type)(void *type) = nullptr;
        bool (*il2cpp_class_is_enum)(void *klass) = nullptr;
        bool (*il2cpp_class_is_valuetype)(void *klass) = nullptr;

        const char *(*il2cpp_method_get_name)(void *method) = nullptr;
        void *(*il2cpp_method_get_param)(void *method, uint32_t index) = nullptr;
        const char *(*il2cpp_method_get_param_name)(void *method, uint32_t index) = nullptr;
        int32_t (*il2cpp_method_get_param_count)(void *method) = nullptr;
        void *(*il2cpp_method_get_return_type)(void *method) = nullptr;
        uint32_t (*il2cpp_method_get_flags)(void *method, uint32_t *iflags) = nullptr;

        void *(*il2cpp_class_get_properties)(void *klass, void **iter) = nullptr;
        const char *(*il2cpp_property_get_name)(void *property) = nullptr;
        void *(*il2cpp_property_get_get_method)(void *property) = nullptr;
        void *(*il2cpp_property_get_set_method)(void *property) = nullptr;

        const char *(*il2cpp_field_get_name)(void *field) = nullptr;
        Il2CppType *(*il2cpp_field_get_type)(void *field) = nullptr;
        int32_t (*il2cpp_field_get_flags)(void *field) = nullptr;
        size_t (*il2cpp_field_get_offset)(void *field) = nullptr;
        void (*il2cpp_field_static_get_value)(void *field, void *value) = nullptr;
        void (*il2cpp_field_static_set_value)(void *field, void *value) = nullptr;

        void *(*il2cpp_string_new)(const char *str) = nullptr;
        void *(*il2cpp_string_new_utf16)(const wchar_t *str, int32_t length) = nullptr;
        uint16_t *(*il2cpp_string_chars)(void *str) = nullptr;
        
        char *(*il2cpp_type_get_name)(void *type) = nullptr;
        bool (*il2cpp_type_is_byref)(void *type) = nullptr;
        uint32_t (*il2cpp_type_get_attrs)(void *type) = nullptr;


        void processAttach(const char *dir) {
        #define GETAPI(returnType, name, ...)                                           \
            Variables::IL2CPP::name = (returnType(*)(__VA_ARGS__))dlsym(handle, #name); \
            if (!Variables::IL2CPP::name) {                                             \
                Dumper::status = Dumper::DumpStatus::ERROR_SYMBOLS;                     \
                return;                                                                 \
            }

            void *handle = dlopen(dir, RTLD_LAZY);
            while (!handle) {
                Dumper::status = Dumper::DumpStatus::ERROR_FRAMEWORK;

                handle = dlopen(dir, RTLD_LAZY);
                sleep(1);
            }

            info = getBaseAddress(BINARY_NAME);

            GETAPI(void *, il2cpp_domain_get, void);
            GETAPI(void **, il2cpp_domain_get_assemblies, const void *, size_t *);

            GETAPI(const void *, il2cpp_assembly_get_image, const void *);

            GETAPI(const char *, il2cpp_image_get_name, void *);
            GETAPI(size_t, il2cpp_image_get_class_count, const Il2CppImage *);
            GETAPI(Il2CppClass *, il2cpp_image_get_class, const Il2CppImage *, size_t);

            GETAPI(const char *, il2cpp_class_get_name, void *);
            GETAPI(const char *, il2cpp_class_get_namespace, void *);
            GETAPI(int32_t, il2cpp_class_get_flags, void *);
            GETAPI(void *, il2cpp_class_get_methods, void *, void **);
            GETAPI(void *, il2cpp_class_get_fields, void *, void **);
            GETAPI(void *, il2cpp_class_get_method_from_name, void *, const char *, int);
            GETAPI(void *, il2cpp_class_get_field_from_name, void *, const char *);
            GETAPI(void *, il2cpp_class_from_name, const void *, const char *, const char *);
            GETAPI(void *, il2cpp_class_from_type, void *);
            GETAPI(bool, il2cpp_class_is_enum, void *);
            GETAPI(bool, il2cpp_class_is_valuetype, void *);

            GETAPI(const char *, il2cpp_method_get_name, void *);
            GETAPI(void *, il2cpp_method_get_param, void *, uint32_t);
            GETAPI(const char *, il2cpp_method_get_param_name, void *, uint32_t);
            GETAPI(int32_t, il2cpp_method_get_param_count, void *);
            GETAPI(void *, il2cpp_method_get_return_type, void *);
            GETAPI(uint32_t, il2cpp_method_get_flags, void *, uint32_t *);

            GETAPI(void *, il2cpp_class_get_properties, void *, void **);
            GETAPI(const char *, il2cpp_property_get_name, void *);
            GETAPI(void *, il2cpp_property_get_get_method, void *);
            GETAPI(void *, il2cpp_property_get_set_method, void *);

            GETAPI(const char *, il2cpp_field_get_name, void *);
            GETAPI(Il2CppType *, il2cpp_field_get_type, void *);
            GETAPI(int32_t, il2cpp_field_get_flags, void *);
            GETAPI(size_t, il2cpp_field_get_offset, void *);
            GETAPI(void, il2cpp_field_static_get_value, void *, void *);
            GETAPI(void, il2cpp_field_static_set_value, void *, void *);

            GETAPI(void *, il2cpp_string_new, const char *);
            GETAPI(void *, il2cpp_string_new_utf16, const wchar_t *, int32_t);
            GETAPI(uint16_t *, il2cpp_string_chars, void *);

            GETAPI(char *, il2cpp_type_get_name, void *);
            GETAPI(bool, il2cpp_type_is_byref, void *);
            GETAPI(uint32_t, il2cpp_type_get_attrs, void *);
        }
    }  // namespace IL2CPP
}  // namespace Variables
