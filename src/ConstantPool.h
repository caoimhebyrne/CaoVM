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

// Forward-declaration
class ConstantInfo;

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4
class ConstantPool {
public:
    // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4-140
    enum Tag : u8 {
        // Raw UTF8 data
        UTF8 = 1,

        // Used to represent a class or an interface
        Class = 7,

        // Used to represent constant objects of the type String
        String = 8,

        // Used to represent a reference to a field on an Object
        FieldReference = 9,

        // Used to represent a reference to a method on an Object
        MethodReference = 10,

        // Used to represent a field or a method, without indicating which class or interface it belongs to
        NameAndType = 12,
    };

    ConstantPool(Vector<NonnullOwnPtr<ConstantInfo>> entries)
        : m_entries(move(entries))
    {
    }

    static ErrorOr<NonnullOwnPtr<ConstantPool>> parse(u16 size, NonnullOwnPtr<BigEndianInputBitStream>& stream);

    Vector<NonnullOwnPtr<ConstantInfo>> const& entries() { return m_entries; };

private:
    Vector<NonnullOwnPtr<ConstantInfo>> m_entries;
};

namespace AK {

template<>
struct Formatter<ConstantPool::Tag> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& builder, ConstantPool::Tag const& tag)
    {
        switch (tag) {
        case ConstantPool::Tag::UTF8: {
            return Formatter<StringView>::format(builder, "UTF8"sv);
        }

        case ConstantPool::Tag::Class: {
            return Formatter<StringView>::format(builder, "Class"sv);
        }

        case ConstantPool::Tag::String: {
            return Formatter<StringView>::format(builder, "String"sv);
        }

        case ConstantPool::Tag::FieldReference: {
            return Formatter<StringView>::format(builder, "FieldReference"sv);
        }

        case ConstantPool::Tag::MethodReference: {
            return Formatter<StringView>::format(builder, "MethodReference"sv);
        }

        case ConstantPool::Tag::NameAndType: {
            return Formatter<StringView>::format(builder, "NameAndType"sv);
        }

        default: {
            TODO();
            break;
        }
        }
    }
};

}

// Every constant in the constant_pool_table has information associated with it
class ConstantInfo {
public:
    ConstantPool::Tag const& tag() { return m_tag; };

protected:
    ConstantInfo(ConstantPool::Tag tag)
        : m_tag(tag)
    {
    }

private:
    ConstantPool::Tag m_tag;
};
