/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ClassFile.h"
#include <AK/NonnullOwnPtr.h>
#include <LibCore/File.h>

class ClassParser {
public:
    ClassParser(NonnullOwnPtr<BigEndianInputBitStream> stream);

    static ErrorOr<NonnullOwnPtr<ClassParser>> create(NonnullOwnPtr<Core::File> stream);

    ErrorOr<ClassFile> parse();

    // The JVM spec defines a few data types for unsigned integers, werid naming but sure...
    ErrorOr<u8> read_u1();
    ErrorOr<u16> read_u2();
    ErrorOr<u32> read_u4();

    NonnullOwnPtr<BigEndianInputBitStream> const& stream() { return m_stream; };

private:
    ErrorOr<ConstantClassInfo> parse_interface(NonnullOwnPtr<ConstantPool> const& constant_pool);
    ErrorOr<NonnullOwnPtr<FieldInfo>> parse_field(NonnullOwnPtr<ConstantPool> const& constant_pool);
    ErrorOr<NonnullOwnPtr<Attribute>> parse_attribute(NonnullOwnPtr<ConstantPool> const& constant_pool);

    NonnullOwnPtr<BigEndianInputBitStream> m_stream;
};
