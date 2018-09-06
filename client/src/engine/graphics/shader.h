#ifndef NIXIE_SHADER_H_
#define NIXIE_SHADER_H_

#pragma once

#include "d3d.h"


namespace Nixie
{
	class Shader
	{
	private:

		struct MatrixBuffer
		{
			DirectX::SimpleMath::Matrix world_matrix;
			DirectX::SimpleMath::Matrix view_matrix;
			DirectX::SimpleMath::Matrix projection_matrix;
		};

		struct LightBuffer
		{
			DirectX::SimpleMath::Color diffuse_color;
			DirectX::SimpleMath::Color ambient_color;
			DirectX::SimpleMath::Vector3 direction;
			float padding;
		};

	public:
		Shader();
		~Shader();

		bool Init(std::string vs_path, std::string ps_path);
		bool Update(DirectX::SimpleMath::Matrix world_matrix, ID3D11ShaderResourceView* texture);

	private:
		bool CreateVertexShader(std::vector<unsigned char*> buffer);
		bool CreateInputLayout(std::vector<unsigned char*> buffer);
		bool CreateMatrixBuffer();

		bool CreatePixelShader(std::vector<unsigned char*> buffer);
		bool CreateSamplerState();
		bool CreateLightBuffer();

		std::vector<unsigned char*> LoadFromFile(std::string file_path);

	private:
		std::shared_ptr<ID3D11Device> device_;
		std::shared_ptr<ID3D11DeviceContext> device_context_;

		ID3D11VertexShader* vertex_shader_;
		ID3D11PixelShader* pixel_shader_;
		ID3D11InputLayout* input_layout_;
		ID3D11SamplerState* sampler_state_;

		ID3D11Buffer* matrix_buffer_;
		ID3D11Buffer* light_buffer_;
	};
}

#endif