/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "../Parser/ConstantInfo.h"
#include "../Parser/ConstantPool.h"
#include <AK/HashMap.h>
#include <AK/NonnullOwnPtr.h>

namespace Interpreter {

// Symbolic references, which may later be resolved
// The symbolic references in the run-time constant pool are derived
// from entries in the constant_pool table in accordance with the structure of each entry.
class SymbolicReference : public RefCounted<SymbolicReference> {
public:
    SymbolicReference(Parser::ConstantPool::Tag tag)
        : m_tag(move(tag))
    {
    }

    virtual ErrorOr<String> debug_description() = 0;

    Parser::ConstantPool::Tag const& tag() { return m_tag; };

private:
    Parser::ConstantPool::Tag m_tag;
};

// A symbolic reference to a class or interface is derived from a CONSTANT_Class_info structure
class ClassReference : public SymbolicReference {

public:
    ClassReference(String name)
        : SymbolicReference(Parser::ConstantPool::Tag::Class)
        , m_name(move(name))
    {
    }

    ErrorOr<String> debug_description()
    {
        StringBuilder builder;

        builder.append("ClassReference { "sv);
        builder.appendff("name = \"{}\"", name());
        builder.append(" }"sv);

        return builder.to_string();
    }

    String const& name() { return m_name; };

private:
    String m_name;
};

// A symbolic reference to a field of a class or an interface is derived from a CONSTANT_Fieldref_info structure
class FieldReference : public SymbolicReference {
public:
    FieldReference(String name, String descriptor, NonnullRefPtr<ClassReference> owner)
        : SymbolicReference(Parser::ConstantPool::Tag::FieldReference)
        , m_name(move(name))
        , m_descriptor(move(descriptor))
        , m_owner(owner)
    {
    }

    String const& name() { return m_name; };
    String const& descriptor() { return m_descriptor; };
    NonnullRefPtr<ClassReference> owner() { return m_owner; };

    ErrorOr<String> debug_description()
    {
        StringBuilder builder;

        builder.append("FieldReference { "sv);
        builder.appendff("name = \"{}\", ", name());
        builder.appendff("descriptor = \"{}\", ", descriptor());
        builder.appendff("owner = {}", TRY(owner()->debug_description()));
        builder.append(" }"sv);

        return builder.to_string();
    }

private:
    String m_name;
    String m_descriptor;
    NonnullRefPtr<ClassReference> m_owner;
};

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-5.html#jvms-5.1
class ConstantPool {
public:
    ConstantPool(NonnullOwnPtr<Parser::ConstantPool> parsed_pool);
    static NonnullOwnPtr<ConstantPool> create(NonnullOwnPtr<Parser::ConstantPool> parsed_pool);

    ErrorOr<void> symbolicate_if_needed();

    NonnullOwnPtr<Parser::ConstantPool> const& parsed_pool() { return m_parsed_pool; };

    HashMap<u16, NonnullRefPtr<SymbolicReference>>& references() { return m_references; };

private:
    NonnullOwnPtr<Parser::ConstantPool> m_parsed_pool;

    HashMap<u16, NonnullRefPtr<SymbolicReference>> m_references;

    ErrorOr<NonnullRefPtr<ClassReference>> get_or_symbolicate_class(u16 constant_pool_index);

    ErrorOr<NonnullRefPtr<ClassReference>> symbolicate_class(Parser::ConstantClassInfo& info);
    ErrorOr<NonnullRefPtr<FieldReference>> symbolicate_field(Parser::ConstantFieldReferenceInfo& info);
};

}
