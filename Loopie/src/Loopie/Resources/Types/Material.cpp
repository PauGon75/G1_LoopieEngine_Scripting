#include "Material.h"

#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"

namespace Loopie
{
	Material::Material()
	{
		InitMaterial();
	}

	Material::~Material()
	{
	}

	void Material::InitMaterial()
	{
		m_uniformValues.clear();

		if (!m_shader.GetIsValidShader())
		{
			Log::Error("Cannot initialize material with invalid shader.");
			return;
		}

		ResetMaterial();
	}

	void Material::ResetMaterial()
	{
		if (!m_shader.GetIsValidShader())
		{
			Log::Error("Cannot reset material with invalid shader.");
			return;
		}

		m_uniformValues.clear();
		const auto& uniforms = m_shader.GetUniforms();
		for (const auto& uniform : uniforms)
		{
			m_uniformValues[uniform.id].type = uniform.type;
			m_uniformValues[uniform.id].value = uniform.default;
		}

		Log::Info("Material reset to default values");
	}

	void Material::Apply()
	{
		if (!m_shader.GetIsValidShader())
		{
			Log::Error("Cannot apply material with invalid shader.");
			return;
		}

		m_shader.Bind();

		if (m_texture)
		{
			m_texture->m_tb->Bind();
		}
		else
		{
			Renderer::s_DefaultTexture->m_tb->Bind();
		}

		for (const auto& [name, uniformValue] : m_uniformValues)
		{
			ApplyUniform(name, uniformValue);
		}
	}

	void Material::ApplyUniform(const std::string& name, const UniformValue& uniformValue)
	{
		switch (uniformValue.type)
		{
		case UniformType_float:m_shader.SetUniformFloat(name, std::get<float>(uniformValue.value));		break;
		case UniformType_int:  m_shader.SetUniformInt(name, std::get<int>(uniformValue.value));			break;
		case UniformType_uint: m_shader.SetUniformInt(name, static_cast<int>(std::get<unsigned int>(uniformValue.value)));	break;
		case UniformType_bool: m_shader.SetUniformInt(name, std::get<bool>(uniformValue.value) ? 1 : 0);break;
		case UniformType_vec2: m_shader.SetUniformVec2(name, std::get<vec2>(uniformValue.value));		break;
		case UniformType_vec3: m_shader.SetUniformVec3(name, std::get<vec3>(uniformValue.value));		break;
		case UniformType_vec4: m_shader.SetUniformVec4(name, std::get<vec4>(uniformValue.value));		break;
		case UniformType_mat2: m_shader.SetUniformMat2(name, std::get<matrix2>(uniformValue.value));	break;
		case UniformType_mat3: m_shader.SetUniformMat3(name, std::get<matrix3>(uniformValue.value));	break;
		case UniformType_mat4: m_shader.SetUniformMat4(name, std::get<matrix4>(uniformValue.value));	break;
		case UniformType_Sampler2D: 	break;
		case UniformType_Sampler3D: 	break;
		default: Log::Warn("Unknown uniform type for '{0}'", name);	break;
		}
	}

	UniformValue* Material::GetShaderVariable(const std::string& name)
	{
		auto it = m_uniformValues.find(name);
		if (it != m_uniformValues.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	void Material::SetShader(const Shader& shader)
	{
		if (!shader.GetIsValidShader())
		{
			Log::Error("Cannot set invalid shader to material.");
			return;
		}

		m_shader = shader;
		InitMaterial();
	}

	bool Material::SetShaderVariable(const std::string& name, const UniformValue& value)
	{
		auto it = m_uniformValues.find(name);
		if (it == m_uniformValues.end())
		{
			Log::Warn("Uniform '{0}' not found in material.", name);
			return false;
		}

		if (it->second.type != value.type)
		{
			Log::Error("Type mismatch for uniform '{0}'. Expected type {1}, got type {2}",
				name, static_cast<int>(it->second.type), static_cast<int>(value.type));
			return false;
		}

		it->second = value;
		return true;
	}

	void Material::SetTexture(std::shared_ptr<Texture> texture)
	{
		m_texture = texture;
	}

	void Material::Bind()
	{
		Apply(); 
	}

	void Material::Unbind() const
	{
		m_shader.Unbind();
	}
}