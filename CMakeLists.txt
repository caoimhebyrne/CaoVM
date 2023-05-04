cmake_minimum_required(VERSION 3.22.1)
project(jvm CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-fno-gnu-keywords)              # JS::Value has a method named typeof, which also happens to be a GNU keyword.
add_compile_options(-Wno-unknown-warning-option)    # We don't really care about if an option doesn't exist

# Both GCC and clang complain about the same issue, but use different warning names.
# AK::StringView defines operator"" sv, they complain because it does not have an underscore.
add_compile_options(-Wno-user-defined-literals)
add_compile_options(-Wno-literal-suffix)

# Use Lagom from SerenityOS
include(FetchContent)
include(CMake/FetchLagom.cmake)

set(SOURCES
    src/main.cpp

    src/Interpreter/ConstantPool.cpp

    src/Parser/Attribute.cpp
    src/Parser/ClassParser.cpp
    src/Parser/ConstantInfo.cpp
    src/Parser/ConstantPool.cpp
)

add_executable(jvm ${SOURCES})
target_link_libraries(jvm Lagom::Core LibCore LibMain)

install(TARGETS jvm RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
