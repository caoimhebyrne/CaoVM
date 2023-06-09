/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <AK/Types.h>

// We need to scope the enum, as using an `enum class` is undesirable
// and not scoping an `enum` causes issues.
struct Constant {
    // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4-140
    enum Tag : u8 {
        // Raw UTF8 data
        UTF8 = 1,

        // Used to represent constant primitives of the type int
        Integer = 3,

        // Used to represent a class or an interface
        Class = 7,

        // Used to represent constant objects of the type String
        String = 8,

        // Used to represent a reference to a field on an Object
        FieldReference = 9,

        // Used to represent a reference to a method on an Object
        MethodReference = 10,

        // Used to represent a field or a method, without indicating which class or interface it belongs to
        NameAndType = 12,
    };
};
