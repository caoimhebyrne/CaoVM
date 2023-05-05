/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Attribute.h"
#include "ClassParser.h"
#include "ConstantPool.h"
#include <AK/BitStream.h>

namespace Parser {

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.2
ConstantValueAttribute::ConstantValueAttribute(u16 value_index)
    : Attribute(AttributeType::ConstantValue)
    , m_value_index(value_index)
{
}

ErrorOr<NonnullRefPtr<ConstantValueAttribute>> ConstantValueAttribute::parse(ClassParser& class_parser)
{
    // The constant_pool entry at this index gives the value represented by this attribute
    auto constant_value_index = TRY(class_parser.read_u2());
    return try_make_ref_counted<ConstantValueAttribute>(constant_value_index);
}

ErrorOr<String> ConstantValueAttribute::debug_description()
{
    StringBuilder builder;

    builder.append("ConstantValue { "sv);
    builder.appendff("value_index = {}", value_index());
    builder.append(" }"sv);

    return builder.to_string();
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.3
CodeAttribute::CodeAttribute(u16 max_stack, u16 max_locals, ByteBuffer code, Vector<NonnullRefPtr<Attribute>> attributes)
    : Attribute(AttributeType::Code)
    , m_max_stack(move(max_stack))
    , m_max_locals(move(max_locals))
    , m_code(move(code))
    , m_attributes(move(attributes))
{
}

ErrorOr<NonnullRefPtr<CodeAttribute>> CodeAttribute::parse(ClassParser& class_parser, NonnullRefPtr<ConstantPool> const& constant_pool)
{
    auto max_stack = TRY(class_parser.read_u2());
    auto max_locals = TRY(class_parser.read_u2());
    auto code_length = TRY(class_parser.read_u4());

    // The value of code_length must be greater than zero (as the code array must not be empty) and less than 65536.
    VERIFY(code_length > 0 && code_length < 65536);

    // The code array gives the actual bytes of Java Virtual Machine code that implement the method.
    auto code = TRY(ByteBuffer::create_uninitialized(code_length));
    TRY(class_parser.stream()->read_some(code));

    // TODO: Implement exception tables
    auto exception_table_length = TRY(class_parser.read_u2());
    TRY(class_parser.stream()->discard(exception_table_length * 64));

    auto attributes_count = TRY(class_parser.read_u2());
    auto attributes = Vector<NonnullRefPtr<Attribute>>();
    for (auto i = 0; i < attributes_count; i++) {
        auto attribute = TRY(class_parser.parse_attribute(constant_pool));
        attributes.append(move(attribute));
    }

    return try_make_ref_counted<CodeAttribute>(max_stack, max_locals, code, move(attributes));
}

ErrorOr<String> CodeAttribute::debug_description()
{
    StringBuilder builder;

    builder.append("Code { "sv);
    builder.appendff("max_stack = {}, ", max_stack());
    builder.appendff("max_locals = {}, ", max_locals());
    builder.append("attributes = [ "sv);

    for (auto const& attribute : attributes()) {
        builder.appendff("{}", TRY(attribute->debug_description()));
    }

    builder.append(" ], "sv);
    builder.append(" }"sv);

    return builder.to_string();
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.12
LineNumberTableAttribute::LineNumberTableAttribute(Vector<LineNumberTableAttribute::Entry> table)
    : Attribute(AttributeType::LineNumberTable)
    , m_table(move(table))
{
}

ErrorOr<NonnullRefPtr<LineNumberTableAttribute>> LineNumberTableAttribute::parse(ClassParser& class_parser)
{
    auto line_number_table_length = TRY(class_parser.read_u2());
    auto table = Vector<LineNumberTableAttribute::Entry>();

    for (auto i = 0; i < line_number_table_length; i++) {
        auto entry = TRY(LineNumberTableAttribute::parse_entry(class_parser));
        table.append(entry);
    }

    return try_make_ref_counted<LineNumberTableAttribute>(table);
}

ErrorOr<LineNumberTableAttribute::Entry> LineNumberTableAttribute::parse_entry(ClassParser& class_parser)
{
    // The start_pc indicates the index into the code array at which the code for a new line in the original source file begins.
    auto start_pc = TRY(class_parser.read_u2());

    // The value of the line_number item gives the corresponding line number in the original source file.
    auto line_number = TRY(class_parser.read_u2());

    return Entry {
        .start_pc = start_pc,
        .line_number = line_number
    };
}

ErrorOr<String> LineNumberTableAttribute::debug_description()
{
    StringBuilder builder;

    builder.append("LineNumberTableAttribute { "sv);

    size_t index = 0;
    for (auto const& entry : table()) {
        index++;

        builder.append("{ "sv);
        builder.appendff("pc = {}, ln = {}", entry.start_pc, entry.line_number);
        builder.append(" }"sv);

        if (table().size() != index) {
            builder.append(',');
        }

        builder.append(" "sv);
    }

    builder.append("}"sv);

    return builder.to_string();
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.10
SourceFileAttribute::SourceFileAttribute(u16 index)
    : Attribute(AttributeType::SourceFile)
    , m_index(move(index))
{
}

ErrorOr<NonnullRefPtr<SourceFileAttribute>> SourceFileAttribute::parse(ClassParser& class_parser)
{
    // The string referenced by the sourcefile_index item will contain the name of the source file from which this class file was compiled.
    auto sourcefile_index = TRY(class_parser.read_u2());
    return try_make_ref_counted<SourceFileAttribute>(sourcefile_index);
}

ErrorOr<String> SourceFileAttribute::debug_description()
{
    StringBuilder builder;

    builder.append("SourceFile { "sv);
    builder.appendff("index = {}", index());
    builder.append(" }"sv);

    return builder.to_string();
}

}
