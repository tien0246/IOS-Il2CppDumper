#include <sstream>

#include "Dumper.hpp"
#include "Il2cpp.hpp"
#include "Util.hpp"
#include "il2cpp-tabledefs.h"

#pragma GCC diagnostic ignored "-Wundefined-internal"

namespace Dumper {
void *domain = nullptr;
DumpStatus status = DumpStatus::NONE;
std::string dumpDir = "";
}  // namespace Dumper

void Dumper::init() {
    Dumper::domain = Variables::IL2CPP::il2cpp_domain_get();
}

std::vector<void *> Dumper::getAssemblies() {
    size_t size;
    void **assemblies = Variables::IL2CPP::il2cpp_domain_get_assemblies(Dumper::domain, &size);
    return std::vector<void *>(assemblies, assemblies + size);
}

std::vector<void *> Dumper::getClasses(void *image) {
    std::vector<void *> classes;
    const Variables::Il2CppImage *il2cppImage = static_cast<Variables::Il2CppImage *>(image);
    size_t classCount = Variables::IL2CPP::il2cpp_image_get_class_count(il2cppImage);

    for (size_t i = 0; i < classCount; ++i) {
        classes.push_back(Variables::IL2CPP::il2cpp_image_get_class(il2cppImage, i));
    }
    return classes;
}

std::vector<void *> Dumper::getMethods(void *klass) {
    std::vector<void *> methods;
    void *iter = nullptr;
    void *method = nullptr;

    while ((method = Variables::IL2CPP::il2cpp_class_get_methods(klass, &iter)) != nullptr) {
        methods.push_back(method);
    }
    return methods;
}

std::vector<void *> Dumper::getFields(void *klass) {
    std::vector<void *> fields;
    void *iter = nullptr;
    void *field = nullptr;

    while ((field = Variables::IL2CPP::il2cpp_class_get_fields(klass, &iter)) != nullptr) {
        fields.push_back(field);
    }
    return fields;
}

Dumper::DumpStatus Dumper::dump(const std::string &dir, const std::string &headers_dir) {
    std::stringstream dumpOut;
    std::stringstream log;
    dumpDir = dir;

    Dumper::init();

    File dumpFile(dir + "/dump.txt", "w");
    if (!dumpFile.ok()) return Dumper::DumpStatus::ERROR;

    if (headers_dir.empty()) return Dumper::DumpStatus::ERROR;

    Log("BaseAddress: 0x%llx", Variables::info.address);
    Log("==========================");
    Log("Init Dumping...");

    auto assemblies = Dumper::getAssemblies();
    Log("Total Assemblies: %d", assemblies.size());

    for (int i = 0; i < assemblies.size(); i++) {
        const void *image = Variables::IL2CPP::il2cpp_assembly_get_image(assemblies[i]);
        dumpOut << "// Image " << i << ": " << Variables::IL2CPP::il2cpp_image_get_name((void *)image) << std::endl;
    }

    for (auto assembly : assemblies) {
        const void *image = Variables::IL2CPP::il2cpp_assembly_get_image(assembly);
        const char *imageName = Variables::IL2CPP::il2cpp_image_get_name((void *)image);

        std::string assemblyFileName = headers_dir + "/" + imageName + ".txt";
        File singleAssemblyFile(assemblyFileName, "w");
        std::stringstream singleAssemblyOutPut;
        auto classes = Dumper::getClasses((void *)image);
        Log("Total Classes in %s: %d", imageName, classes.size());

        for (auto klass : classes) {
            const char *classNamespace = Variables::IL2CPP::il2cpp_class_get_namespace(klass);
            if (classNamespace) {
                dumpOut << "// Namespace: " << classNamespace << std::endl;
                singleAssemblyOutPut << "// Namespace: " << classNamespace << std::endl;
            }

            auto flags = Variables::IL2CPP::il2cpp_class_get_flags(klass);
            auto isEnum = Variables::IL2CPP::il2cpp_class_is_enum(klass);
            auto isValueType = Variables::IL2CPP::il2cpp_class_is_valuetype(klass);
            auto visibility = flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
            switch (visibility) {
                case TYPE_ATTRIBUTE_PUBLIC:
                case TYPE_ATTRIBUTE_NESTED_PUBLIC:
                    dumpOut << "public ";
                    singleAssemblyOutPut << "public ";
                    break;
                case TYPE_ATTRIBUTE_NOT_PUBLIC:
                case TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM:
                case TYPE_ATTRIBUTE_NESTED_ASSEMBLY:
                    dumpOut << "internal ";
                    singleAssemblyOutPut << "internal ";
                    break;
                case TYPE_ATTRIBUTE_NESTED_PRIVATE:
                    dumpOut << "private ";
                    singleAssemblyOutPut << "private ";
                    break;
                case TYPE_ATTRIBUTE_NESTED_FAMILY:
                    dumpOut << "protected ";
                    singleAssemblyOutPut << "protected ";
                    break;
                case TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM:
                    dumpOut << "protected internal ";
                    singleAssemblyOutPut << "protected internal ";
                    break;
            }

            if (flags & TYPE_ATTRIBUTE_ABSTRACT && flags & TYPE_ATTRIBUTE_SEALED) {
                dumpOut << "static ";
                singleAssemblyOutPut << "static ";
            } else if (!(flags & TYPE_ATTRIBUTE_INTERFACE) && flags & TYPE_ATTRIBUTE_ABSTRACT) {
                dumpOut << "abstract ";
                singleAssemblyOutPut << "abstract ";
            } else if (!isValueType && !isEnum && flags & TYPE_ATTRIBUTE_SEALED) {
                dumpOut << "sealed ";
                singleAssemblyOutPut << "sealed ";
            }
            if (flags & TYPE_ATTRIBUTE_INTERFACE) {
                dumpOut << "interface ";
                singleAssemblyOutPut << "interface ";
            } else if (isEnum) {
                dumpOut << "enum ";
                singleAssemblyOutPut << "enum ";
            } else if (isValueType) {
                dumpOut << "struct ";
                singleAssemblyOutPut << "struct ";
            } else {
                dumpOut << "class ";
                singleAssemblyOutPut << "class ";
            }

            std::string className = getClassName(klass);
            dumpOut << className << std::endl;
            singleAssemblyOutPut << className << std::endl;

            dumpOut << "{";
            singleAssemblyOutPut << "{";

            dumpOut << dumpField(klass);
            singleAssemblyOutPut << dumpField(klass);

            dumpOut << dumpProperty(klass);
            singleAssemblyOutPut << dumpProperty(klass);

            dumpOut << dumpMethod(klass);
            singleAssemblyOutPut << dumpMethod(klass);

            dumpOut << "}\n\n";
            singleAssemblyOutPut << "}\n\n";
        }
        dumpOut << std::endl;
        singleAssemblyOutPut << std::endl;
        singleAssemblyFile.open(assemblyFileName, "w");
        singleAssemblyFile.write(singleAssemblyOutPut);
        singleAssemblyFile.close();
    }
    Log("Dumping Completed.");
    dumpFile.write(dumpOut);
    dumpFile.close();
    return Dumper::DumpStatus::SUCCESS;
}

std::string Dumper::dumpField(void *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Fields\n";
    auto is_enum = Variables::IL2CPP::il2cpp_class_is_enum(klass);
    void *iter = nullptr;
    while (auto field = Variables::IL2CPP::il2cpp_class_get_fields(klass, &iter)) {
        outPut << "\t";
        auto attrs = Variables::IL2CPP::il2cpp_field_get_flags(field);
        auto access = attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
        switch (access) {
            case FIELD_ATTRIBUTE_PRIVATE:
                outPut << "private ";
                break;
            case FIELD_ATTRIBUTE_PUBLIC:
                outPut << "public ";
                break;
            case FIELD_ATTRIBUTE_FAMILY:
                outPut << "protected ";
                break;
            case FIELD_ATTRIBUTE_ASSEMBLY:
            case FIELD_ATTRIBUTE_FAM_AND_ASSEM:
                outPut << "internal ";
                break;
            case FIELD_ATTRIBUTE_FAM_OR_ASSEM:
                outPut << "protected internal ";
                break;
        }
        if (attrs & FIELD_ATTRIBUTE_LITERAL) {
            outPut << "const ";
        } else {
            if (attrs & FIELD_ATTRIBUTE_STATIC) {
                outPut << "static ";
            }
            if (attrs & FIELD_ATTRIBUTE_INIT_ONLY) {
                outPut << "readonly ";
            }
        }

        auto field_type = Variables::IL2CPP::il2cpp_field_get_type(field);
        auto field_class = Variables::IL2CPP::il2cpp_class_from_type(field_type);
        outPut << getClassName(field_class) << " " << Variables::IL2CPP::il2cpp_field_get_name(field);

        if (attrs & FIELD_ATTRIBUTE_LITERAL && is_enum) {
            uint64_t val = 0;
            Variables::IL2CPP::il2cpp_field_static_get_value(field, &val);
            outPut << " = " << std::dec << val << ";\n";
        } else {
            outPut << "; // 0x" << std::hex << std::uppercase << Variables::IL2CPP::il2cpp_field_get_offset(field) << "\n";
        }
    }
    if (outPut.str().length() == 12) return "";  // no fields
    return outPut.str();
}

std::string Dumper::dumpProperty(void *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Properties\n";
    void *iter = nullptr;
    while (auto prop = Variables::IL2CPP::il2cpp_class_get_properties(klass, &iter)) {
        //TODO attribute
        auto get = Variables::IL2CPP::il2cpp_property_get_get_method(prop);
        auto set = Variables::IL2CPP::il2cpp_property_get_set_method(prop);
        auto prop_name = Variables::IL2CPP::il2cpp_property_get_name(prop);
        outPut << "\t";
        void *prop_class = nullptr;
        uint32_t iflags = 0;
        if (get) {
            outPut << getMethodModifier(Variables::IL2CPP::il2cpp_method_get_flags(get, &iflags));
            prop_class = Variables::IL2CPP::il2cpp_class_from_type(Variables::IL2CPP::il2cpp_method_get_return_type(get));
        } else if (set) {
            outPut << getMethodModifier(Variables::IL2CPP::il2cpp_method_get_flags(set, &iflags));
            auto param = Variables::IL2CPP::il2cpp_method_get_param(set, 0);
            prop_class = Variables::IL2CPP::il2cpp_class_from_type(param);
        }
        if (prop_class) {
            outPut << getClassName(prop_class) << " " << prop_name << " { ";
            if (get) {
                outPut << "get; ";
            }
            if (set) {
                outPut << "set; ";
            }
            outPut << "}\n";
        } else {
            if (prop_name) {
                outPut << " // unknown property " << prop_name;
            }
        }
    }
    if (outPut.str().length() == 16) return "";  // no properties
    return outPut.str();
}

std::string Dumper::dumpMethod(void *klass) {
    std::stringstream outPut;
    outPut << "\n\t// Methods\n";
    void *iter = nullptr;
    while (auto method = Variables::IL2CPP::il2cpp_class_get_methods(klass, &iter)) {
        auto methodPointer = *(void **)method;
        if (methodPointer) {
            outPut << "\t// RVA: 0x" << std::hex << std::uppercase << (uint64_t)methodPointer - Variables::info.address;
        } else {
            outPut << "\t// RVA: -1";
        }
        outPut << "\n\t";
        uint32_t iflags = 0;
        auto flags = Variables::IL2CPP::il2cpp_method_get_flags(method, &iflags);
        outPut << getMethodModifier(flags);
        auto return_type = Variables::IL2CPP::il2cpp_method_get_return_type(method);
        if (Variables::IL2CPP::il2cpp_type_is_byref(return_type)) {
            outPut << "ref ";
        }
        auto return_class = Variables::IL2CPP::il2cpp_class_from_type(return_type);
        outPut << getClassName(return_class) << " " << Variables::IL2CPP::il2cpp_method_get_name(method)
               << "(";
        auto param_count = Variables::IL2CPP::il2cpp_method_get_param_count(method);
        for (int i = 0; i < param_count; ++i) {
            auto param = Variables::IL2CPP::il2cpp_method_get_param(method, i);
            auto attrs = Variables::IL2CPP::il2cpp_type_get_attrs(param);
            if (Variables::IL2CPP::il2cpp_type_is_byref(param)) {
                if (attrs & PARAM_ATTRIBUTE_OUT && !(attrs & PARAM_ATTRIBUTE_IN)) {
                    outPut << "out ";
                } else if (attrs & PARAM_ATTRIBUTE_IN && !(attrs & PARAM_ATTRIBUTE_OUT)) {
                    outPut << "in ";
                } else {
                    outPut << "ref ";
                }
            } else {
                if (attrs & PARAM_ATTRIBUTE_IN) {
                    outPut << "[In] ";
                }
                if (attrs & PARAM_ATTRIBUTE_OUT) {
                    outPut << "[Out] ";
                }
            }
            auto parameter_class = Variables::IL2CPP::il2cpp_class_from_type(param);
            outPut << getClassName(parameter_class) << " "
                   << Variables::IL2CPP::il2cpp_method_get_param_name(method, i);
            outPut << ", ";
        }
        if (param_count > 0) {
            outPut.seekp(-2, outPut.cur);
        }
        outPut << ") { }\n";
        // TODO GenericInstMethod: lmao too lazy to implement
    }
    if (outPut.str().length() == 13) return "";  // no methods
    return outPut.str();
}

std::string Dumper::getMethodModifier(uint32_t flags) {
    std::stringstream outPut;
    auto access = flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
    switch (access) {
        case METHOD_ATTRIBUTE_PRIVATE:
            outPut << "private ";
            break;
        case METHOD_ATTRIBUTE_PUBLIC:
            outPut << "public ";
            break;
        case METHOD_ATTRIBUTE_FAMILY:
            outPut << "protected ";
            break;
        case METHOD_ATTRIBUTE_ASSEM:
        case METHOD_ATTRIBUTE_FAM_AND_ASSEM:
            outPut << "internal ";
            break;
        case METHOD_ATTRIBUTE_FAM_OR_ASSEM:
            outPut << "protected internal ";
            break;
    }
    if (flags & METHOD_ATTRIBUTE_STATIC) {
        outPut << "static ";
    }
    if (flags & METHOD_ATTRIBUTE_ABSTRACT) {
        outPut << "abstract ";
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_FINAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "sealed override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_VIRTUAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_NEW_SLOT) {
            outPut << "virtual ";
        } else {
            outPut << "override ";
        }
    }
    if (flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
        outPut << "extern ";
    }
    return outPut.str();
}

std::string Dumper::getClassName(void *klass) {
    std::stringstream outPut;
    const char *klassName = Variables::IL2CPP::il2cpp_class_get_name(klass);
    if (strchr(klassName, '`')) {
        int numArgs = klassName[strlen(klassName) - 1] - '0';
        char *name = new char[strlen(klassName)];
        strncpy(name, klassName, strlen(klassName) - 2);
        name[strlen(klassName) - 2] = '\0';
        outPut << name << "<";
        for (int i = 0; i < numArgs - 1; i++) {
            outPut << "T" << i << ", ";
        }
        outPut << "T" << numArgs - 1 << ">";
    } else {
        outPut << klassName;
    }
    return outPut.str();
}

void Dumper::Log(const char *fmt, ...) {
#ifdef DEBUG
    File logfile(dumpDir + "/logs.txt", "a");
    if (!logfile.ok()) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(logfile, fmt, args);
    fprintf(logfile, "\n");
    va_end(args);
    logfile.close();
#endif
}