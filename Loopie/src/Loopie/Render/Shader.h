#pragma once
#include <Loopie/Core/Math.h>

#include <string>
#include <vector>
#include <unordered_map>

typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;

// Things left to work on for this module: 
// - The shader file may have more than the vertex and fragment information. Geometry might
//   be added. In which case, that information should be able to be parsed and compiled as well.
// - Check how the 'file.shader' will be structured. Right now the order has to be 
//   [vertex] and then [fragment]. We will need to change that in the future.
// - Verify if we actually need to cache the attributes, version and uniforms.

namespace Loopie {
	class Shader
	{
	public:
		// Shaders might need geometry information, which would be included in the constructor & Reload
		Shader(const char* sourcePath);
		~Shader();

		void Bind() const;
		void Unbind() const;
		// Constructor and Reload do similar things, might be a good thing to 
		// put them together in a helper function 
		bool Reload(); // Recompiles from the source path currently set

		// Add more if needed: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformMat3(const std::string& name, const Loopie::matrix3& matrix);
		void SetUniformMat4(const std::string& name, const Loopie::matrix4& matrix);
		void SetUniformVec2(const std::string& name, const Loopie::vec2& vector);
		void SetUniformVec3(const std::string& name, const Loopie::vec3& vector);
		void SetUniformVec4(const std::string& name, const Loopie::vec4& vector);

		// Getters
		GLuint GetProgramID() const;
		GLint GetUniformLocation(const std::string& name);
		bool GetIsValidShader() const;
		bool GetIsBound() const;
		// Vertex and Fragment shader version be different, although they will generally match
		// https://stackoverflow.com/questions/30943346/should-vertex-and-fragment-shader-versions-always-match
		// Maybe keeping two is overkill
		const std::string& GetVertexVersion() const;
		const std::string& GetFragmentVersion() const;
		const std::string& GetVertexSource() const;
		const std::string& GetFragmentSource() const;
		const std::string& GetFilePath() const;
		const std::vector<std::string>& GetActiveUniforms() const;
		const std::vector<std::string>& GetActiveAttributes() const;

		// Setters
		void SetPath(const std::string& path);

	private:
		GLuint CompileShader(GLenum shaderType, const char* sourcePath);

		// OpenGL is silent about shaders failing, which is why I created this function
		bool CheckCompileErrors(GLuint shader, const std::string& type);

		// Parser GLSL to Shader variables
		std::string ParseGLSLVersion(const std::string& source);

		// This expects a file with [vertex] first then [fragment]
		bool ParseShaderSourcePath(const std::string& filePath);
		bool CheckIfShaderIsBoundAndWarn();

	private:
		GLuint m_id;
		std::unordered_map<std::string, GLint> m_uniformLocationCache;
		mutable std::vector<std::string> m_activeUniformsCache;
		mutable std::vector<std::string> m_activeAttributesCache;
		mutable bool m_uniformsCached = false;
		mutable bool m_attributesCached = false;

		std::string m_filePath;

		// Unsure if these two are necessary instead of a general version
		std::string m_vertexVersion; 
		std::string m_fragmentVersion;

		// Keeping sources in case we might want to inspect them later
		std::string m_vertexSource;
		std::string m_fragmentSource;

		bool m_isValidShader = true;
	};
}