/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ConstantInfo.h"
#include "ConstantPool.h"
#include <AK/Format.h>
#include <AK/StringBuilder.h>
#include <AK/Vector.h>

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.1-200-B.2
enum MajorVersion : u16 {
    V1_1 = 45,
    V1_2 = 46,
    V1_3 = 47,
    V1_4 = 48,
    V5 = 49,
    V6 = 50,
    V7 = 51,
    V8 = 52,
    V9 = 43,
    V10 = 54,
    V11 = 55,
    V12 = 56,
    V13 = 57,
    V14 = 58,
    V15 = 59,
    V16 = 60,
    V17 = 61
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7
struct AttributeInfo {
    u16 name_index;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.5
struct FieldInfo {
    // The value of the access_flags item is a mask of flags used to denote access permission to and properties of this field.
    u16 access_flags;

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (§4.4.7) which represents a valid unqualified name denoting a field (§4.2.2).
    u16 name_index;

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (§4.4.7) which represents a valid field descriptor (§4.3.2).
    u16 descriptor_index;

    //  A field can have any number of optional attributes associated with it.
    Vector<AttributeInfo> attributes;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html
struct ClassFile {
    // The magic item supplies the magic number identifying the class file format; it has the value 0xCAFEBABE.
    u32 magic;

    // The values of the minor_version and major_version items are the minor and major version numbers of this class file.
    // Together, a major and a minor version number determine the version of the class file format.
    // If a class file has major version number M and minor version number m, we denote the version of its class file format as M.m.
    u16 minor_version;
    u16 major_version;

    // The value of the constant_pool_count item is equal to the number of entries in the constant_pool table plus one.
    u16 constant_pool_count;
    NonnullOwnPtr<ConstantPool> constant_pool;

    // A mask of flags used to denote access permissions to and properties of this class or interface
    u16 access_flags;

    // An index into the constant pool table, the value at that index be a CONSTANT_Class_info structure.
    u16 this_class;

    // Must be 0, or a valid index into the constant pool table. If it's nonzero, the value at that index be a CONSTANT_Class_info structure.
    // If 0, then this class file must represent the class Object, the only class or interface without a direct superclass.
    u16 super_class;

    // Each value in the interfaces array must be a valid index into the constant_pool table.
    //
    // The constant_pool entry at each value of interfaces[i], where 0 ≤ i < interfaces_count, must be a CONSTANT_Class_info structure
    // representing an interface that is a direct superinterface of this class or interface type, in the left-to-right order given in the source for the type.
    Vector<ConstantClassInfo> interfaces;

    // Each value in the fields table must be a field_info structure (§4.5) giving a complete description of a field in this class or interface.
    //
    // The fields table includes only those fields that are declared by this class or interface.
    // It does not include items representing fields that are inherited from superclasses or superinterfaces.
    Vector<FieldInfo> fields;
};

// Used for debug formatting
namespace AK {

template<>
struct Formatter<ClassFile> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& format_builder, ClassFile const& class_file)
    {
        StringBuilder builder;
        builder.append("ClassFile {\n"sv);

        builder.appendff("  magic=0x{:02X}\n", class_file.magic);
        builder.appendff("  major_version={}\n", class_file.major_version);
        builder.appendff("  minor_version={}\n", class_file.minor_version);
        builder.appendff("  constant_pool_count={}\n", class_file.constant_pool_count);
        builder.appendff("  access_flags={}\n", class_file.access_flags);
        builder.appendff("  this_class={}\n", class_file.this_class);
        builder.appendff("  super_class={}\n", class_file.super_class);
        // builder.appendff("  interfaces={}\n", class_file.interfaces);

        // The default foratting for Vector<T> annoyed me!
        builder.appendff("  fields=[\n", class_file.fields);
        for (auto& field : class_file.fields) {
            builder.appendff("    {}\n", field);
        }
        builder.appendff("  ]\n", class_file.fields);

        builder.append('}');
        return Formatter<StringView>::format(format_builder, builder.string_view());
    }
};

template<>
struct Formatter<FieldInfo> : Formatter<FormatString> {
    ErrorOr<void> format(FormatBuilder& builder, FieldInfo const& field_info)
    {
        return Formatter<FormatString>::format(builder,
            "FieldInfo {{ access_flags={}, name_index={}, descriptor_index={} }}"sv,
            field_info.access_flags,
            field_info.name_index,
            field_info.descriptor_index);
    }
};

}
