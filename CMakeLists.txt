cmake_minimum_required(VERSION 3.10.0)
project(AMS VERSION 0.9.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_options(/utf-8)
    add_compile_definitions(MSVC_BUILD)
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
    set(CMAKE_MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/info.plist.in)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${TARGET}
    MACOSX_BUNDLE
    main.cpp
    icon.icns
    res/
    
    ${DISPLAY_SOURCES}
    ${Core_SOURCES}
    )
    add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:${TARGET}>/../Resources/res
    )
endif()


target_compile_definitions(${TARGET}
    PRIVATE
    APP_NAME="${TARGET}"
    )

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
