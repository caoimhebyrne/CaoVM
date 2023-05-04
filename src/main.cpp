/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <AK/BitStream.h>
#include <AK/Error.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/Stream.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibMain/Main.h>

#include "Interpreter/SymbolicatedConstantPool.h"

#include "Parser/ClassFile.h"
#include "Parser/ClassParser.h"
#include "Parser/ConstantInfo.h"
#include "Parser/ConstantPool.h"

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    auto dump_constant_pool = false;

    auto args_parser = make<Core::ArgsParser>();
    args_parser->add_option(dump_constant_pool, "Shows the contents of the constant pool table", "dump-constant-pool", 0, Core::ArgsParser::OptionHideMode::None);
    args_parser->parse(arguments);

    auto file = TRY(Core::File::open("Example/Test.class"sv, Core::File::OpenMode::Read));
    auto class_parser = TRY(Parser::ClassParser::create(move(file)));
    auto class_file = TRY(class_parser->parse());

    if (dump_constant_pool) {
        // Dump the constant pool table
        for (auto const& constant : class_file.constant_pool->entries()) {
            // The constant pool is 1 indexed
            auto index = class_file.constant_pool->entries().find_first_index(constant).value() + 1;
            dbgln("{}: {}", index, TRY(constant->debug_description()));
        }
    }

    // Attempt to symbolicate the parsed constant pool
    // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-5.html#jvms-5.1
    auto symbolicated_constant_pool = Interpreter::SymbolicatedConstantPool::create(move(class_file.constant_pool));
    TRY(symbolicated_constant_pool->symbolicate());

    return 0;
}
