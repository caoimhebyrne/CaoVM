/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <AK/Forward.h>
#include <AK/Types.h>

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4
class ConstantPool {
public:
    // https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.4-140
    enum Tag : u8 {
        // Raw UTF8 data
        UTF8 = 1,

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

    static ErrorOr<void> parse_constant_pool(u16 size, NonnullOwnPtr<BigEndianInputBitStream>& stream);
};
