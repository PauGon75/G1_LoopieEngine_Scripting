cmake_minimum_required(VERSION 3.10)
project("LoopieScripting" NONE)

find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

set(LOOPIE_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/LoopieScriptProject.csproj
)

set(LOOPIE_SCRIPTING_OUTPUT
    ${CMAKE_BINARY_DIR}/Loopie.Core.dll
)

add_custom_command(
    OUTPUT ${LOOPIE_SCRIPTING_OUTPUT}
    COMMAND ${DOTNET_EXECUTABLE} build
            ${LOOPIE_SCRIPT}
            -c $<CONFIG>
            -o ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building LoopieScripting (C#)"
)

add_custom_target(LoopieScriptingBuild ALL
    DEPENDS ${LOOPIE_SCRIPTING_OUTPUT}
)