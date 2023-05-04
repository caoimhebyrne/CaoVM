/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/String.h>

namespace Parser {

// Forward-declaration
class ClassParser;
class ConstantPool;

enum class AttributeType {
    // A ConstantValue attribute represents the value of a constant expression
    ConstantValue,

    // A Code attribute contains the Java Virtual Machine instructions and auxiliary information for a method
    Code,

    // The LineNumberTable attribute is an optional variable-length attribute in the attributes table of a Code attribute.
    // It may be used by debuggers to determine which part of the code array corresponds to a given line number in the original source file.
    LineNumberTable,

    // The SourceFile attribute is an optional fixed-length attribute in the attributes table of a ClassFile structure (§4.1).
    // It provides an index into the constant pool table, denoting the name of the original source file of this class.
    SourceFile
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

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.12
class LineNumberTableAttribute : public Attribute {
public:
    struct Entry {
        // The item indicates the index into the code array at which the code for a new line in the original source file begins.
        u16 start_pc;

        // The value of the line_number item gives the corresponding line number in the original source file.
        u16 line_number;
    };

    LineNumberTableAttribute(Vector<Entry> table);

    static ErrorOr<NonnullOwnPtr<LineNumberTableAttribute>> parse(ClassParser& class_parser);

    ErrorOr<String> debug_description();

    Vector<Entry> const& table() { return m_table; };

private:
    Vector<Entry> m_table;

    static ErrorOr<Entry> parse_entry(ClassParser& class_parser);
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.10
class SourceFileAttribute : public Attribute {
public:
    SourceFileAttribute(u16 index);

    static ErrorOr<NonnullOwnPtr<SourceFileAttribute>> parse(ClassParser& class_parser);

    ErrorOr<String> debug_description();

    u16 index() { return m_index; };

private:
    u16 m_index;
};

}

namespace AK {
template<>
struct Formatter<Parser::Attribute> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& builder, Parser::Attribute& attribute_info)
    {
        return Formatter<StringView>::format(builder, TRY(attribute_info.debug_description()));
    }
};

}
