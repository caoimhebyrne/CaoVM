/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/RefCounted.h>
#include <AK/String.h>

namespace Interpreter {

// Forward-declaration, these classes are dependant on each-other!
class SymbolicatedConstantPool;

// A SymbolicatedReference is similar to a ConstantInfo class from the parser.
//
// It takes a ConstantInfo structure, and resolves any un-resolved information.
// For example:
// - The SymbolicatedClassReference class will take the `name_index` from the ConstantClassInfo
//   - It will then attempt to get the name from the UTF8 entry pointed to by that index
// - The class' name will be stored in the SymbolicatedClassReference
// - The SymbolicatedClassReference can be referenced to by other references
//   - For example: a field member reference will have a SymbolicatedClassReference as an owner!
class SymbolicatedReference : public RefCounted<SymbolicatedReference> {
public:
    enum Type {
        // A symbolic reference to a class or interface is derived from a CONSTANT_Class_info structure
        Class,

        // A symbolic reference to a method of a class is derived from a CONSTANT_Methodref_info structure
        Method,

        // A symbolic reference to a method of a class is derived from a CONSTANT_Fieldref_info structure
        Field
    };

    SymbolicatedReference(u16 index, Type type);
    virtual ~SymbolicatedReference() = default;

    // Used for debugging
    virtual ErrorOr<String> debug_description() = 0;

    // The index into the constant pool for the original constant
    // used to derive this symbolicated reference
    u16 const& index() { return m_index; };

    // Used to identify what type of SymbolicatedReference this is
    Type const& type() { return m_type; };

private:
    u16 m_index;
    Type m_type;
};

// A symbolic reference to a class or interface is derived from a CONSTANT_Class_info structure
class SymbolicatedClassReference : public SymbolicatedReference {
public:
    SymbolicatedClassReference(u16 index, String name);

    // Attempts to symbolicate a class reference, given its index into the parsed constant pool
    static ErrorOr<NonnullRefPtr<SymbolicatedClassReference>> create(u16 index, SymbolicatedConstantPool* symbolicated_pool);

    // Used for debugging
    ErrorOr<String> debug_description();

    // The fully qualified name of this class
    String const& name() { return m_name; };

private:
    String m_name;
};

// A symbolic reference to a method of a class is derived from a CONSTANT_Methodref_info structure
class SymbolicatedMethodReference : public SymbolicatedReference {
public:
    SymbolicatedMethodReference(u16 index, String name, String descriptor, NonnullRefPtr<SymbolicatedClassReference> owner);

    // Attempts to symbolicate a method reference, given its index into the parsed constant pool
    static ErrorOr<NonnullRefPtr<SymbolicatedMethodReference>> create(u16 index, SymbolicatedConstantPool* symbolicated_pool);

    // Used for debugging
    ErrorOr<String> debug_description();

    // The unqualified name of this method
    String const& name() { return m_name; };

    // The descriptor (signature) of this method
    String const& descriptor() { return m_descriptor; };

    NonnullRefPtr<SymbolicatedClassReference> const& owner() { return m_owner; };

private:
    String m_name;
    String m_descriptor;
    NonnullRefPtr<SymbolicatedClassReference> m_owner;
};

// A symbolic reference to a method of a class is derived from a CONSTANT_Fieldref_info structure
// Very similar to a SymbolicatedMethodReference
class SymbolicatedFieldReference : public SymbolicatedReference {
public:
    SymbolicatedFieldReference(u16 index, String name, String descriptor, NonnullRefPtr<SymbolicatedClassReference> owner);

    // Attempts to symbolicate a method reference, given its index into the parsed constant pool
    static ErrorOr<NonnullRefPtr<SymbolicatedFieldReference>> create(u16 index, SymbolicatedConstantPool* symbolicated_pool);

    // Used for debugging
    ErrorOr<String> debug_description();

    // The unqualified name of this method
    String const& name() { return m_name; };

    // The descriptor (signature) of this method
    String const& descriptor() { return m_descriptor; };

    NonnullRefPtr<SymbolicatedClassReference> const& owner() { return m_owner; };

private:
    String m_name;
    String m_descriptor;
    NonnullRefPtr<SymbolicatedClassReference> m_owner;
};

}
