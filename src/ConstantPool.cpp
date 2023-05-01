/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantPool.h"
#include <AK/BitStream.h>
#include <AK/String.h>

ErrorOr<void> ConstantPool::parse_constant_pool(u16 size, NonnullOwnPtr<BigEndianInputBitStream>& stream)
{
    // FIXME: Let's parse these into structs
    for (int i = 0; i < size; i++) {
        // The constant_pool table is indexed from 1 to constant_pool_count - 1.
        auto pool_index = i + 1;

        auto tag = TRY(stream->read_bits<u8>(8));
        switch (tag) {
        case ConstantPool::Tag::FieldReference:
        case ConstantPool::Tag::MethodReference: {
            auto class_index = TRY(stream->read_bits<u16>(16));
            auto name_and_type_index = TRY(stream->read_bits<u16>(16));

            dbgln("[Method Reference @ {}] class_index: {}, name_and_type_index: {}", pool_index, class_index, name_and_type_index);
            break;
        }

        case ConstantPool::Tag::Class: {
            auto name_index = TRY(stream->read_bits<u16>(16));

            dbgln("[Class @ {}] name_index: {}", pool_index, name_index);
            break;
        }

        case ConstantPool::Tag::NameAndType: {
            auto name_index = TRY(stream->read_bits<u16>(16));
            auto descriptor_index = TRY(stream->read_bits<u16>(16));

            dbgln("[Name and Type @ {}] name_index: {}, descriptor_index: {}", pool_index, name_index, descriptor_index);
            break;
        }

        case ConstantPool::Tag::UTF8: {
            auto length = TRY(stream->read_bits<u16>(16));
            auto buffer = TRY(ByteBuffer::create_uninitialized(length));

            TRY(stream->read_until_filled(buffer));

            dbgln("[UTF8 @ {}] length: {}, bytes: {}", pool_index, length, String::from_utf8(StringView { buffer }));
            break;
        }

        case ConstantPool::Tag::String: {
            auto index = TRY(stream->read_bits<u16>(16));

            dbgln("[String @ {}] index: {}", pool_index, index);
            break;
        }

        default: {
            dbgln("Unimplemented tag @ {}: {}", pool_index, tag);
            TODO();
        }
        }
    }

    return {};
}
