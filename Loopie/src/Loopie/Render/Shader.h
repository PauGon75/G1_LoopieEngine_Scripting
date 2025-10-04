#include <glad/glad.h>
#include <Loopie/Core/Math.h>

#include <string>
#include <vector>

namespace Loopie {
	struct ShaderVariable
	{
		std::string qualifier;	// in / out / uniform
		std::string type;		// vec3, vec4, etc.
		std::string name;		// variable name
		std::string arraySize;	// might be [16] or empty
	};

	class Shader
	{
	public:
		// Shaders might need geometry information, which would be included in the constructor
		Shader(const char* vertexPath, const char* fragmentPath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		// TODO - Confirm IF binding should be included in these methods too
		// TODO - Confirm if these will be tried to be called more than once in update loops
		// Uniform Setters - Useful to pass data to shaders
		// More can be created as necessary. They depend on this:
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformMat4(const std::string& name, const Loopie::matrix4& matrix);
		void SetUniformVec2(const std::string& name, const Loopie::vec2& vector);
		void SetUniformVec3(const std::string& name, const Loopie::vec3& vector);
		void SetUniformVec4(const std::string& name, const Loopie::vec4& vector);

		// Getters
		GLuint GetProgramID() const;
		const std::vector<ShaderVariable>& GetParsedVariables() const;
		// Vertex and Fragment shader version be different, although they will generally match
		// https://stackoverflow.com/questions/30943346/should-vertex-and-fragment-shader-versions-always-match
		const std::string& GetVertexVersion() const;
		const std::string& GetFragmentVersion() const;
		bool GetIsValidShader() const;

		// Debug print
		void PrintParsedVariables() const;

	private:
		GLuint CompileShader(GLenum shaderType, const char* sourcePath);

		// OpenGL is silent about shaders failing, which is why I created this function
		bool CheckCompileErrors(GLuint shader, const std::string& type);

		// Parser GLSL to Shader variables
		std::string ParseGLSLVersion(const std::string& source);
		void ParseShaderSource(const std::string& source);

	private:
		GLuint m_ID;
		std::vector<ShaderVariable> m_variables;

		// Unsure if these two are necessary instead of a general version
		std::string m_vertexVersion; 
		std::string m_fragmentVersion;

		// Keeping sources in case we might want to inspect them later
		std::string m_vertexSource;
		std::string m_fragmentSource;

		bool m_isValidShader = true;
	};
}