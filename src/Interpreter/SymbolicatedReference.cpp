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
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& potential_class_info = symbolicated_pool->parsed_pool()->entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Class_info structure.
    VERIFY(potential_class_info->tag() == Constant::Tag::Class);

    // (Cast `potential_class_info` to the expected type)
    auto& class_info = static_cast<Parser::ConstantClassInfo&>(*potential_class_info);

    // The value of the `name_index` item must be a valid index into the constant_pool table.
    auto const& potential_utf8_constant = symbolicated_pool->parsed_pool()->entries().at(class_info.name_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_utf8_constant->tag() == Constant::Tag::UTF8);

    // (Cast `potential_class_info` to the expected type)
    auto& name_utf8 = static_cast<Parser::ConstantUTF8Info&>(*potential_utf8_constant);

    // For a nonarray class or an interface, the name is the binary name of the class or interface.
    auto name = name_utf8.data();

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
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& potential_method_info = symbolicated_pool->parsed_pool()->entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Methodref_info structure.
    VERIFY(potential_method_info->tag() == Constant::Tag::MethodReference);

    // (Cast `potential_method_info` to the expected type)
    auto& method_info = static_cast<Parser::ConstantMethodReferenceInfo&>(*potential_method_info);

    // The value of the `name_and_type_index` item must be a valid index into the constant_pool table.
    auto const& potential_name_and_type_constant = symbolicated_pool->parsed_pool()->entries().at(method_info.name_and_type_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_NameAndType_info structure.
    VERIFY(potential_name_and_type_constant->tag() == Constant::Tag::NameAndType);

    // (Cast `potential_name_and_type_constant` to the expected type)
    auto& name_and_type = static_cast<Parser::ConstantNameAndTypeInfo&>(*potential_name_and_type_constant);

    // The value of the `name_index` item must be a valid index into the constant_pool table.
    auto const& potential_name_utf8_constant = symbolicated_pool->parsed_pool()->entries().at(name_and_type.name_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_name_utf8_constant->tag() == Constant::Tag::UTF8);
    auto& name_utf8 = static_cast<Parser::ConstantUTF8Info&>(*potential_name_utf8_constant);

    // Represents either an unqualified name, or the special method name `<init>`
    auto name = name_utf8.data();

    // The value of the `name_index` item must be a valid index into the constant_pool table.
    auto const& potential_descriptor_utf8_constant = symbolicated_pool->parsed_pool()->entries().at(name_and_type.descriptor_index() - 1);

    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    VERIFY(potential_descriptor_utf8_constant->tag() == Constant::Tag::UTF8);

    // (Cast `potential_descriptor_utf8_constant` to the expected type)
    auto& descriptor_utf8 = static_cast<Parser::ConstantUTF8Info&>(*potential_descriptor_utf8_constant);

    // Represents a valid field or method (in this case method) descriptor.
    auto descriptor = descriptor_utf8.data();

    // The value of the `class_index` type must correspond to a SymbolicatedClassReference.
    auto owner = TRY(symbolicated_pool->get_or_symbolicate_class(method_info.class_index()));

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

}
