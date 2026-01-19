# Configurar versión de .NET Framework
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")

# Definir los archivos del proyecto
set(SOURCES
    "Entity.cs"
)

# Crear la librería (DLL)
add_library(LoopieScriptCore SHARED ${SOURCES})

# Configurar dónde guardar la DLL compilada
set_target_properties(LoopieScriptCore PROPERTIES
    VS_DOTNET_REFERENCES "System"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Assets/Scripts/Bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Assets/Scripts/Bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Assets/Scripts/Bin"
)