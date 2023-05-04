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
    , m_references(Vector<NonnullRefPtr<SymbolicReference>>())
{
}

NonnullOwnPtr<ConstantPool> ConstantPool::create(NonnullOwnPtr<Parser::ConstantPool> parsed_pool)
{
    return make<ConstantPool>(move(parsed_pool));
}

ErrorOr<void> ConstantPool::symbolicate_if_needed()
{
    for (auto const& entry : parsed_pool()->entries()) {
        switch (entry->tag()) {
        case Parser::ConstantPool::Tag::Class: {
            auto info = static_cast<Parser::ConstantClassInfo&>(*entry);
            TRY(this->symbolicate_class(info));

            break;
        }

        case Parser::ConstantPool::Tag::FieldReference: {
            auto info = static_cast<Parser::ConstantFieldReferenceInfo&>(*entry);
            TRY(this->symbolicate_field(info));

            break;
        }

        case Parser::ConstantPool::Tag::MethodReference: {
            auto info = static_cast<Parser::ConstantFieldReferenceInfo&>(*entry);
            TRY(this->symbolicate_field(info));

            break;
        }

        default: {
            dbgln("Unimplemented {}", TRY(entry->debug_description()));
            // TODO();
        }
        }
    }
    for (auto& entry : references()) {
        dbgln("{}", TRY(entry->debug_description()));
    }

    return {};
}

ErrorOr<void> ConstantPool::symbolicate_class(Parser::ConstantClassInfo& info)
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

    auto reference = TRY(try_make_ref_counted<ClassReference>(name));
    this->references().append(reference);

    return {};
}

// TODO:
// We need way better way at associating a "symbolicated reference" of an entry in the constant pool to its original index (or info class).
// Not really sure how to do that at the minute, hence why this bunch of duplicated code exists!
ErrorOr<NonnullRefPtr<ClassReference>> ConstantPool::get_or_symbolicate_class(u16 class_index)
{
    // The value of the class_index item must be a valid index into the constant_pool table.
    auto const& entry = parsed_pool()->entries().at(class_index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Class_info structure.
    VERIFY(entry->tag() == Parser::ConstantPool::Tag::Class);

    // (Cast the `entry` to the expected type)
    auto class_info = static_cast<Parser::ConstantClassInfo&>(*entry);

    // The value of the name_index item must be a valid index into the constant_pool table.
    auto const& potential_name_constant = parsed_pool()->entries().at(class_info.name_index() - 1);

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

ErrorOr<void> ConstantPool::symbolicate_field(Parser::ConstantFieldReferenceInfo& info)
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

    auto reference = TRY(try_make_ref_counted<FieldReference>(name, descriptor, owner));
    this->references().append(reference);

    return {};
}

}
