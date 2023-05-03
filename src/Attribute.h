/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/String.h>

// Forward-declaration
class ClassParser;
class ConstantPool;

enum class AttributeType {
    // A ConstantValue attribute represents the value of a constant expression
    ConstantValue,

    // A Code attribute contains the Java Virtual Machine instructions and auxiliary information for a method
    Code,
};

class Attribute {
public:
    virtual ~Attribute() = default;
    virtual ErrorOr<String> debug_description() = 0;

    AttributeType const& type() { return m_type; };

protected:
    Attribute(AttributeType type)
        : m_type(move(type))
    {
    }

private:
    AttributeType m_type;
};

namespace AK {
template<>
struct Formatter<Attribute> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& builder, Attribute& attribute_info)
    {
        return Formatter<StringView>::format(builder, TRY(attribute_info.debug_description()));
    }
};

}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.2
class ConstantValueAttribute : public Attribute {
public:
    ConstantValueAttribute(u16 value_index);

    static ErrorOr<NonnullOwnPtr<ConstantValueAttribute>> parse(ClassParser& class_parser);

    ErrorOr<String> debug_description();

    u16 value_index() { return m_value_index; };

private:
    u16 m_value_index;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.3
class CodeAttribute : public Attribute {
public:
    CodeAttribute(u16 max_stack, u16 max_locals, ByteBuffer code, Vector<NonnullOwnPtr<Attribute>> attributes);

    static ErrorOr<NonnullOwnPtr<CodeAttribute>> parse(ClassParser& class_parser, NonnullOwnPtr<ConstantPool> const& constant_pool);

    ErrorOr<String> debug_description();

    u16 max_stack() { return m_max_stack; };
    u16 max_locals() { return m_max_locals; };
    ByteBuffer const& code() { return m_code; };
    Vector<NonnullOwnPtr<Attribute>> const& attributes() { return m_attributes; };

private:
    u16 m_max_stack;
    u16 m_max_locals;
    ByteBuffer m_code;
    Vector<NonnullOwnPtr<Attribute>> m_attributes;
};
