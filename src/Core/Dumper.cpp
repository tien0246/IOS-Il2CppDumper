#include "Dumper.hpp"
#include "Il2cpp.hpp"
#include "Util.hpp"

#include <fmt/core.h>

#include <sstream>

#pragma GCC diagnostic ignored "-Wundefined-internal"

namespace Dumper
{
    void *domain = nullptr;
    DumpStatus status = DumpStatus::NONE;
}

void Dumper::init()
{
    Dumper::domain = Variables::IL2CPP::il2cpp_domain_get();
}

std::vector<void *> Dumper::getAssemblies()
{
    size_t size;
    void **assemblies = Variables::IL2CPP::il2cpp_domain_get_assemblies(Dumper::domain, &size);
    return std::vector<void *>(assemblies, assemblies + size);
}

std::vector<void *> Dumper::getClasses(void *image)
{
    std::vector<void *> classes;
    const Variables::Il2CppImage *il2cppImage = static_cast<Variables::Il2CppImage *>(image);
    size_t classCount = Variables::IL2CPP::il2cpp_image_get_class_count(il2cppImage);

    for (size_t i = 0; i < classCount; ++i)
    {
        classes.push_back(Variables::IL2CPP::il2cpp_image_get_class(il2cppImage, i));
    }
    return classes;
}

std::vector<void *> Dumper::getMethods(void *klass)
{
    std::vector<void *> methods;
    void *iter = nullptr;
    void *method = nullptr;

    while ((method = Variables::IL2CPP::il2cpp_class_get_methods(klass, &iter)) != nullptr)
    {
        methods.push_back(method);
    }
    return methods;
}

std::vector<void *> Dumper::getFields(void *klass)
{
    std::vector<void *> fields;
    void *iter = nullptr;
    void *field = nullptr;

    while ((field = Variables::IL2CPP::il2cpp_class_get_fields(klass, &iter)) != nullptr)
    {
        fields.push_back(field);
    }
    return fields;
}

Dumper::DumpStatus Dumper::dump(const std::string &dir, const std::string &headers_dir)
{
    Dumper::init();

    File logfile(dir + "/logs.txt", "w");
    if (!logfile.ok())
        return Dumper::DumpStatus::ERROR;

    File dumpFile(dir + "/dump.txt", "w");
    if (!dumpFile.ok())
        return Dumper::DumpStatus::ERROR;

    if (headers_dir.empty())
        return Dumper::DumpStatus::ERROR;

    fmt::print(logfile, "BaseAddress: {:#08x}\n", Variables::info.address);
    fmt::print(logfile, "==========================\n");
    fmt::print(logfile, "Init Dumping...\n");

    auto assemblies = Dumper::getAssemblies();
    fmt::print(logfile, "Total Assemblies: {}\n", assemblies.size());

    for (auto assembly : assemblies)
    {
        const void *image = Variables::IL2CPP::il2cpp_assembly_get_image(assembly);
        const char *imageName = Variables::IL2CPP::il2cpp_image_get_name((void *)image);
        fmt::print(dumpFile, "DLL: {}\n", imageName);

        std::string assemblyFileName = headers_dir + "/" + imageName + ".txt";
        File singleAssemblyFile(assemblyFileName, "w");

        auto classes = Dumper::getClasses((void *)image);
        fmt::print(logfile, "Total Classes in {}: {}\n", imageName, classes.size());

        for (auto klass : classes)
        {
            const char *className = Variables::IL2CPP::il2cpp_class_get_name(klass);
            const char *classNamespace = Variables::IL2CPP::il2cpp_class_get_namespace(klass);
            if (classNamespace)
            {
                fmt::print(dumpFile, "  Class: {}.{}\n", classNamespace, className);
                fmt::print(singleAssemblyFile, "  Class: {}.{}\n", classNamespace, className);
            }
            else
            {
                fmt::print(dumpFile, "  Class: {}\n", className);
                fmt::print(singleAssemblyFile, "  Class: {}\n", className);
            }

            auto methods = Dumper::getMethods(klass);
            fmt::print(logfile, "Total Methods in class {}: {}\n", className, methods.size());

            for (auto method : methods)
            {
                const char *methodName = Variables::IL2CPP::il2cpp_method_get_name(method);

                const char *returnType = "void";
                void *returnTypeObj = Variables::IL2CPP::il2cpp_method_get_return_type(method);
                if (returnTypeObj != nullptr)
                {
                    returnType = Variables::IL2CPP::il2cpp_type_get_name(returnTypeObj);
                }

                fmt::print(dumpFile, "    Method: {} {}(", returnType, methodName);
                fmt::print(singleAssemblyFile, "    Method: {} {}(", returnType, methodName);

                int32_t paramCount = Variables::IL2CPP::il2cpp_method_get_param_count(method);
                for (int32_t j = 0; j < paramCount; ++j)
                {
                    void *paramType = Variables::IL2CPP::il2cpp_method_get_param(method, j);
                    if (paramType)
                    {
                        void *parameter_class = Variables::IL2CPP::il2cpp_class_from_type(paramType);
                        const char *parameter_class_name = Variables::IL2CPP::il2cpp_class_get_name(parameter_class);
                        fmt::print(dumpFile, "{}", parameter_class_name);
                        fmt::print(singleAssemblyFile, "{}", parameter_class_name);
                        if (j < paramCount - 1)
                        {
                            fmt::print(dumpFile, ", ");
                            fmt::print(singleAssemblyFile, ", ");
                        }
                    }
                }
                void **methodPointer = (void **)Variables::IL2CPP::il2cpp_class_get_method_from_name(klass, methodName, paramCount);
                method = *methodPointer;
                uint64_t rvaOffset = reinterpret_cast<uint64_t>(method) - Variables::info.address;
                fmt::print(dumpFile, ") // RVA Offset: 0x{:x}\n", rvaOffset);
                fmt::print(singleAssemblyFile, ") // RVA Offset: 0x{:x}\n", rvaOffset);
            }

            auto fields = Dumper::getFields(klass);
            fmt::print(logfile, "Total Fields in class {}: {}\n", className, fields.size());

            for (auto field : fields)
            {
                const char *fieldName = Variables::IL2CPP::il2cpp_field_get_name(field);
                Variables::Il2CppType *fieldType = Variables::IL2CPP::il2cpp_field_get_type(field);

                void *field_class = Variables::IL2CPP::il2cpp_class_from_type(fieldType);
                const char *fieldTypeName = Variables::IL2CPP::il2cpp_class_get_name(field_class);

                size_t offset = Variables::IL2CPP::il2cpp_field_get_offset(field);

                fmt::print(dumpFile, "    Field: {} {} // 0x{:x}\n", fieldTypeName, fieldName, offset);
                fmt::print(singleAssemblyFile, "    Field: {} {} // 0x{:x}\n", fieldTypeName, fieldName, offset);
            }
        }
        fmt::print(dumpFile, "\n");
        fmt::print(singleAssemblyFile, "\n");
    }
    fmt::print(logfile, "Dumping Completed.\n");
    return Dumper::DumpStatus::SUCCESS;
}
