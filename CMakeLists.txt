cmake_minimum_required(VERSION 3.24)
project(calculator LANGUAGES CXX)

option(USE_CLANG_FORMAT "Format sources with clang-format before each build" ON)
option(USE_CLANG_TIDY "Check sources with clang-tidy during compilation" ON)
option(CALCULATOR_BUILD_TESTS "Build unit tests with GoogleTest" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)


FetchContent_Declare(
    ar_math
    GIT_REPOSITORY "https://github.com/bulbaattacks/ar_math"
    GIT_TAG        "v1.0.5"
)

FetchContent_Declare(
    json 
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    spdlog 
    GIT_REPOSITORY  "https://github.com/gabime/spdlog/"
    GIT_TAG         "v1.12.0"
    FIND_PACKAGE_ARGS 1.12.0 
)

FetchContent_MakeAvailable(ar_math json spdlog)

if(CALCULATOR_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(INSTALL_GTEST OFF)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
endif()

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy-21 clang-tidy)
    if(NOT CLANG_TIDY_EXE)
        message(FATAL_ERROR "USE_CLANG_TIDY=ON, but clang-tidy was not found")
    endif()
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()


set(CALCULATOR_SOURCES 
    src/main.cpp
)

set(CALCULATOR_LOGIC
    src/app.cpp
    src/app.h
    src/log.h
    src/log.cpp
)

add_library(calculator-logic ${CALCULATOR_LOGIC})    
      
target_include_directories(calculator-logic PUBLIC src)

target_link_libraries(calculator-logic
    PUBLIC  ar_math nlohmann_json::nlohmann_json
    PRIVATE spdlog::spdlog_header_only
)

add_executable(calculator-app ${CALCULATOR_SOURCES})     
target_link_libraries(calculator-app PRIVATE calculator-logic)        

if(USE_CLANG_FORMAT)
    find_program(CLANG_FORMAT_EXE NAMES clang-format-21 clang-format)
    if(NOT CLANG_FORMAT_EXE)
        message(FATAL_ERROR "USE_CLANG_FORMAT=ON, but clang-format was not found")
    endif()

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${CALCULATOR_SOURCES} ${CALCULATOR_LOGIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting sources with clang-format"
        VERBATIM
    )
    add_dependencies(calculator-logic format)
endif()

install(TARGETS calculator-app
    RUNTIME DESTINATION bin
)
