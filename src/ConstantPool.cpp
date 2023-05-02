/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantPool.h"
#include "ConstantInfo.h"
#include <AK/BitStream.h>
#include <AK/String.h>

ErrorOr<Vector<NonnullOwnPtr<ConstantInfo>>> ConstantPool::parse_constant_pool(u16 size, NonnullOwnPtr<BigEndianInputBitStream>& stream)
{
    auto table = Vector<NonnullOwnPtr<ConstantInfo>>();

    for (int i = 0; i < size; i++) {
        // The constant_pool table is indexed from 1 to constant_pool_count - 1.
        auto pool_index = i + 1;
        auto tag = TRY(stream->read_bits<u8>(8));

        switch (tag) {
        case ConstantPool::Tag::FieldReference: {
            table.append(TRY(ConstantMemberReferenceInfo::parse(ConstantPool::Tag::FieldReference, stream)));
            break;
        }

        case ConstantPool::Tag::MethodReference: {
            table.append(TRY(ConstantMemberReferenceInfo::parse(ConstantPool::Tag::MethodReference, stream)));
            break;
        }

        case ConstantPool::Tag::Class: {
            table.append(TRY(ConstantClassInfo::parse(stream)));
            break;
        }

        case ConstantPool::Tag::NameAndType: {
            table.append(TRY(ConstantNameAndTypeInfo::parse(stream)));
            break;
        }

        case ConstantPool::Tag::UTF8: {
            table.append(TRY(ConstantUTF8Info::parse(stream)));
            break;
        }

        case ConstantPool::Tag::String: {
            table.append(TRY(ConstantStringInfo::parse(stream)));
            break;
        }

        default: {
            dbgln("Unimplemented tag @ {}: {}", pool_index, tag);
            TODO();
        }
        }
    }

    return table;
}
