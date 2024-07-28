#ifndef IL2CPP_HPP
#define IL2CPP_HPP

#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <unistd.h>

#include <cstdint>
#include <string>
#include <vector>

#include "config.h"

#pragma GCC diagnostic ignored "-Wundefined-internal"

class MemoryInfo
{
public:
    uint32_t index;
    const mach_header *header;
    const char *name;
    intptr_t address;

    MemoryInfo getBaseAddress(const std::string &fileName);
};

namespace Variables
{

    extern MemoryInfo info;

    struct Il2CppClass
    {
        const char *name;    
        const char *namespace_;

        Il2CppClass(const char *name, const char *namespace_)
            : name(name), namespace_(namespace_) {}
    };

    struct Il2CppImage
    {
        const char *name;     
        size_t class_count;  
        Il2CppClass **classes;

        Il2CppImage(const char *name, size_t class_count, Il2CppClass **classes)
            : name(name), class_count(class_count), classes(classes) {}
    };

    struct Il2CppType
    {
        int type;
        int attrs;

        Il2CppType(int t, int a) : type(t), attrs(a) {}
    };

    namespace IL2CPP
    {
        // domain
        extern void *(*il2cpp_domain_get)();
        extern void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size);

        // assembly
        extern const void *(*il2cpp_assembly_get_image)(const void *assembly);

        // image
        extern const char *(*il2cpp_image_get_name)(void *image);
        extern size_t (*il2cpp_image_get_class_count)(const Il2CppImage *image);
        extern Il2CppClass *(*il2cpp_image_get_class)(const Il2CppImage *image, size_t index);

        // class
        extern const char *(*il2cpp_class_get_name)(void *klass);
        extern const char *(*il2cpp_class_get_namespace)(void *klass);
        extern int32_t (*il2cpp_class_get_flags)(void *klass);
        extern void *(*il2cpp_class_get_methods)(void *klass, void **iter);
        extern void *(*il2cpp_class_get_fields)(void *klass, void **iter);
        extern void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount);
        extern void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name);
        extern void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name);
        extern void *(*il2cpp_class_from_type)(void *type);
        extern bool (*il2cpp_class_is_enum)(void *klass);
        extern bool (*il2cpp_class_is_valuetype)(void *klass);

        // method
        extern const char *(*il2cpp_method_get_name)(void *method);
        extern void *(*il2cpp_method_get_param)(void *method, uint32_t index);
        extern const char *(*il2cpp_method_get_param_name)(void *method, uint32_t index);
        extern int32_t (*il2cpp_method_get_param_count)(void *method);
        extern void *(*il2cpp_method_get_return_type)(void *method);
        extern uint32_t (*il2cpp_method_get_flags)(void *method, uint32_t *iflags);

        // property
        extern void* (*il2cpp_class_get_properties)(void *klass, void **iter);
        extern const char *(*il2cpp_property_get_name)(void *property);
        extern void *(*il2cpp_property_get_get_method)(void *property);
        extern void *(*il2cpp_property_get_set_method)(void *property);

        // field
        extern const char *(*il2cpp_field_get_name)(void *field);
        extern Il2CppType *(*il2cpp_field_get_type)(void *field);
        extern int32_t (*il2cpp_field_get_flags)(void *field);
        extern size_t (*il2cpp_field_get_offset)(void *field);
        extern void (*il2cpp_field_static_get_value)(void *field, void *value);
        extern void (*il2cpp_field_static_set_value)(void *field, void *value);

        // string
        extern void *(*il2cpp_string_new)(const char *str);
        extern void *(*il2cpp_string_new_utf16)(const wchar_t *str, int32_t length);
        extern uint16_t *(*il2cpp_string_chars)(void *str);

        // type
        extern char *(*il2cpp_type_get_name)(void *type);
        extern bool (*il2cpp_type_is_byref)(void *type);
        extern uint32_t (*il2cpp_type_get_attrs)(void *type);

        void processAttach(const char *unitydir);
    }

}

#endif
