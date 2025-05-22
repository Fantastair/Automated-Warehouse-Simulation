cmake_minimum_required(VERSION 3.10.0)
project(AMS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4)
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/")

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/SDL3")
if (NOT EXISTS SDL3_DIR)
    set(SDL3_DIR "C:/C_C++/Library/SDL3")
endif()

find_package(SDL3 REQUIRED PATHS SDL3_DIR)
find_package(SDL3_image REQUIRED PATHS SDL3_DIR)
find_package(SDL3_ttf REQUIRED PATHS SDL3_DIR)
find_package(SDL3_mixer REQUIRED PATHS SDL3_DIR)

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}-${CMAKE_PROJECT_VERSION})

add_executable(${TARGET}
    main.cpp
    info.rc
)

target_link_options(
    ${TARGET}
    PRIVATE
    # "/SUBSYSTEM:WINDOWS"
)

target_link_libraries(
    ${TARGET}
    ${SDL3_LIBRARIES}
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    )
