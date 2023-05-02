/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <AK/BitStream.h>
#include <AK/Error.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/Stream.h>
#include <LibCore/File.h>
#include <LibMain/Main.h>

#include "ClassFile.h"
#include "ConstantInfo.h"
#include "ConstantPool.h"

ErrorOr<ClassFile> parse_class_file(NonnullOwnPtr<BigEndianInputBitStream> stream)
{
    // The magic value is always 0xCAFEBABE, if it's not, this isn't a .class file
    auto magic = TRY(stream->read_bits<u32>(32));
    VERIFY(magic == 0xCAFEBABE);

    auto minor_version = TRY(stream->read_bits<u16>(16));
    auto major_version = MajorVersion(TRY(stream->read_bits<u16>(16)));

    // Ensure that our major version is within our supported versions
    VERIFY(major_version >= MajorVersion::V1_1 && major_version <= MajorVersion::V17);

    auto constant_pool_count = TRY(stream->read_bits<u16>(16));

    // Parse the constant pool table, its actual length is the count -1
    auto constant_pool = TRY(ConstantPool::parse(constant_pool_count - 1, stream));

    auto access_flags = TRY(stream->read_bits<u16>(16));
    auto this_class = TRY(stream->read_bits<u16>(16));
    auto super_class = TRY(stream->read_bits<u16>(16));

    auto interfaces_count = TRY(stream->read_bits<u16>(16));
    auto interfaces = Vector<ConstantClassInfo>();
    for (auto i = 0; i < interfaces_count; i++) {
        auto index = TRY(stream->read_bits<u16>(16));
        auto const& constant = constant_pool->entries().at(index);

        // The constant_pool entry at each value of interfaces[i], where 0 ≤ i < interfaces_count, must be a CONSTANT_Class_info structure
        VERIFY(constant->tag() == ConstantPool::Tag::Class);

        auto value = static_cast<ConstantClassInfo&>(*constant);
        interfaces.append(value);
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
    };

    return file;
}

ErrorOr<int> serenity_main(Main::Arguments)
{
    auto file = TRY(Core::File::open("Example/Test.class"sv, Core::File::OpenMode::Read));
    auto buffer = TRY(Core::BufferedFile::create(move(file)));
    auto stream = TRY(try_make<BigEndianInputBitStream>(MaybeOwned<Stream>(*buffer)));

    auto parsed = TRY(parse_class_file(move(stream)));
    dbgln("{}", parsed);

    for (auto const& entry : parsed.constant_pool->entries()) {
        switch (entry->tag()) {
        case ConstantPool::Tag::UTF8: {
            auto info = static_cast<ConstantUTF8Info&>(*entry);
            dbgln("[UTF8 Entry]: {}", info.data());
            break;
        }

        case ConstantPool::Tag::Class: {
            auto info = static_cast<ConstantClassInfo&>(*entry);
            dbgln("[Class Entry]: name_index = {}", info.name_index());
            break;
        }

        case ConstantPool::Tag::FieldReference: {
            auto info = static_cast<ConstantFieldReferenceInfo&>(*entry);
            dbgln("[FieldReference Entry]: name_and_type_index = {}", info.name_and_type_index());
            break;
        }

        case ConstantPool::Tag::MethodReference: {
            auto info = static_cast<ConstantMethodReferenceInfo&>(*entry);
            dbgln("[MethodReference Entry]: name_and_type_index = {}", info.name_and_type_index());
            break;
        }

        case ConstantPool::Tag::NameAndType: {
            auto info = static_cast<ConstantNameAndTypeInfo&>(*entry);
            dbgln("[NameAndType Entry]: name_index = {}, descriptor_index = {}", info.name_index(), info.descriptor_index());
            break;
        }

        case ConstantPool::Tag::String: {
            auto info = static_cast<ConstantStringInfo&>(*entry);
            dbgln("[String Entry]: index = {}", info.index());
            break;
        }

        default:
            TODO();
        }
    }

    return 0;
}
