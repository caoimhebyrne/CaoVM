/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "SymbolicatedReference.h"
#include "../Descriptor.h"
#include "../Parser/ConstantInfo.h"
#include "SymbolicatedConstantPool.h"

namespace Interpreter {

SymbolicatedReference::SymbolicatedReference(u16 index, SymbolicatedReference::Type type)
    : m_index(move(index))
    , m_type(move(type))
{
}

// A symbolic reference to a class or interface is derived from a CONSTANT_Class_info structure
SymbolicatedClassReference::SymbolicatedClassReference(u16 index, String name)
    : SymbolicatedReference(index, SymbolicatedReference::Type::Class)
    , m_name(move(name))
{
}

ErrorOr<NonnullRefPtr<SymbolicatedClassReference>> SymbolicatedClassReference::create(u16 index, SymbolicatedConstantPool* symbolicated_pool)
{
    // The entry at the index must be a Constant_Class_info structure.
    auto const& class_info = TRY(symbolicated_pool->parsed_pool()->class_at(index));

    // The entry at `name_index` must be a Constant_Utf8_info structure.
    auto const& name_utf8 = TRY(symbolicated_pool->parsed_pool()->utf8_at(class_info->name_index()));

    // For a nonarray class or an interface, the name is the binary name of the class or interface.
    auto name = name_utf8->data();

    // For an array class of n dimensions...
    // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-5.html#:~:text=For%20an%20array%20class%20of%20n%20dimensions
    if (name.starts_with(FieldDescriptor::ArrayDimension)) {
        dbgln("SymbolicatedClassReference::create: Array [{}] classes not implemented!");
        TODO();
    }

    return try_make_ref_counted<SymbolicatedClassReference>(index, name);
}

// Used for debugging
ErrorOr<String> SymbolicatedClassReference::debug_description()
{
    StringBuilder builder;

    builder.append("SymbolicatedClassReference { "sv);
    builder.appendff("name = \"{}\"", this->name());
    builder.append(" }"sv);

    return builder.to_string();
}

// A symbolic reference to a method of a class is derived from a CONSTANT_Methodref_info structure
SymbolicatedMethodReference::SymbolicatedMethodReference(u16 index, String name, String descriptor, NonnullRefPtr<SymbolicatedClassReference> owner)
    : SymbolicatedReference(index, SymbolicatedReference::Type::Method)
    , m_name(move(name))
    , m_descriptor(move(descriptor))
    , m_owner(owner)
{
}

// Attempts to symbolicate a class reference, given its index into the parsed constant pool
ErrorOr<NonnullRefPtr<SymbolicatedMethodReference>> SymbolicatedMethodReference::create(u16 index, SymbolicatedConstantPool* symbolicated_pool)
{
    // The entry at the index must be a Constant_Methodref_info structure.
    auto const& field_info = TRY(symbolicated_pool->parsed_pool()->method_reference_at(index));

    // The entry at `name_and_type_index` must be a CONSTANT_NameAndType_info structure.
    auto const& name_and_type = TRY(symbolicated_pool->parsed_pool()->name_and_type_at(field_info->name_and_type_index()));

    // The entries at `name_index` and `descriptor_index` must be CONSTANT_Utf8_info structures.
    auto const& name_utf8 = TRY(symbolicated_pool->parsed_pool()->utf8_at(name_and_type->name_index()));
    auto const& descriptor_utf8 = TRY(symbolicated_pool->parsed_pool()->utf8_at(name_and_type->descriptor_index()));

    // Represents the field's name
    auto name = name_utf8->data();

    // Represents a valid field or method (in this case field) descriptor.
    auto descriptor = descriptor_utf8->data();

    // The value of the `class_index` type must correspond to a SymbolicatedClassReference.
    auto owner = TRY(symbolicated_pool->get_or_symbolicate_class(field_info->class_index()));

    return try_make_ref_counted<SymbolicatedMethodReference>(index, name, descriptor, owner);
}

// Used for debugging
ErrorOr<String> SymbolicatedMethodReference::debug_description()
{
    StringBuilder builder;

    builder.append("SymbolicatedMethodReference { "sv);
    builder.appendff("name = \"{}\", ", this->name());
    builder.appendff("descriptor = \"{}\", ", this->descriptor());
    builder.appendff("owner = {}", TRY(this->owner()->debug_description()));
    builder.append(" }"sv);

    return builder.to_string();
}

// A symbolic reference to a method of a class is derived from a CONSTANT_Fieldref_info structure
// Very similar to a SymbolicatedMethodReference
SymbolicatedFieldReference::SymbolicatedFieldReference(u16 index, String name, String descriptor, NonnullRefPtr<SymbolicatedClassReference> owner)
    : SymbolicatedReference(index, SymbolicatedReference::Type::Field)
    , m_name(move(name))
    , m_descriptor(move(descriptor))
    , m_owner(owner)
{
}

// Attempts to symbolicate a class reference, given its index into the parsed constant pool
ErrorOr<NonnullRefPtr<SymbolicatedFieldReference>> SymbolicatedFieldReference::create(u16 index, SymbolicatedConstantPool* symbolicated_pool)
{
    // The entry at the index must be a Constant_Methodref_info structure.
    auto const& method_info = TRY(symbolicated_pool->parsed_pool()->field_reference_at(index));

    // The entry at `name_and_type_index` must be a CONSTANT_NameAndType_info structure.
    auto const& name_and_type = TRY(symbolicated_pool->parsed_pool()->name_and_type_at(method_info->name_and_type_index()));

    // The entries at `name_index` and `descriptor_index` must be CONSTANT_Utf8_info structures.
    auto const& name_utf8 = TRY(symbolicated_pool->parsed_pool()->utf8_at(name_and_type->name_index()));
    auto const& descriptor_utf8 = TRY(symbolicated_pool->parsed_pool()->utf8_at(name_and_type->descriptor_index()));

    // Represents either an unqualified name, or the special method name `<init>`
    auto name = name_utf8->data();

    // Represents a valid field or method (in this case method) descriptor.
    auto descriptor = descriptor_utf8->data();

    // The value of the `class_index` type must correspond to a SymbolicatedClassReference.
    auto owner = TRY(symbolicated_pool->get_or_symbolicate_class(method_info->class_index()));

    return try_make_ref_counted<SymbolicatedFieldReference>(index, name, descriptor, owner);
}

// Used for debugging
ErrorOr<String> SymbolicatedFieldReference::debug_description()
{
    StringBuilder builder;

    builder.append("SymbolicatedFieldReference { "sv);
    builder.appendff("name = \"{}\", ", this->name());
    builder.appendff("descriptor = \"{}\", ", this->descriptor());
    builder.appendff("owner = {}", TRY(this->owner()->debug_description()));
    builder.append(" }"sv);

    return builder.to_string();
}

}
