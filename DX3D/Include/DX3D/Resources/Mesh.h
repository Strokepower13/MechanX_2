#pragma once
#include <Windows.h>
#include <DX3D/Prerequisites.h>
#include <string>
#include <unordered_map>

struct SubMesh
{
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	INT baseVertexLocation = 0;
};

struct Mesh
{
	std::string name;

	VertexBufferPtr p_vertexBuffer = nullptr;
	IndexBufferPtr p_indexBuffer = nullptr;

	std::unordered_map<std::string, SubMesh> drawArgs;
};

