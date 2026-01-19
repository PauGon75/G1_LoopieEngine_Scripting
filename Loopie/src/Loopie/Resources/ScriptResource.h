#pragma once
#include "Loopie/Resources/Resource.h"
#include <string>
#include <filesystem>

namespace Loopie {

    class ScriptResource : public Resource {
    public:
        // Constructor: Pasa el UUID y el Tipo al padre
        ScriptResource(UUID uuid, const std::string& path)
            : Resource(uuid, ResourceType::SCRIPT), m_path(path) {
            if (std::filesystem::exists(path)) {
                m_lastWriteTime = std::filesystem::last_write_time(path);
            }
        }

        // Destructor virtual: Muy importante para evitar fugas y errores de vtable
        virtual ~ScriptResource() override = default;

        // ESTA LINEA ES LA QUE ARREGLA std::make_shared
        // Debe ser EXACTAMENTE "bool Load() override"
        bool Load() override {
            return true;
        }

        // Getters y Setters
        const std::string& GetSourcePath() const { return m_path; }
        const std::string& GetClassName() const { return m_className; }
        void SetClassName(const std::string& name) { m_className = name; }

        bool IsCompiled() const { return m_isCompiled; }
        void SetCompiled(bool compiled) { m_isCompiled = compiled; }

        const std::string& GetLibraryPath() const { return m_libraryPath; }
        void SetLibraryPath(const std::string& path) { m_libraryPath = path; }

        std::filesystem::file_time_type GetLastWriteTime() const { return m_lastWriteTime; }
        void SetLastWriteTime(std::filesystem::file_time_type time) { m_lastWriteTime = time; }

        bool NeedsRecompile(std::filesystem::file_time_type currentWriteTime) const {
            return currentWriteTime > m_lastWriteTime;
        }

    private:
        std::string m_path;
        std::string m_className;
        std::string m_libraryPath;
        bool m_isCompiled = false;
        std::filesystem::file_time_type m_lastWriteTime;
    };
}