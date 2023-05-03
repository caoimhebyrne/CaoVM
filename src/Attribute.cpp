/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Attribute.h"
#include "ClassParser.h"
#include "ConstantPool.h"
#include <AK/BitStream.h>

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.2
ConstantValueAttribute::ConstantValueAttribute(u16 value_index)
    : Attribute(AttributeType::ConstantValue)
    , m_value_index(value_index)
{
}

ErrorOr<NonnullOwnPtr<ConstantValueAttribute>> ConstantValueAttribute::parse(ClassParser& class_parser)
{
    // The constant_pool entry at this index gives the value represented by this attribute
    auto constant_value_index = TRY(class_parser.read_u2());
    return try_make<ConstantValueAttribute>(constant_value_index);
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
CodeAttribute::CodeAttribute(u16 max_stack, u16 max_locals, ByteBuffer code, Vector<NonnullOwnPtr<Attribute>> attributes)
    : Attribute(AttributeType::Code)
    , m_max_stack(move(max_stack))
    , m_max_locals(move(max_locals))
    , m_code(move(code))
    , m_attributes(move(attributes))
{
}

ErrorOr<NonnullOwnPtr<CodeAttribute>> CodeAttribute::parse(ClassParser& class_parser, NonnullOwnPtr<ConstantPool> const& constant_pool)
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
    auto attributes = Vector<NonnullOwnPtr<Attribute>>();
    for (auto i = 0; i < attributes_count; i++) {
        auto attribute = TRY(class_parser.parse_attribute(constant_pool));
        attributes.append(move(attribute));
    }

    return try_make<CodeAttribute>(max_stack, max_locals, code, move(attributes));
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
