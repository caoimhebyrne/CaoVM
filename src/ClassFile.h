/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantPool.h"
#include <AK/Format.h>
#include <AK/Vector.h>

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.1-200-B.2
// FIXME: Use this
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
};

// Used for debug formatting
namespace AK {

template<>
struct Formatter<ClassFile> : Formatter<FormatString> {
    ErrorOr<void> format(FormatBuilder& builder, ClassFile const& class_file)
    {
        return Formatter<FormatString>::format(builder,
            "Classfile {{ magic=0x{:02X}, minor_version={}, major_version={}, constant_pool_count={}, access_flags={}, this_class={}, super_class={} }}"sv,
            class_file.magic,
            class_file.minor_version,
            class_file.major_version,
            class_file.constant_pool_count,
            class_file.access_flags,
            class_file.this_class,
            class_file.super_class);
    }
};

}
