#include "../../../stdafx.h"

#include "model.h"
#include "../../graphics/mesh.h"
#include "../../graphics/material.h"


namespace Nixie
{
	Model::Model(std::string mesh_path, std::string vs_path, std::string ps_path, std::string texture_path) :
		mesh_path_(mesh_path),
		vs_path_(vs_path),
		ps_path_(ps_path),
		texture_path_(texture_path) {}


	void Model::OnInit()
	{
		mesh_ = std::make_shared<Mesh>();
		if (!mesh_->Init(mesh_path_))
		{
			std::cerr << "Error: Failed to initialize mesh" << std::endl;
		}

		material_ = std::make_shared<Material>();
		if (!material_->Init(vs_path_, ps_path_, texture_path_))
		{
			std::cerr << "Error: Failed to initialize material" << std::endl;
		}
	}


	void Model::OnUpdate()
	{
		material_->Update(GetTransform()->CalculateWorldMatrix());
		mesh_->Render();
	}
}