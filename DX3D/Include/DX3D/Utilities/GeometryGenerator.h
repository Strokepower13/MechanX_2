#pragma once
#include <DirectXMath.h>
#include <vector>
#include <Windows.h>

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() :position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tangentU(0.0f, 0.0f, 0.0f), texC(0.0f, 0.0f) {}
		Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv) :position(p), normal(n), tangentU(t), texC(uv) {}
		Vertex(float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :position(px, py, pz), normal(nx, ny, nz), tangentU(tx, ty, tz), texC(u, v) {}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangentU;
		DirectX::XMFLOAT2 texC;
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices32;

		std::vector<uint16_t>& getIndices16()
		{
			if (p_indices16.empty())
			{
				p_indices16.resize(indices32.size());
				for (size_t i = 0; i < indices32.size(); ++i)
					p_indices16[i] = static_cast<uint16_t>(indices32[i]);
			}

			return p_indices16;
		}

	private:
		std::vector<uint16_t> p_indices16;
	};

	MeshData createGrid(float width, float depth, uint32_t m, uint32_t n);
	MeshData createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);
	MeshData createSphere(float radius, uint32_t sliceCount, uint32_t stackCount);
	MeshData createGeosphere(float radius, uint32_t numSubdivisions);
	MeshData createBox(float width, float height, float depth, uint32_t numSubdivisions);

private:
	void buildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
	void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
	void subdivide(MeshData& meshData);
	float findAngle(float x, float y);
};

