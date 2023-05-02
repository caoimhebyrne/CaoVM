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

private:
    NonnullOwnPtr<BigEndianInputBitStream> m_stream;
};
