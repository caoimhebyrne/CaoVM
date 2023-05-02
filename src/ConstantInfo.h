/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ConstantPool.h"
#include <AK/String.h>

class ConstantInfo {
public:
    virtual ~ConstantInfo() = default;

    ConstantPool::Tag const& tag() { return m_tag; };

    virtual ErrorOr<String> debug_description() = 0;

protected:
    ConstantInfo(ConstantPool::Tag tag)
        : m_tag(tag)
    {
    }

private:
    ConstantPool::Tag m_tag;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.7
class ConstantUTF8Info : public ConstantInfo {
public:
    ConstantUTF8Info(String data);

    static ErrorOr<NonnullOwnPtr<ConstantUTF8Info>> parse(NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    String const& data() { return m_data; };

private:
    String m_data;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.1
class ConstantClassInfo : public ConstantInfo {
public:
    ConstantClassInfo(u16 name_index);

    static ErrorOr<NonnullOwnPtr<ConstantClassInfo>> parse(NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    u16 name_index() { return m_name_index; };

private:
    // The value of the name_index item must be a valid index into the constant_pool table.
    u16 m_name_index;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.2
class ConstantMemberReferenceInfo : public ConstantInfo {
public:
    ConstantMemberReferenceInfo(ConstantPool::Tag tag, u16 name_index, u16 descriptor_index);

    static ErrorOr<NonnullOwnPtr<ConstantMemberReferenceInfo>> parse(ConstantPool::Tag tag, NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    u16 class_index() { return m_class_index; };
    u16 name_and_type_index() { return m_name_and_type_index; };

private:
    // The constant_pool entry at that index must be a CONSTANT_Class_info structure (§4.4.1) representing a class or interface type that has the field or method as a member.
    u16 m_class_index;

    // The constant_pool entry at that index must be a CONSTANT_NameAndType_info structure (§4.4.6).
    u16 m_name_and_type_index;
};

class ConstantFieldReferenceInfo : public ConstantMemberReferenceInfo {
};

class ConstantMethodReferenceInfo : public ConstantMemberReferenceInfo {
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.3
class ConstantStringInfo : public ConstantInfo {
public:
    ConstantStringInfo(u16 string_index);

    static ErrorOr<NonnullOwnPtr<ConstantStringInfo>> parse(NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    u16 index() { return m_index; };

private:
    u16 m_index;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.4
class ConstantIntegerInfo : public ConstantInfo {
public:
    ConstantIntegerInfo(u32 value);

    static ErrorOr<NonnullOwnPtr<ConstantIntegerInfo>> parse(NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    u32 value() { return m_value; };

private:
    u32 m_value;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4.6
class ConstantNameAndTypeInfo : public ConstantInfo {
public:
    ConstantNameAndTypeInfo(u16 name_index, u16 descriptor_index);

    static ErrorOr<NonnullOwnPtr<ConstantNameAndTypeInfo>> parse(NonnullOwnPtr<BigEndianInputBitStream>& stream);

    ErrorOr<String> debug_description();

    u16 name_index() { return m_name_index; };
    u16 descriptor_index() { return m_descriptor_index; };

private:
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (§4.4.7) representing either a valid unqualified name denoting a field or method (§4.2.2), or the special method name <init> (§2.9.1).
    u16 m_name_index;

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (§4.4.7) representing a valid field descriptor or method descriptor (§4.3.2, §4.3.3).
    u16 m_descriptor_index;
};
