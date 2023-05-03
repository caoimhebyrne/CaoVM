/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantInfo.h"
#include "ClassParser.h"
#include <AK/BitStream.h>
#include <AK/NonnullOwnPtr.h>

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.7
ConstantUTF8Info::ConstantUTF8Info(String data)
    : ConstantInfo(ConstantPool::Tag::UTF8)
    , m_data(data)
{
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.1
ConstantClassInfo::ConstantClassInfo(u16 name_index)
    : ConstantInfo(ConstantPool::Tag::Class)
    , m_name_index(move(name_index))
{
}

// // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.2
ConstantMemberReferenceInfo::ConstantMemberReferenceInfo(ConstantPool::Tag tag, u16 class_index, u16 name_and_type_index)
    : ConstantInfo(tag)
    , m_class_index(move(class_index))
    , m_name_and_type_index(move(name_and_type_index))
{
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.3
ConstantStringInfo::ConstantStringInfo(u16 string_index)
    : ConstantInfo(ConstantPool::Tag::String)
    , m_index(string_index)
{
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.4
ConstantIntegerInfo::ConstantIntegerInfo(u32 value)
    : ConstantInfo(ConstantPool::Tag::Integer)
    , m_value(value)
{
}

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.6
ConstantNameAndTypeInfo::ConstantNameAndTypeInfo(u16 name_index, u16 descriptor_index)
    : ConstantInfo(ConstantPool::Tag::NameAndType)
    , m_name_index(move(name_index))
    , m_descriptor_index(move(descriptor_index))
{
}

ErrorOr<NonnullOwnPtr<ConstantUTF8Info>> ConstantUTF8Info::parse(ClassParser& class_parser)
{
    // The value of the length item gives the number of bytes in the bytes array (not the length of the resulting string).
    auto length = TRY(class_parser.read_u2());

    // The bytes array contains the bytes of the string.
    // FIXME: String content is encoded in modified UTF-8.
    auto buffer = TRY(ByteBuffer::create_uninitialized(length));
    TRY(class_parser.stream()->read_until_filled(buffer));

    // Convert the bytest to a UTF-8 String
    auto string = TRY(String::from_utf8(buffer));
    return try_make<ConstantUTF8Info>(string);
}

ErrorOr<NonnullOwnPtr<ConstantClassInfo>> ConstantClassInfo::parse(ClassParser& class_parser)
{
    // u2 name_index;
    auto name_index = TRY(class_parser.read_u2());
    return try_make<ConstantClassInfo>(name_index);
}

ErrorOr<NonnullOwnPtr<ConstantMemberReferenceInfo>> ConstantMemberReferenceInfo::parse(ConstantPool::Tag tag, ClassParser& class_parser)
{
    // u2 class_index;
    auto class_index = TRY(class_parser.read_u2());

    // u2 name_and_type_index;
    auto name_and_type_index = TRY(class_parser.read_u2());

    return try_make<ConstantMemberReferenceInfo>(tag, class_index, name_and_type_index);
}

ErrorOr<NonnullOwnPtr<ConstantStringInfo>> ConstantStringInfo::parse(ClassParser& class_parser)
{
    // u2 string_index;
    auto string_index = TRY(class_parser.read_u2());
    return try_make<ConstantStringInfo>(string_index);
}

ErrorOr<NonnullOwnPtr<ConstantIntegerInfo>> ConstantIntegerInfo::parse(ClassParser& class_parser)
{
    // u4 bytes;
    auto value = TRY(class_parser.read_u4());
    return try_make<ConstantIntegerInfo>(value);
}

ErrorOr<NonnullOwnPtr<ConstantNameAndTypeInfo>> ConstantNameAndTypeInfo::parse(ClassParser& class_parser)
{
    // u2 name_index;
    auto name_index = TRY(class_parser.read_u2());

    // u2 descriptor_index;
    auto descriptor_index = TRY(class_parser.read_u2());

    return try_make<ConstantNameAndTypeInfo>(name_index, descriptor_index);
}

ErrorOr<String> ConstantUTF8Info::debug_description()
{
    StringBuilder builder;

    builder.append("UTF8 { "sv);
    builder.appendff("\"{}\"", data());
    builder.append(" }"sv);

    return builder.to_string();
}

ErrorOr<String> ConstantClassInfo::debug_description()
{
    StringBuilder builder;

    builder.append("Class { "sv);
    builder.appendff("name_index = {}", name_index());
    builder.append(" }"sv);

    return builder.to_string();
}

ErrorOr<String> ConstantMemberReferenceInfo::debug_description()
{
    StringBuilder builder;

    builder.append("MemberReference { "sv);
    builder.appendff("class_index = {}, ", class_index());
    builder.appendff("name_and_type_index = {}", name_and_type_index());
    builder.append(" }"sv);

    return builder.to_string();
}

ErrorOr<String> ConstantStringInfo::debug_description()
{
    StringBuilder builder;

    builder.append("String { "sv);
    builder.appendff("index = {}", index());
    builder.append(" }"sv);

    return builder.to_string();
}

ErrorOr<String> ConstantIntegerInfo::debug_description()
{
    StringBuilder builder;

    builder.append("Integer { "sv);
    builder.appendff("{}", value());
    builder.append(" }"sv);

    return builder.to_string();
}

ErrorOr<String> ConstantNameAndTypeInfo::debug_description()
{
    StringBuilder builder;

    builder.append("NameAndType { "sv);
    builder.appendff("name_index = {}, ", name_index());
    builder.appendff("descriptor_index = {}", descriptor_index());
    builder.append(" }"sv);

    return builder.to_string();
}
