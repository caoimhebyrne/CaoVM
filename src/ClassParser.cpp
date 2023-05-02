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
    auto magic = TRY(m_stream->read_bits<u32>(32));
    VERIFY(magic == 0xCAFEBABE);

    auto minor_version = TRY(m_stream->read_bits<u16>(16));
    auto major_version = TRY(m_stream->read_bits<u16>(16));

    // Ensure that our major version is within our supported versions
    VERIFY(major_version >= MajorVersion::V1_1 && major_version <= MajorVersion::V17);

    auto constant_pool_count = TRY(m_stream->read_bits<u16>(16));

    // Parse the constant pool table, its actual length is the count -1
    auto constant_pool = TRY(ConstantPool::parse(constant_pool_count - 1, m_stream));

    auto access_flags = TRY(m_stream->read_bits<u16>(16));
    auto this_class = TRY(m_stream->read_bits<u16>(16));
    auto super_class = TRY(m_stream->read_bits<u16>(16));

    auto interfaces_count = TRY(m_stream->read_bits<u16>(16));
    auto interfaces = Vector<ConstantClassInfo>();
    for (auto i = 0; i < interfaces_count; i++) {
        auto index = TRY(m_stream->read_bits<u16>(16));
        auto const& constant = constant_pool->entries().at(index);

        // The constant_pool entry at each value of interfaces[i], where 0 ≤ i < interfaces_count, must be a CONSTANT_Class_info structure
        VERIFY(constant->tag() == ConstantPool::Tag::Class);

        auto value = static_cast<ConstantClassInfo&>(*constant);
        interfaces.append(value);
    }

    // FIXME: Clean this up
    auto fields_count = TRY(m_stream->read_bits<u16>(16));
    auto fields = Vector<FieldInfo>();
    for (auto i = 0; i < fields_count; i++) {
        auto access_flags = TRY(m_stream->read_bits<u16>(16));
        auto name_index = TRY(m_stream->read_bits<u16>(16));
        auto descriptor_index = TRY(m_stream->read_bits<u16>(16));

        auto attributes_count = TRY(m_stream->read_bits<u16>(16));
        auto attributes = Vector<AttributeInfo>();
        for (auto i = 0; i < attributes_count; i++) {
            auto name_index = TRY(m_stream->read_bits<u16>(16));
            auto attribute_length = TRY(m_stream->read_bits<u32>(32));

            // FIXME: Implement attribute parsing
            TRY(m_stream->discard(attribute_length));

            attributes.append({ .name_index = name_index });
        }

        fields.append({
            .access_flags = access_flags,
            .name_index = name_index,
            .descriptor_index = descriptor_index,
            .attributes = attributes,
        });
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
