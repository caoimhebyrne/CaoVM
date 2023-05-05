/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/Forward.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/Types.h>
#include <AK/Vector.h>

namespace Parser {

// Forward-declaration
class ConstantInfo;
class ClassParser;

class ConstantMethodReferenceInfo;
class ConstantNameAndTypeInfo;
class ConstantUTF8Info;
class ConstantClassInfo;
class ConstantFieldReferenceInfo;

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4
class ConstantPool {
public:
    ConstantPool(Vector<NonnullRefPtr<ConstantInfo>> entries)
        : m_entries(move(entries))
    {
    }

    static ErrorOr<NonnullOwnPtr<ConstantPool>> parse(u16 size, ClassParser& class_parser);

    Vector<NonnullRefPtr<ConstantInfo>> const& entries() { return m_entries; };

    // Attempts to read a method reference from the constant pool
    ErrorOr<NonnullRefPtr<ConstantMethodReferenceInfo>> method_reference_at(u16 index);

    // Attempts to read a field reference from the constant pool
    ErrorOr<NonnullRefPtr<ConstantFieldReferenceInfo>> field_reference_at(u16 index);

    // Attempts to read a name and type from the constant pool
    ErrorOr<NonnullRefPtr<ConstantNameAndTypeInfo>> name_and_type_at(u16 index);

    // Attempts to read a utf8 constant from the constant pool
    ErrorOr<NonnullRefPtr<ConstantUTF8Info>> utf8_at(u16 index);

    // Attempts to read a class' information from the constant pool
    ErrorOr<NonnullRefPtr<ConstantClassInfo>> class_at(u16 index);

private:
    Vector<NonnullRefPtr<ConstantInfo>> m_entries;
};

}
