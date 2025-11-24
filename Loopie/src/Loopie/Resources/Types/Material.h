#pragma once

#include "Loopie/Render/Shader.h"
#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/Types/Texture.h"

#include <variant>

namespace Loopie
{
	struct UniformValue
	{
		UniformType type = UniformType::UniformType_Unknown;
		std::variant<int, float, bool, unsigned int, vec2, vec3, vec4, matrix2, matrix3, matrix4> value;
	};

	class Material : public Resource
	{
	public:
		DEFINE_TYPE(Material)
		Material(const UUID& id);
		~Material();

		void InitMaterial();
		void ResetMaterial();
		void Apply(); // Applies all uniform values to the shader

		// Getters
		Shader& GetShader() { return m_shader; }
		const Shader& GetShader() const { return m_shader; }
		std::shared_ptr<Texture> GetTexture() const { return m_texture; } /// Remove
		UniformValue* GetShaderVariable(const std::string& name);
		const std::unordered_map<std::string, UniformValue>& GetUniforms() const { return m_uniformValues; }

		// Setters
		void SetShader(const Shader& shader);
		bool SetShaderVariable(const std::string& name, const UniformValue& value);
		void SetTexture(std::shared_ptr<Texture> texture); /// Remove

		void Bind();
		void Unbind() const;

		void LoadFromFile(const std::string path) override;
		void Reload() override;

	private:
		void ApplyUniform(const std::string& name, const UniformValue& uniformValue);
		void SaveToFile();
		

	private:
		Shader m_shader = Shader("assets/shaders/DefaultShader.shader");
		std::shared_ptr<Texture> m_texture;
		// The idea behind uniforms is to retrieve them from shader and being able to 
		// adjust them for different textures (maybe we want a variable of type roughness
		// to be different for all different kinds of textures, which can be changed like this)
		std::unordered_map<std::string, UniformValue> m_uniformValues;

		void LoadFromFile(const std::string path) override;
		void Reload() override;
	};
}