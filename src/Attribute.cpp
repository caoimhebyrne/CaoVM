/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Attribute.h"
#include "ClassParser.h"

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.7.2
ConstantValueAttributeInfo::ConstantValueAttributeInfo(u16 value_index)
    : AttributeInfo(AttributeType::ConstantValue)
    , m_value_index(value_index)
{
}

ErrorOr<NonnullOwnPtr<ConstantValueAttributeInfo>> ConstantValueAttributeInfo::parse(ClassParser& class_parser)
{
    // The constant_pool entry at this index gives the value represented by this attribute
    auto constant_value_index = TRY(class_parser.read_u2());
    return try_make<ConstantValueAttributeInfo>(constant_value_index);
}

ErrorOr<String> ConstantValueAttributeInfo::debug_description()
{
    StringBuilder builder;

    builder.append("ConstantValueAttributeInfo { "sv);
    builder.appendff("value_index = {}", value_index());
    builder.append(" }"sv);

    return builder.to_string();
}
