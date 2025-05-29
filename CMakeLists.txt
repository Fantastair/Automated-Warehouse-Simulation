cmake_minimum_required(VERSION 3.10.0)
project(AMS VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4)
    add_compile_options(/utf-8)
    add_definitions(MSVC_BUILD)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_definitions(UNIX_BUILD)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/")

if (MSVC)
    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/SDL3")
    if (NOT EXISTS ${SDL3_DIR})
        set(SDL3_DIR "C:/C_C++/Library/SDL3")
    endif()
    message(STATUS "Using SDL3 from ${SDL3_DIR}")

    find_package(SDL3 REQUIRED PATHS ${SDL3_DIR})
    find_package(SDL3_image REQUIRED PATHS ${SDL3_DIR})
    find_package(SDL3_ttf REQUIRED PATHS ${SDL3_DIR})
    find_package(SDL3_mixer REQUIRED PATHS ${SDL3_DIR})
else()
    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_ttf REQUIRED)
    find_package(SDL3_mixer REQUIRED)
endif()

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}-${CMAKE_PROJECT_VERSION})

file(GLOB 
    DISPLAY_SOURCES
    "Display/src/*.cpp"
)

file(GLOB 
    Core_SOURCES
    "Core/src/*.cpp"
)

if (MSVC)
    add_executable(${TARGET}
        main.cpp
        info.rc
        disable_dpi.manifest

        ${DISPLAY_SOURCES}
        ${Core_SOURCES}
    )
else()
    add_executable(${TARGET}
    MACOSX_BUNDLE
    main.cpp
    info.rc

    ${DISPLAY_SOURCES}
    ${Core_SOURCES}
    )
    # configure_file(Info.plist Info.plist)
    # set_target_properties(${TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ßinfo.plist)
endif()

# target_link_options(
#     ${TARGET}
#     PRIVATE
#     # "/SUBSYSTEM:WINDOWS"
# )

target_link_libraries(
    ${TARGET}
    ${SDL3_LIBRARIES}
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)
