/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "../Parser/ConstantPool.h"
#include "SymbolicatedReference.h"
#include <AK/Forward.h>
#include <AK/HashMap.h>

namespace Interpreter {

// The symbolicated constant pool takes a parsed constant pool, and
// symbolicates its entries.
//
// See the JVM spec for more information on how to symbolicate entries:
// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-5.html#jvms-5.1
class SymbolicatedConstantPool : public RefCounted<SymbolicatedConstantPool> {
public:
    SymbolicatedConstantPool(NonnullRefPtr<Parser::ConstantPool> parsed_pool);
    static NonnullRefPtr<SymbolicatedConstantPool> create(NonnullRefPtr<Parser::ConstantPool> parsed_pool);

    // Iterates through the entries found in the constant pool and symbolicates them
    ErrorOr<void> symbolicate();

    // Returns a reference to the non-symbolicated constant pool
    NonnullRefPtr<Parser::ConstantPool> parsed_pool() { return m_parsed_pool; };

    HashMap<u16, NonnullRefPtr<SymbolicatedReference>>& entries() { return m_entries; };

    // Attempts to retreive a symbolicated class reference, or creates if it hasn't been symbolicated yet
    ErrorOr<NonnullRefPtr<SymbolicatedClassReference>> get_or_symbolicate_class(u16 index);

private:
    // The non-symbolicated constant pool
    NonnullRefPtr<Parser::ConstantPool> m_parsed_pool;

    HashMap<u16, NonnullRefPtr<SymbolicatedReference>> m_entries;
};

}
