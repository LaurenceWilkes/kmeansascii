cmake_minimum_required(VERSION 3.20)

project(fontPic
    VERSION 0.1.0
    LANGUAGES CXX
)

# display.cpp uses the native Win32 windowing and GDI APIs.
if (NOT WIN32)
    message(FATAL_ERROR "fontPic currently supports Windows only.")
endif()

add_executable(fontPic
    src/main.cpp
    src/image_io.cpp
    src/reduce.cpp
    src/discretise.cpp
    src/display.cpp
    src/html.cpp
)

# Keep the language version and include paths local to this executable.
target_compile_features(fontPic PRIVATE cxx_std_23)
set_target_properties(fontPic PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(fontPic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# stb_image is third-party code. Marking it as a system include prevents its
# warnings from obscuring warnings in the project's own source files.
target_include_directories(fontPic SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Native image display needs these Windows import libraries.
target_link_libraries(fontPic PRIVATE
    gdi32
    user32
)

# Enable useful warnings without promoting them to errors during development.
if (MSVC)
    target_compile_options(fontPic PRIVATE /W4 /permissive-)
else()
    target_compile_options(fontPic PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Image bounds checks are enabled automatically for Debug configurations.
target_compile_definitions(fontPic PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE>
)
