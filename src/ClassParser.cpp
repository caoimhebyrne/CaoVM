/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ClassParser.h"
#include <AK/BitStream.h>
#include <AK/MaybeOwned.h>
#include <AK/String.h>

ClassParser::ClassParser(NonnullOwnPtr<BigEndianInputBitStream> stream)
    : m_stream(move(stream))
{
}

ErrorOr<NonnullOwnPtr<ClassParser>> ClassParser::create(NonnullOwnPtr<Core::File> file)
{
    auto buffer = TRY(Core::BufferedFile::create(move(file)));
    auto bit_stream = TRY(try_make<BigEndianInputBitStream>(MaybeOwned<Stream>(move(buffer))));

    return make<ClassParser>(move(bit_stream));
}

ErrorOr<ClassFile> ClassParser::parse()
{
    // The magic value is always 0xCAFEBABE, if it's not, this isn't a .class file
    auto magic = TRY(this->read_u4());
    VERIFY(magic == 0xCAFEBABE);

    // The minor version isn't really used that much in the class file spec.
    // We don't care about it but we'll parse it anyways!
    auto minor_version = TRY(this->read_u2());

    // The version that this class file was compiled for (e.g. 52 for Java 8 or 61 for Java 17)
    auto major_version = TRY(this->read_u2());

    // Ensure that our major version is within our supported versions
    VERIFY(major_version >= MajorVersion::V1_1 && major_version <= MajorVersion::V17);

    // The constant pool is a table of structures representing various constants!
    auto constant_pool_count = TRY(this->read_u2());
    auto constant_pool = TRY(ConstantPool::parse(constant_pool_count - 1, m_stream));

    // Used to denote access permissions to this class/interface and its properties
    auto access_flags = TRY(this->read_u2());

    // An index into the constant pool table to the class defined by this file
    auto this_class = TRY(this->read_u2());

    // An index into the constant pool table to the super class of the class in this file.
    // It may be 0, and if it is, then this class file must represent the class Object, the only class or interface without a direct superclass.
    auto super_class = TRY(this->read_u2());

    // The direct super-interfaces of this class/interface
    auto interfaces_length = TRY(this->read_u2());
    auto interfaces = Vector<ConstantClassInfo>();
    for (auto i = 0; i < interfaces_length; i++) {
        // Each "interface" is just a reference to an index in the constant pool table
        auto interface_info = TRY(this->parse_interface(constant_pool));
        interfaces.append(interface_info);
    }

    // The number of field_info structures in the fields table.
    auto fields_length = TRY(this->read_u2());

    // The field_info structures represent all fields, both class and instance variables defined by this class/interface.
    auto fields = Vector<FieldInfo>();
    for (auto i = 0; i < fields_length; i++) {
        auto field_info = TRY(this->parse_field());
        fields.append(field_info);
    }

    // Construct a class file struct
    ClassFile file {
        .magic = magic,
        .minor_version = minor_version,
        .major_version = major_version,
        .constant_pool_count = constant_pool_count,
        .constant_pool = move(constant_pool),
        .access_flags = access_flags,
        .this_class = this_class,
        .super_class = super_class,
        .interfaces = interfaces,
        .fields = fields,
    };

    return file;
}

ErrorOr<ConstantClassInfo> ClassParser::parse_interface(NonnullOwnPtr<ConstantPool> const& constant_pool)
{
    // Each value in ther interfaces array is an index into the constant pool table
    auto index = TRY(this->read_u2());
    auto const& constant = constant_pool->entries().at(index);

    // The constant_pool entry at each value of interfaces[i], where 0 ≤ i < interfaces_count, must be a CONSTANT_Class_info structure
    VERIFY(constant->tag() == ConstantPool::Tag::Class);

    // ~~ Fancy casting woooo ~~
    return static_cast<ConstantClassInfo&>(*constant);
}

ErrorOr<FieldInfo> ClassParser::parse_field()
{
    // Used to denote access permission to this field
    auto access_flags = TRY(this->read_u2());

    // An index in the constant pool table to the name of this field
    auto name_index = TRY(this->read_u2());

    // An index in the constant pool table to the descriptor for this field
    auto descriptor_index = TRY(this->read_u2());

    // The amount of attributes belonging to this field
    auto attributes_count = TRY(this->read_u2());
    auto attributes = Vector<AttributeInfo>();
    for (auto i = 0; i < attributes_count; i++) {
        auto attribute = TRY(this->parse_attribute());
        attributes.append(attribute);
    }

    return FieldInfo {
        .access_flags = access_flags,
        .name_index = name_index,
        .descriptor_index = descriptor_index,
        .attributes = attributes,
    };
}

ErrorOr<AttributeInfo> ClassParser::parse_attribute()
{
    // An index in the constant pool table to name of this attribute
    auto name_index = TRY(this->read_u2());

    // The length of the data for this attribute, immediately after the end of this u4
    auto attribute_length = TRY(this->read_u4());

    // FIXME: Implement attribute parsing
    TRY(m_stream->discard(attribute_length));

    return AttributeInfo {
        .name_index = name_index,
    };
}

ErrorOr<u8> ClassParser::read_u1()
{
    return m_stream->read_bits<u8>(8);
}

ErrorOr<u16> ClassParser::read_u2()
{
    return m_stream->read_bits<u16>(16);
}

ErrorOr<u32> ClassParser::read_u4()
{
    return m_stream->read_bits<u32>(32);
}