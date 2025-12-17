#pragma once
#include "model.h"

class Item_Ins_Base
{
public:
	Object_Instance model;
	std::string name;
	std::vector<AABB> world_hitboxs;
	Matrix model_adjust;

	void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos, 
		Texture_Manager* textures, std::string model_name, std::string matrix_file_name, bool if_VS_ani)
	{
		model_adjust = model_adjust.mul(Matrix::rotateY(M_PI / 2));
		
		//create_matixes(model.instances_matix, Vec3(0, 0, 0), 1000.0f, 1000.0f, 100, 120);
		//model.instances_matix = generateFenceRectangle(1500.0f, 1500.0f, 100.0f, Vec3(100, 100, 100), model_adjust);
		//save_instance_matrices(FILE_NAME_FENCE_MATRIX, model.instances_matix);
		load_instance_matrices(matrix_file_name, model.instances_matix);

		model.init(core, shader_manager, psos, textures, model_name, if_VS_ani);
		model.init_hitbox(core, shader_manager, psos, true);

		for (unsigned int i = 0; i < model.instances_matix.size(); i++)
		{
			//model.instances_matix[i].w = model.instances_matix[i].w.mul(model_adjust);
			AABB world = model.hitbox.local_aabb;
			world_hitboxs.push_back(world.transform(model.instances_matix[i].w));
		}

	}

	bool collide(const AABB& aabb) const
	{
		for (const auto& w : world_hitboxs)
		{
			if (aabb.intersects_toother(w))
				return true;
		}
		return false;
	}

	static bool collide_with_map(const AABB& testBox,
		const std::vector<Item_Ins_Base*>& items)
	{
		for (auto item : items)
		{
			for (const AABB& test : item->world_hitboxs)
			{
				if (testBox.intersects_toother(test))
					return true;
			}
		}
		return false;
	}


	void draw(Core* core, Matrix& vp)
	{


		model.draw(core, vp);

		for (unsigned int i = 0; i < world_hitboxs.size(); i++)
		{
			model.hitbox.draw(core, model.instances_matix[i].w, vp);
		}
	}
};
