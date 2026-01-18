#pragma once
#include "Loopie/Resources/Resource.h"
#include <string>

namespace Loopie {

    class ScriptResource : public Resource {
    public:
        ScriptResource(const std::string& path) : Resource(path) {}

        const std::string& GetSourcePath() const { return m_Path; }
        const std::string& GetClassName() const { return m_ClassName; }
        void SetClassName(const std::string& name) { m_ClassName = name; }

        bool IsCompiled() const { return m_IsCompiled; }
        void SetCompiled(bool compiled) { m_IsCompiled = compiled; }

        const std::string& GetLibraryPath() const { return m_LibraryPath; }
        void SetLibraryPath(const std::string& path) { m_LibraryPath = path; }

    private:
        std::string m_ClassName;
        std::string m_LibraryPath;
        bool m_IsCompiled = false;
    };

}