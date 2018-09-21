#include "../stdafx_client.h"
#include "model.h"

#include "camera.h"
#include "core/mesh_loader.h"
#include "core/graphics/renderer.h"


namespace nixie
{
	Model::Model(std::string mesh_path, std::string vs_path, std::string ps_path, std::string texture_path) :
		mesh_path_(mesh_path),
		vs_path_(vs_path),
		ps_path_(ps_path),
		texture_path_(texture_path) {}


	bool Model::OnInit()
	{
		mesh_ = MeshLoader::Get().Load(mesh_path_);
		if (!mesh_->CreateBuffers())
		{
			std::cerr << "Error: Failed to initialize mesh" << std::endl;
			return false;
		}

		material_ = std::make_shared<Material>();
		if (!material_->Init(vs_path_, ps_path_, texture_path_, true))
		{
			std::cerr << "Error: Failed to initialize material" << std::endl;
			return false;
		}

		return true;
	}


	bool Model::OnUpdate()
	{
		if (!material_->Update(
			GetTransform()->CalculateWorldMatrix(),
			GetScene()->GetCamera()->GetViewMatrix(),
			GetGameObject()->GetScene()->GetCamera()->GetProjectionMatrix()))
		{
			return false;
		}

		Renderer::Get().RenderMesh(mesh_);

		return true;
	}
}