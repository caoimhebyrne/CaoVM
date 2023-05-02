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
#include "ClassParser.h"
#include "ConstantInfo.h"
#include "ConstantPool.h"

ErrorOr<int> serenity_main(Main::Arguments)
{
    auto file = TRY(Core::File::open("Example/Test.class"sv, Core::File::OpenMode::Read));
    auto class_parser = TRY(ClassParser::create(move(file)));
    auto class_file = TRY(class_parser->parse());

    dbgln("{}", class_file);

    // Dump the constant pool table
    for (auto const& constant : class_file.constant_pool->entries()) {
        // The constant pool is 1 indexed
        auto index = class_file.constant_pool->entries().find_first_index(constant).value() + 1;
        dbgln("{}: {}", index, TRY(constant->debug_description()));
    }

    return 0;
}
