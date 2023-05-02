cmake_minimum_required(VERSION 3.22.1)
project(jvm CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use Serenity's recommended defaults
add_compile_options(-Wno-literal-suffix) # AK::StringView defines operator"" sv, which GCC complains does not have an underscore.
add_compile_options(-fno-gnu-keywords)   # JS::Value has a method named typeof, which also happens to be a GNU keyword.

# Use Lagom from SerenityOS
include(FetchContent)
include(CMake/FetchLagom.cmake)

set(SOURCES
    src/main.cpp
    src/ConstantInfo.cpp
    src/ConstantPool.cpp
)

add_executable(jvm ${SOURCES})
target_link_libraries(jvm Lagom::Core LibCore LibMain)

install(TARGETS jvm RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
