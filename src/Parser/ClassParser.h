/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ClassFile.h"
#include <AK/NonnullOwnPtr.h>
#include <LibCore/File.h>

namespace Parser {

class ClassParser {
public:
    ClassParser(NonnullOwnPtr<BigEndianInputBitStream> stream);

    static ErrorOr<NonnullOwnPtr<ClassParser>> create(NonnullOwnPtr<Core::File> stream);

    ErrorOr<ClassFile> parse();
    ErrorOr<NonnullRefPtr<Attribute>> parse_attribute(NonnullRefPtr<ConstantPool> const& constant_pool);

    // The JVM spec defines a few data types for unsigned integers, werid naming but sure...
    ErrorOr<u8> read_u1();
    ErrorOr<u16> read_u2();
    ErrorOr<u32> read_u4();

    NonnullOwnPtr<BigEndianInputBitStream> const& stream() { return m_stream; };

private:
    ErrorOr<NonnullRefPtr<ConstantClassInfo>> parse_interface(NonnullRefPtr<ConstantPool> const& constant_pool);
    ErrorOr<NonnullOwnPtr<FieldInfo>> parse_field(NonnullRefPtr<ConstantPool> const& constant_pool);
    ErrorOr<NonnullOwnPtr<MethodInfo>> parse_method(NonnullRefPtr<ConstantPool> const& constant_pool);

    NonnullOwnPtr<BigEndianInputBitStream> m_stream;
};

}
