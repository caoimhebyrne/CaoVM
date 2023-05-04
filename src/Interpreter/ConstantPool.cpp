/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantPool.h"
#include "../Descriptor.h"

#include <AK/String.h>

namespace Interpreter {

ConstantPool::ConstantPool(NonnullOwnPtr<Parser::ConstantPool> parsed_pool)
    : m_parsed_pool(move(parsed_pool))
    , m_references(HashMap<u16, NonnullRefPtr<SymbolicReference>>())
{
}

NonnullOwnPtr<ConstantPool> ConstantPool::create(NonnullOwnPtr<Parser::ConstantPool> parsed_pool)
{
    return make<ConstantPool>(move(parsed_pool));
}

ErrorOr<void> ConstantPool::symbolicate_if_needed()
{
    auto const& entries = parsed_pool()->entries();
    for (u16 i = 0; i < entries.size(); i++) {
        auto const& entry = entries.at(i);
        auto constant_pool_index = i + 1;

        switch (entry->tag()) {
        case Parser::ConstantPool::Tag::Class: {
            dbgln("[i] Symbolicating class at {}: {}", constant_pool_index, TRY(entry->debug_description()));

            auto reference = TRY(this->get_or_symbolicate_class(constant_pool_index));
            this->references().set(constant_pool_index, reference);

            break;
        }

        case Parser::ConstantPool::Tag::FieldReference: {
            dbgln("[i] Symbolicating field reference at {}: {}", constant_pool_index, TRY(entry->debug_description()));

            auto info = static_cast<Parser::ConstantFieldReferenceInfo&>(*entry);
            auto reference = TRY(this->symbolicate_field(info));
            this->references().set(constant_pool_index, reference);

            break;
        }

        case Parser::ConstantPool::Tag::Integer:
        case Parser::ConstantPool::Tag::UTF8: {
            // dbgln("[!] Skipping constant-type field at {}: {}", constant_pool_index, TRY(entry->debug_description()));
            break;
        }

        default: {
            dbgln("[!] Unable to symbolicate entry at {}: {}", constant_pool_index, TRY(entry->debug_description()));
            break;
        }
        }
    }

    for (auto& entry : references()) {
        dbgln("{}: {}", entry.key, TRY(entry.value->debug_description()));
    }

    return {};
}

ErrorOr<NonnullRefPtr<ClassReference>> ConstantPool::get_or_symbolicate_class(u16 constant_pool_index)
{
    // Attempt to get an already symbolicated class at this index
    auto optional_class = this->references().get(constant_pool_index);

    // If we don't have an existing symbolicated entry...
    if (!optional_class.has_value()) {
        dbgln("    [*] Class not symbolicated at index {}... attempting to symbolicate!", constant_pool_index);

        // Get the class' information
        auto const& potential_class_info = parsed_pool()->entries().at(constant_pool_index - 1);

        // Ensure that it is of the right type
        VERIFY(potential_class_info->tag() == Parser::ConstantPool::Tag::Class);

        // Attempt to symbolicate the class
        auto class_reference = TRY(this->symbolicate_class(static_cast<Parser::ConstantClassInfo&>(*potential_class_info)));
        this->references().set(constant_pool_index, class_reference);

        dbgln("    [*] Class at index {} symbolicated to: {}", constant_pool_index, TRY(class_reference->debug_description()));

        return class_reference;
    }

    // We already have a symbolicated class! We just need to return that.
    auto class_reference = optional_class.value();

    dbgln("    [:D] Class already symbolicated at index {} to: {}", constant_pool_index, TRY(class_reference->debug_description()));
    return adopt_ref(static_cast<ClassReference&>(*class_reference));
}

ErrorOr<NonnullRefPtr<ClassReference>> ConstantPool::symbolicate_class(Parser::ConstantClassInfo& info)
{
    // The value of the name_index item must be a valid index into the constant_pool table.
    auto const& potential_name_constant = parsed_pool()->entries().at(info.name_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_name_constant->tag() == Parser::ConstantPool::Tag::UTF8);

    // For a nonarray class or an interface, the name is the binary name of the class or interface.
    auto name = static_cast<Parser::ConstantUTF8Info&>(*potential_name_constant).data();

    // For an array class of n dimensions...
    if (name.starts_with(FieldDescriptor::ArrayDimension)) {
        auto size = name.count("["sv);
        // The name begins with n occurrences of the ASCII [ character followed by a representation of the element type:
        dbgln("ConstantPool::symbolicate_class: Array [{}] classes not implemented!", size);
        TODO();
    }

    return TRY(try_make_ref_counted<ClassReference>(name));
}

ErrorOr<NonnullRefPtr<FieldReference>> ConstantPool::symbolicate_field(Parser::ConstantFieldReferenceInfo& info)
{
    // The value of the name_and_type_index item must be a valid index into the constant_pool table.
    auto const& constant = parsed_pool()->entries().at(info.name_and_type_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_NameAndType_info structure
    VERIFY(constant->tag() == Parser::ConstantPool::Tag::NameAndType);

    // (Cast the `constant` to the expected type)
    auto name_and_type_info = static_cast<Parser::ConstantNameAndTypeInfo&>(*constant);

    // The indices into the constant pool containing the name and descriptor of the field
    auto name_index = name_and_type_info.name_index();
    auto descriptor_index = name_and_type_info.descriptor_index();

    // The value of the name_index item must be a valid index into the constant_pool table.
    auto const& potential_name_constant = parsed_pool()->entries().at(name_index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_name_constant->tag() == Parser::ConstantPool::Tag::UTF8);

    // (Cast the constant to the expected type)
    auto name_constant = static_cast<Parser::ConstantUTF8Info&>(*potential_name_constant);

    // The value of the descriptor_index item must be a valid index into the constant_pool table.
    auto const& potential_descriptor_constant = parsed_pool()->entries().at(descriptor_index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_descriptor_constant->tag() == Parser::ConstantPool::Tag::UTF8);

    // (Cast the constant to the expected type)
    auto descriptor_constant = static_cast<Parser::ConstantUTF8Info&>(*potential_descriptor_constant);

    auto name = name_constant.data();
    auto descriptor = descriptor_constant.data();
    auto owner = TRY(this->get_or_symbolicate_class(info.class_index()));

    dbgln("    [:D] Symbolicated field {}: {} with owner: {}", name, descriptor, TRY(owner->debug_description()));

    return TRY(try_make_ref_counted<FieldReference>(name, descriptor, owner));
}

}
