/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "SymbolicatedConstantPool.h"
#include "../Parser/ConstantInfo.h"
#include <AK/NonnullRefPtr.h>

namespace Interpreter {

SymbolicatedConstantPool::SymbolicatedConstantPool(NonnullOwnPtr<Parser::ConstantPool> parsed_pool)
    : m_parsed_pool(move(parsed_pool))
{
}

NonnullRefPtr<SymbolicatedConstantPool> SymbolicatedConstantPool::create(NonnullOwnPtr<Parser::ConstantPool> parsed_pool)
{
    // FIXME: Verify some stuff about the constant pool before continuing
    return make_ref_counted<SymbolicatedConstantPool>(move(parsed_pool));
}

// Iterates through the entries found in the constant pool and symbolicates them
ErrorOr<void> SymbolicatedConstantPool::symbolicate()
{
    // Loop through the entries in the constant pool
    auto const& constant_entries = parsed_pool()->entries();
    for (size_t i = 0; i < constant_entries.size(); i++) {
        // We only really need the entry for grabbing the tag, we use the index for everything else
        auto const& entry = constant_entries.at(i);

        // The constant pool references are 1-indexed
        auto index = i + 1;

        // If we already have symbolicated this index, we don't need to do anything else
        if (entries().contains(index)) {
            continue;
        }

        // Attempt to symbolicate the entry
        switch (entry->tag()) {
        case Constant::Tag::Class: {
            auto reference = TRY(SymbolicatedClassReference::create(index, this));
            entries().set(index, reference);

            break;
        }

        case Constant::Tag::MethodReference: {
            auto reference = TRY(SymbolicatedMethodReference::create(index, this));
            entries().set(index, reference);

            break;
        }

        default: {
            // FIXME: We need to use proper error types
            warnln("!!! No symbolicator for {} at {}\n", TRY(entry->debug_description()), index);
            TODO();
            break;
        }
        }
    }

    // FIXME: Remove this, only used for debugging
    dbgln("Symbolicated {} constant pool entries out of {}!", entries().size(), constant_entries.size());
    for (auto entry : entries()) {
        dbgln("  - {}: {}", entry.key, TRY(entry.value->debug_description()));
    }

    return {};
}

ErrorOr<NonnullRefPtr<SymbolicatedClassReference>> SymbolicatedConstantPool::get_or_symbolicate_class(u16 index)
{
    // Attempt to get the existing symbolicated class reference
    auto optional_reference = entries().get(index);
    if (optional_reference.has_value()) {
        auto reference = optional_reference.value();

        // We need to verify that we have the correct type
        VERIFY(reference->type() == SymbolicatedReference::Type::Class);

        // All we need to do now is cast it to the expected type :)
        auto& casted_reference = static_cast<SymbolicatedClassReference&>(*reference);
        return adopt_ref(casted_reference);
    }

    // We don't have a reference already, so let's create one.
    auto reference = TRY(SymbolicatedClassReference::create(index, this));
    entries().set(index, reference);

    return reference;
}

}
