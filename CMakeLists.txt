cmake_minimum_required(VERSION 3.16)
project(calculator LANGUAGES CXX)

option(USE_CLANG_FORMAT "Format sources with clang-format before each build" OFF)
option(USE_CLANG_TIDY "Check sources with clang-tidy during compilation" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    ar_math
    GIT_REPOSITORY "https://github.com/bulbaattacks/ar_math"
    GIT_TAG        "v1.0.3"
)

FetchContent_MakeAvailable(ar_math)

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy-21 clang-tidy)
    if(NOT CLANG_TIDY_EXE)
        message(FATAL_ERROR "USE_CLANG_TIDY=ON, but clang-tidy was not found")
    endif()
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

set(CALCULATOR_SOURCES 
    src/main.cpp
    src/app.cpp
    src/app.h
)

add_executable(calculator-app ${CALCULATOR_SOURCES})

if(USE_CLANG_FORMAT)
    find_program(CLANG_FORMAT_EXE NAMES clang-format-21 clang-format)
    if(NOT CLANG_FORMAT_EXE)
        message(FATAL_ERROR "USE_CLANG_FORMAT=ON, but clang-format was not found")
    endif()

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${CALCULATOR_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting sources with clang-format"
        VERBATIM
    )
    add_dependencies(calculator-app format)
endif()

target_link_libraries(calculator-app PRIVATE ar_math)

install(TARGETS calculator-app
    RUNTIME DESTINATION bin
)
