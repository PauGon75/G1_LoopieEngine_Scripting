#include "Shader.h"
#include "Loopie/Core/Log.h"

#include <regex>

namespace Loopie {
	// If the shader has errors, it won't be created.
	// If so, the pointer has to be deleted from outside to prevent memory leaks. 
	// Check if it's valid with GetIsValidShader()
	Shader::Shader(const char* vertexPath, const char* fragmentPath)
	{
		// TODO: Read files once we have them
		// m_vertexSource = ReadFile(vertexPath);
		// m_fragmentSource = ReadFile(fragmentPath);
		m_vertexSource = vertexPath;
		m_fragmentSource = fragmentPath;

		ParseShaderSource(m_vertexSource);
		ParseShaderSource(m_fragmentSource);

		m_vertexVersion = ParseGLSLVersion(m_vertexSource);
		m_fragmentVersion = ParseGLSLVersion(m_fragmentSource);

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexPath);
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentPath);

		if (!vertexShader || !fragmentShader)
		{
			Loopie::Log::Critical("Shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return; 
		}

		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);

		if (!CheckCompileErrors(m_ID, "PROGRAM"))
		{
			Loopie::Log::Critical("Shader linking failed. Deleting shader.");
			glDeleteProgram(m_ID);
			m_ID = 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void Shader::SetUniformFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void Shader::SetUniformMat4(const std::string& name, const matrix4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformVec2(const std::string& name, const Loopie::vec2& vector)
	{
		glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vector[0]);
	}

	void Shader::SetUniformVec3(const std::string& name, const vec3& vector)
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vector[0]);
	}

	void Shader::SetUniformVec4(const std::string& name, const Loopie::vec4& vector)
	{
		glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vector[0]);
	}

	const std::vector<ShaderVariable>& Shader::GetParsedVariables() const
	{
		return m_variables;
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
		return m_ID;
	}

	bool Shader::GetIsValidShader() const
	{
		return m_isValidShader;
	}

	void Shader::PrintParsedVariables() const
	{
		Log::Info("=== Parsed Shader Variables ===");
		Log::Info("Vertex version: {0}", m_vertexVersion);
		Log::Info("Fragment version: {0}", m_fragmentVersion);
		for (const auto& var : m_variables)
		{
			Log::Info("{0} {1} {2}", var.qualifier, var.type, var.name);
			if (!var.arraySize.empty())
			{
				Log::Info("{0}", var.arraySize);
			}
		}
		Log::Info("==============================");
	}

	GLuint Shader::CompileShader(GLenum shaderType, const char* source)
	{
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		CheckCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
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
				Loopie::Log::Critical("ERROR - SHADER_COMPILATION_ERROR of type {0}:\n{1}", type, infoLog);
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
				Loopie::Log::Critical("ERROR - PROGRAM_LINKING_ERROR:\n{0}", infoLog);
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

	void Shader::ParseShaderSource(const std::string& source)
	{
		std::regex varRE(
			R"((?:layout\s*\([^)]*\)\s*)?(?:\w+\s+)*(in|out|uniform)\s+(\w+)\s+(\w+)(\[\s*\d+\s*\])?\s*;)",
			std::regex::ECMAScript | std::regex::optimize);

		auto begin = std::sregex_iterator(source.begin(), source.end(), varRE);
		auto end = std::sregex_iterator();

		for (auto it = begin; it != end; ++it)
		{
			ShaderVariable var;
			var.qualifier = (*it)[1].str();
			var.type = (*it)[2].str();
			var.name = (*it)[3].str();
			var.arraySize = (*it)[4].matched ? (*it)[4].str() : "";
			m_variables.push_back(var);
		}
	}
}