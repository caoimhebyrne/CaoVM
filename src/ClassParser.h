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

private:
    NonnullOwnPtr<BigEndianInputBitStream> m_stream;
};
