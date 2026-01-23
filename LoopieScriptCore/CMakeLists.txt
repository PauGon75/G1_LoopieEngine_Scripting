cmake_minimum_required(VERSION 3.10)
project("LoopieScripting" NONE)

find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

set(LOOPIE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/LoopieScriptProject.csproj")

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/assets/scripts")
set(LOOPIE_SCRIPT_DLL "${OUTPUT_DIR}/LoopieScriptCore.dll")

add_custom_command(
    OUTPUT ${LOOPIE_SCRIPT_DLL}
    COMMAND ${DOTNET_EXECUTABLE} build ${LOOPIE_SCRIPT} -c $<CONFIG> -o "${OUTPUT_DIR}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building LoopieScripting (C#) to Assets/Scripts..."
    VERBATIM
)

add_custom_target(LoopieScriptingBuild ALL DEPENDS ${LOOPIE_SCRIPT_DLL})