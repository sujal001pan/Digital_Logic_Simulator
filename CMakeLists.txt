cmake_minimum_required(VERSION 3.16)

project(DigitalLogicSimulator VERSION 1.0 LANGUAGES CXX)

# -------------------------------------------------------
# C++ Standard
# -------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------
# Output Folder
# -------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------
# Fetch SFML automatically (builds from source, no manual install)
# ----------------------------------------------------------
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# -------------------------------------------------------
# Project Sources
# -------------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)
list(FILTER SOURCES EXCLUDE REGEX "/build/")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
list(FILTER HEADERS EXCLUDE REGEX "/build/")

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# -------------------------------------------------------
# Include Directories
# -------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    src
)

# -------------------------------------------------------
# Link SFML
# -------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# -------------------------------------------------------
# Compiler Warnings
# -------------------------------------------------------
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -------------------------------------------------------
# Copy Resources Automatically
# resources/
# ├── fonts/
# ├── icons/
# └── themes/
# -------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    )
endif()

# -------------------------------------------------------
# Visual Studio Organization
# -------------------------------------------------------
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${SOURCES}
    ${HEADERS}
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})