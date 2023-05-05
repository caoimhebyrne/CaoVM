/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ConstantPool.h"
#include "ClassParser.h"
#include "ConstantInfo.h"
#include <AK/BitStream.h>
#include <AK/String.h>

namespace Parser {

ErrorOr<NonnullOwnPtr<ConstantPool>> ConstantPool::parse(u16 size, ClassParser& class_parser)
{
    auto entries = Vector<NonnullRefPtr<ConstantInfo>>();

    for (int i = 0; i < size; i++) {
        // The constant_pool table is indexed from 1 to constant_pool_count - 1.
        auto pool_index = i + 1;
        auto tag = TRY(class_parser.read_u1());

        switch (tag) {
        case Constant::Tag::FieldReference: {
            entries.append(TRY(ConstantMemberReferenceInfo::parse(Constant::Tag::FieldReference, class_parser)));
            break;
        }

        case Constant::Tag::MethodReference: {
            entries.append(TRY(ConstantMemberReferenceInfo::parse(Constant::Tag::MethodReference, class_parser)));
            break;
        }

        case Constant::Tag::Class: {
            entries.append(TRY(ConstantClassInfo::parse(class_parser)));
            break;
        }

        case Constant::Tag::NameAndType: {
            entries.append(TRY(ConstantNameAndTypeInfo::parse(class_parser)));
            break;
        }

        case Constant::Tag::UTF8: {
            entries.append(TRY(ConstantUTF8Info::parse(class_parser)));
            break;
        }

        case Constant::Tag::String: {
            entries.append(TRY(ConstantStringInfo::parse(class_parser)));
            break;
        }

        case Constant::Tag::Integer: {
            entries.append(TRY(ConstantIntegerInfo::parse(class_parser)));
            break;
        }

        default: {
            dbgln("Unimplemented tag @ {}: {}", pool_index, tag);
            TODO();
        }
        }
    }

    return make<ConstantPool>(move(entries));
}

// Attempts to read a method reference from the constant pool
ErrorOr<NonnullRefPtr<ConstantMethodReferenceInfo>> ConstantPool::method_reference_at(u16 index)
{
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& entry = entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Methodref_info structure.
    VERIFY(entry->tag() == Constant::Tag::MethodReference);

    return static_cast<Parser::ConstantMethodReferenceInfo&>(*entry);
}

// Attempts to read a method reference from the constant pool
ErrorOr<NonnullRefPtr<ConstantNameAndTypeInfo>> ConstantPool::name_and_type_at(u16 index)
{
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& entry = entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_NameAndType_info structure.
    VERIFY(entry->tag() == Constant::Tag::NameAndType);

    return static_cast<Parser::ConstantNameAndTypeInfo&>(*entry);
}

// Attempts to read a utf8 constant from the constant pool
ErrorOr<NonnullRefPtr<ConstantUTF8Info>> ConstantPool::utf8_at(u16 index)
{
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& entry = entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_UTF8_info structure.
    VERIFY(entry->tag() == Constant::Tag::UTF8);

    return static_cast<Parser::ConstantUTF8Info&>(*entry);
}

// Attempts to read a class' information from the constant pool
ErrorOr<NonnullRefPtr<ConstantClassInfo>> ConstantPool::class_at(u16 index)
{
    // The value of the `index` item must be a valid index into the constant_pool table.
    auto const& entry = entries().at(index - 1);

    // The constant_pool entry at that index must be a CONSTANT_Class_info structure.
    VERIFY(entry->tag() == Constant::Tag::Class);

    return static_cast<Parser::ConstantClassInfo&>(*entry);
}

}
