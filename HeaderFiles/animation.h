#pragma once
#include <map>
#include <vector>
#include <string>
#include "core.h"
#include "vectors.h"

struct Bone
{
	std::string name;
	Matrix offset;
	int parentIndex;
};

struct Skeleton
{
	std::vector<Bone> bones;
	Matrix globalInverse;
	int findBone(std::string name)
	{
		for (int i = 0; i < bones.size(); i++)
		{
			if (bones[i].name == name)
			{
				return i;
			}
		}
		return -1;
	}
};

struct AnimationFrame
{
	std::vector<Vec3> positions;
	std::vector<Quaternion> rotations;
	std::vector<Vec3> scales;
};

