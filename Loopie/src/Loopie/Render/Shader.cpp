#include "Shader.h"
#include "Loopie/Core/Log.h"

#include <regex>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

namespace Loopie {
	// If shader compilation/linking fails, GetIsValidShader() will return false.
	// The object is still valid but the shader program may not be usable.
	// SourcePath should have first [vertex] and then [fragment]
	Shader::Shader(const char* sourcePath)
	{
		m_filePath = sourcePath;

		if (!ParseShaderSourcePath(m_filePath))
		{
			Loopie::Log::Critical("Shader file parsing failed. Aborting shader compilation.");
			return;
		}

		m_vertexVersion = ParseGLSLVersion(m_vertexSource);
		m_fragmentVersion = ParseGLSLVersion(m_fragmentSource);

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, m_vertexSource.c_str());
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, m_fragmentSource.c_str());

		if (!CheckCompileErrors(vertexShader, "VERTEX"))
		{
			Loopie::Log::Critical("Vertex shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return;
		}

		if (!CheckCompileErrors(fragmentShader, "FRAGMENT"))
		{
			Loopie::Log::Critical("Fragment shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return;
		}

		m_id = glCreateProgram();
		glAttachShader(m_id, vertexShader);
		glAttachShader(m_id, fragmentShader);
		glLinkProgram(m_id);

		if (!CheckCompileErrors(m_id, "PROGRAM"))
		{
			Loopie::Log::Critical("Shader linking failed. Deleting shader.");
			glDeleteProgram(m_id);
			m_id = 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_id);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_id);
	}

	void Shader::Unbind() const
	{	
		glUseProgram(0);
	}

	bool Shader::Reload()
	{
		m_isValidShader = true; // resetting the state so it can fail again if necessary

		if (!ParseShaderSourcePath(m_filePath))
		{
			Loopie::Log::Critical("Shader file parsing failed. Aborting shader compilation.");
			return false;
		}

		m_vertexVersion = ParseGLSLVersion(m_vertexSource);
		m_fragmentVersion = ParseGLSLVersion(m_fragmentSource);

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, m_vertexSource.c_str());
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, m_fragmentSource.c_str());

		if (!CheckCompileErrors(vertexShader, "VERTEX"))
		{
			Loopie::Log::Critical("Vertex shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;
		}

		if (!CheckCompileErrors(fragmentShader, "FRAGMENT"))
		{
			Loopie::Log::Critical("Fragment shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;
		}

		// Create new program
		GLuint newProgram = glCreateProgram();
		glAttachShader(newProgram, vertexShader);
		glAttachShader(newProgram, fragmentShader);
		glLinkProgram(newProgram);

		if (!CheckCompileErrors(newProgram, "PROGRAM"))
		{
			Loopie::Log::Critical("Shader linking failed. Keeping old shader.");
			glDeleteProgram(newProgram);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			m_isValidShader = false;
			return false;
		}

		// Delete old program and swap them
		glDeleteProgram(m_id);
		m_id = newProgram;

		// Clear cache since uniform locations may have changed
		m_uniformLocationCache.clear();
		m_activeUniformsCache.clear();
		m_activeAttributesCache.clear();
		m_uniformsCached = false;
		m_attributesCached = false;

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return true;
	}

	void Shader::SetUniformInt(const std::string& name, int value)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform1i(location, value);
	}

	void Shader::SetUniformFloat(const std::string& name, float value)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform1f(location, value);
	}

	void Shader::SetUniformMat3(const std::string& name, const Loopie::matrix3& matrix)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformMat4(const std::string& name, const matrix4& matrix)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformVec2(const std::string& name, const Loopie::vec2& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform2fv(location, 1, &vector[0]);
	}

	void Shader::SetUniformVec3(const std::string& name, const vec3& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform3fv(location, 1, &vector[0]);
	}

	void Shader::SetUniformVec4(const std::string& name, const Loopie::vec4& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform4fv(location, 1, &vector[0]);
	}

	const std::string& Shader::GetVertexVersion() const
	{
		return m_vertexVersion;
	}

	const std::string& Shader::GetFragmentVersion() const
	{
		return m_fragmentVersion;
	}

	GLuint Shader::GetProgramID() const
	{
		return m_id;
	}

	GLint Shader::GetUniformLocation(const std::string& name)
	{
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		{
			return m_uniformLocationCache[name];
		}

		GLint location = glGetUniformLocation(m_id, name.c_str());
		m_uniformLocationCache[name] = location;
		return location;
	}

	bool Shader::GetIsValidShader() const
	{
		return m_isValidShader;
	}

	const std::string& Shader::GetVertexSource() const
	{ 
		return m_vertexSource; 
	}

	const std::string& Shader::GetFragmentSource() const
	{ 
		return m_fragmentSource;
	}

	const std::string& Shader::GetFilePath() const
	{ 
		return m_filePath; 
	}

	bool Shader::GetIsBound() const
	{
		GLint currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		return currentProgram == static_cast<GLint>(m_id);
	}

	const std::vector<std::string>& Shader::GetActiveUniforms() const
	{
		if (m_uniformsCached)
			return m_activeUniformsCache;

		m_activeUniformsCache.clear();

		GLint count;
		glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &count);

		for (GLint i = 0; i < count; i++)
		{
			GLchar name[256];
			GLsizei length;
			GLint size;
			GLenum type;

			glGetActiveUniform(m_id, i, sizeof(name), &length, &size, &type, name);
			m_activeUniformsCache.push_back(std::string(name));
		}

		m_uniformsCached = true;
		return m_activeUniformsCache;
	}

	const std::vector<std::string>& Shader::GetActiveAttributes() const
	{
		if (m_attributesCached)
			return m_activeAttributesCache;

		m_activeAttributesCache.clear();

		GLint count;
		glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &count);

		for (GLint i = 0; i < count; i++)
		{
			GLchar name[256];
			GLsizei length;
			GLint size;
			GLenum type;

			glGetActiveAttrib(m_id, i, sizeof(name), &length, &size, &type, name);
			m_activeAttributesCache.push_back(std::string(name));
		}

		m_attributesCached = true;
		return m_activeAttributesCache;
	}

	void Shader::SetPath(const std::string& path)
	{
		m_filePath = path;
	}

	GLuint Shader::CompileShader(GLenum shaderType, const char* source)
	{
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		return shader;
	}

	bool Shader::CheckCompileErrors(GLuint shader, const std::string& type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				Loopie::Log::Error("ERROR - SHADER_COMPILATION_ERROR of type {0}:\n{1}", type, infoLog);
				m_isValidShader = false;
				return false;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				Loopie::Log::Error("ERROR - PROGRAM_LINKING_ERROR of type {0}:\n{1}", type, infoLog);
				m_isValidShader = false;
				return false;
			}
		}
		return true;
	}

	std::string Shader::ParseGLSLVersion(const std::string& source)
	{
		// Regex for: "#version 460 core" or "#version 450"
		std::regex versionRE(R"(#\s*version\s+([0-9]+)(?:\s+(\w+))?)");
		std::smatch match;
		if (std::regex_search(source, match, versionRE))
		{
			std::string version = match[1].str();
			if (match.size() > 2 && match[2].matched)
				version += " " + match[2].str();
			return version;
		}
		return "unknown";
	}

	bool Shader::ParseShaderSourcePath(const std::string& filePath)
	{
		// Read the entire file
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			Log::Error("ERROR - SHADER PARSER - File not found: {0}", filePath);
			m_isValidShader = false;
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		file.close();

		// Find [vertex] and [fragment] markers
		size_t vertexPos = content.find("[vertex]");
		size_t fragmentPos = content.find("[fragment]");

		if (vertexPos == std::string::npos || fragmentPos == std::string::npos)
		{
			Log::Error("ERROR - SHADER PARSER - Could not find [vertex] or [fragment] markers in {0}", filePath);
			m_isValidShader = false;
			return false;
		}

		// Extract vertex shader (between [vertex] and [fragment])
		size_t vertexStart = vertexPos + 8; // Skip "[vertex]"
		m_vertexSource = content.substr(vertexStart, fragmentPos - vertexStart);

		// Extract fragment shader (after [fragment] to end)
		size_t fragmentStart = fragmentPos + 10; // Skip "[fragment]"
		m_fragmentSource = content.substr(fragmentStart);

		// Trim whitespace
		auto trim = [](std::string& s)
			{
				size_t start = s.find_first_not_of(" \t\n\r");
				if (start == std::string::npos) {
					s.clear();
					return;
				}
				size_t end = s.find_last_not_of(" \t\n\r");
				s = s.substr(start, end - start + 1);
			};

		trim(m_vertexSource);
		trim(m_fragmentSource);

		m_isValidShader = !m_vertexSource.empty() && !m_fragmentSource.empty();
		return true;
	}

	bool Shader::CheckIfShaderIsBoundAndWarn() 
	{
		bool isBound = GetIsBound();
		if (!isBound)
		{
			Log::Warn("Shader is not bound. Cancelling operation.");
		}
		return isBound;
	}
}