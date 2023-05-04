/*
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// https://docs.oracle.com/javase/specs/jvms/se17/html/jvms-4.html#jvms-4.3.2
enum FieldDescriptor : char const {
    // Signed byte
    Byte = 'B',

    // Unicode character code point in the Basic Multilingual Plane, encoded with UTF-16
    Char = 'C',

    // Double-precision floating point value
    Double = 'D',

    // Single-precision floating point value
    Float = 'F',

    // Long integer
    Long = 'J',

    // The start of `L` {ClassName} `;`
    ReferenceStart = 'L',

    // The end of `L` {ClassName} `;`
    ReferenceEnd = ';',

    // Signed short
    Short = 'S',

    // `true` or `false`
    Boolean = 'Z',

    // One array dimension
    ArrayDimension = '[',
};
