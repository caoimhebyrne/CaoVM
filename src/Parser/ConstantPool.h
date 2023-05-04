/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/Forward.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/Types.h>
#include <AK/Vector.h>

namespace Parser {

// Forward-declaration
class ConstantInfo;
class ClassParser;

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4
class ConstantPool {
public:
    ConstantPool(Vector<NonnullOwnPtr<ConstantInfo>> entries)
        : m_entries(move(entries))
    {
    }

    static ErrorOr<NonnullOwnPtr<ConstantPool>> parse(u16 size, ClassParser& class_parser);

    Vector<NonnullOwnPtr<ConstantInfo>> const& entries() { return m_entries; };

private:
    Vector<NonnullOwnPtr<ConstantInfo>> m_entries;
};

}
