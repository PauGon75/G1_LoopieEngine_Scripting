cmake_minimum_required (VERSION 3.10)
project ("LoopieEngine" VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/EHs" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/EHc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/GR- /EHs-)
else()
    add_compile_options(-fno-rtti -fno-exceptions)
endif()

# --- Mono Configuration ---
set(MONO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono" CACHE PATH "Mono root directory")
set(MONO_INCLUDE_DIR "${MONO_ROOT}/include/mono-2.0" CACHE PATH "Mono include directory")
set(MONO_LIB_DIR "${MONO_ROOT}/lib" CACHE PATH "Mono library directory")
set(MONO_BIN_DIR "${MONO_ROOT}/bin" CACHE PATH "Mono binary directory")

# Verify Mono paths exist
if(NOT EXISTS "${MONO_INCLUDE_DIR}")
    message(FATAL_ERROR "Mono include directory not found: ${MONO_INCLUDE_DIR}\nPlease check that Mono is installed in ${MONO_ROOT}")
endif()

# Find Mono library
find_library(MONO_LIBRARY 
    NAMES mono-2.0-sgen monosgen-2.0
    PATHS "${MONO_LIB_DIR}"
    NO_DEFAULT_PATH
)

if(NOT MONO_LIBRARY)
    message(FATAL_ERROR "Mono library not found in: ${MONO_LIB_DIR}")
endif()

# Create imported Mono target
add_library(Mono::Runtime SHARED IMPORTED GLOBAL)
set_target_properties(Mono::Runtime PROPERTIES
    IMPORTED_LOCATION "${MONO_BIN_DIR}/mono-2.0-sgen.dll"
    IMPORTED_IMPLIB "${MONO_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${MONO_INCLUDE_DIR}"
)

message(STATUS "Mono configured:")
message(STATUS "  Root: ${MONO_ROOT}")
message(STATUS "  Include: ${MONO_INCLUDE_DIR}")
message(STATUS "  Library: ${MONO_LIBRARY}")
# --------------------------

#Subdirectories
add_subdirectory("Loopie")
add_subdirectory("LoopieEditor")
#add_subdirectory("LoopieSandbox")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoopieEditor)    