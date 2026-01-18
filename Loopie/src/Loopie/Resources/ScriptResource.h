#pragma once

#include "Loopie/Resources/Resource.h"
#include <string>
#include <filesystem>

namespace Loopie {

    class ScriptResource : public Resource {
    public:
        // Corregido: Ahora pasa el UUID y el ResourceType a la clase base
        ScriptResource(UUID uuid, const std::string& path)
            : Resource(uuid, ResourceType::UNKNOWN), m_Path(path)
        {
            if (std::filesystem::exists(path)) {
                m_LastWriteTime = std::filesystem::last_write_time(path);
            }
            else {
                m_LastWriteTime = std::filesystem::file_time_type::min();
            }
        }

        // Implementacion obligatoria de la funcion virtual pura de Resource
        bool Load() override {
            return true;
        }

        // Getters y Setters para el Pipeline de Compilacion
        const std::string& GetSourcePath() const { return m_Path; }

        const std::string& GetClassName() const { return m_ClassName; }
        void SetClassName(const std::string& name) { m_ClassName = name; }

        bool IsCompiled() const { return m_IsCompiled; }
        void SetCompiled(bool compiled) { m_IsCompiled = compiled; }

        const std::string& GetLibraryPath() const { return m_LibraryPath; }
        void SetLibraryPath(const std::string& path) { m_LibraryPath = path; }

        // Getters/Setters para el Asset Watcher
        std::filesystem::file_time_type GetLastWriteTime() const { return m_LastWriteTime; }
        void SetLastWriteTime(std::filesystem::file_time_type time) { m_LastWriteTime = time; }

        bool NeedsRecompile(std::filesystem::file_time_type currentWriteTime) const {
            return currentWriteTime > m_LastWriteTime;
        }

    private:
        std::string m_Path;
        std::string m_ClassName;
        std::string m_LibraryPath;
        bool m_IsCompiled = false;
        std::filesystem::file_time_type m_LastWriteTime;
    };

}