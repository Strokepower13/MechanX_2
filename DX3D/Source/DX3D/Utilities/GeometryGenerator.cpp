#include <DX3D/Utilities/GeometryGenerator.h>

GeometryGenerator::MeshData GeometryGenerator::createGrid(float width, float depth, uint32_t m, uint32_t n)
{
	MeshData meshData;

	uint32_t vertexCount = m * n;
	uint32_t faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	meshData.vertices.resize(vertexCount);

	for (uint32_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32_t j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			meshData.vertices[(size_t)i * n + j].position = DirectX::XMFLOAT3(x, 0.0f, z);

			meshData.vertices[(size_t)i * n + j].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.vertices[(size_t)i * n + j].tangentU = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			meshData.vertices[(size_t)i * n + j].texC.x = j * du;
			meshData.vertices[(size_t)i * n + j].texC.y = i * dv;
		}
	}

	meshData.indices32.resize((size_t)faceCount * 3);
	uint32_t k = 0;

	for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			meshData.indices32[k] = i * n + j;
			meshData.indices32[(size_t)k + 1] = i * n + j + 1;
			meshData.indices32[(size_t)k + 2] = (i + 1) * n + j;
			meshData.indices32[(size_t)k + 3] = (i + 1) * n + j;
			meshData.indices32[(size_t)k + 4] = i * n + j + 1;
			meshData.indices32[(size_t)k + 5] = (i + 1) * n + j + 1;
			k += 6;
		}
	}
	
	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
{
	MeshData meshData;
	
	meshData.vertices.clear();
	meshData.indices32.clear();

	float stackHeight = height / stackCount;
	float radiusStep = (topRadius - bottomRadius) / stackCount;
	uint32_t ringCount = stackCount + 1;

	float dTheta = 2.0f * DirectX::XM_PI / sliceCount;
	float dr = bottomRadius - topRadius;

	for (uint32_t i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		for (uint32_t j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.position = DirectX::XMFLOAT3(r * c, y, r * s);

			vertex.texC.x = (float)j / sliceCount;
			vertex.texC.y = 1.0f - (float)i / stackCount;

			vertex.tangentU = DirectX::XMFLOAT3(-s, 0.0f, c);

			DirectX::XMFLOAT3 bitangent(dr * c, -height, dr * s);

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.tangentU);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
			DirectX::XMStoreFloat3(&vertex.normal, N);

			meshData.vertices.push_back(vertex);
		}
	}

	uint32_t ringVertexCount = sliceCount + 1;

	for (uint32_t i = 0; i < stackCount; ++i)
	{
		for (uint32_t j = 0; j < sliceCount; ++j)
		{
			meshData.indices32.push_back(i * ringVertexCount + j);
			meshData.indices32.push_back((i + 1) * ringVertexCount + j);
			meshData.indices32.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.indices32.push_back(i * ringVertexCount + j);
			meshData.indices32.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.indices32.push_back(i * ringVertexCount + j + 1);
		}
	}

	buildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	buildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);

	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::createSphere(float radius, uint32_t sliceCount, uint32_t stackCount)
{
	MeshData meshData;
	
	meshData.vertices.clear();
	meshData.indices32.clear();

	// Compute the vertices stating at the top pole and moving down the stacks.

	Vertex topVertex(
		0.0f, +radius, 0.0f,
		0.0f, +1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	Vertex bottomVertex(
		0.0f, -radius, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	meshData.vertices.push_back(topVertex);

	float phiStep = DirectX::XM_PI / stackCount;
	float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (uint32_t i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint32_t j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// spherical to cartesian
			v.position.x = radius * sinf(phi) * cosf(theta);
			v.position.y = radius * cosf(phi);
			v.position.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.tangentU.x = -radius * sinf(phi) * sinf(theta);
			v.tangentU.y = 0.0f;
			v.tangentU.z = +radius * sinf(phi) * cosf(theta);

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&v.tangentU);
			DirectX::XMStoreFloat3(&v.tangentU, DirectX::XMVector3Normalize(T));

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(P));

			v.texC.x = theta / DirectX::XM_2PI;
			v.texC.y = phi / DirectX::XM_PI;

			meshData.vertices.push_back(v);
		}
	}

	meshData.vertices.push_back(bottomVertex);

	// Compute indices32 for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.

	for (uint32_t i = 1; i <= sliceCount; ++i)
	{
		meshData.indices32.push_back(0);
		meshData.indices32.push_back(i + 1);
		meshData.indices32.push_back(i);
	}

	// Compute indices32 for inner stacks (not connected to poles).

	// Offset the indices32 to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.

	uint32_t baseIndex = 1;
	uint32_t ringVertexCount = sliceCount + 1;

	for (uint32_t i = 0; i < stackCount - 2; ++i)
	{
		for (uint32_t j = 0; j < sliceCount; ++j)
		{
			meshData.indices32.push_back(baseIndex + i * ringVertexCount + j);
			meshData.indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// Compute indices32 for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.

	// South pole vertex was added last.
	uint32_t southPoleIndex = (UINT)meshData.vertices.size() - 1;

	// Offset the indices32 to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < sliceCount; ++i)
	{
		meshData.indices32.push_back(southPoleIndex);
		meshData.indices32.push_back(baseIndex + i);
		meshData.indices32.push_back(baseIndex + i + 1);
	}

	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::createGeosphere(float radius, uint32_t numSubdivisions)
{
	MeshData meshData;
	
	numSubdivisions = min(numSubdivisions, 5u);

	const float X = 0.525731f;
	const float Z = 0.850651f;

	DirectX::XMFLOAT3 pos[12] =
	{
		DirectX::XMFLOAT3(-X, 0.0f, Z),  DirectX::XMFLOAT3(X, 0.0f, Z),
		DirectX::XMFLOAT3(-X, 0.0f, -Z), DirectX::XMFLOAT3(X, 0.0f, -Z),
		DirectX::XMFLOAT3(0.0f, Z, X),   DirectX::XMFLOAT3(0.0f, Z, -X),
		DirectX::XMFLOAT3(0.0f, -Z, X),  DirectX::XMFLOAT3(0.0f, -Z, -X),
		DirectX::XMFLOAT3(Z, X, 0.0f),   DirectX::XMFLOAT3(-Z, X, 0.0f),
		DirectX::XMFLOAT3(Z, -X, 0.0f),  DirectX::XMFLOAT3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,		4,9,0,		4,5,9,		8,5,4,		1,8,4,
		1,10,8,		10,3,8,		8,3,5,		3,2,5,		3,7,2,
		3,10,7,		10,6,7,		6,11,7,		6,0,11,		6,1,0,
		10,1,6,		11,0,9,		2,11,9,		5,2,9,		11,2,7
	};

	meshData.vertices.resize(12);
	meshData.indices32.resize(60);

	for (size_t i = 0; i < 12; ++i)
		meshData.vertices[i].position = pos[i];

	for (size_t i = 0; i < 60; ++i)
		meshData.indices32[i] = k[i];

	for (size_t i = 0; i < numSubdivisions; ++i)
		subdivide(meshData);

	for (size_t i = 0; i < meshData.vertices.size(); ++i)
	{
		using DirectX::operator*;
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&meshData.vertices[i].position));

		DirectX::XMVECTOR p = radius * n;

		DirectX::XMStoreFloat3(&meshData.vertices[i].position, p);
		DirectX::XMStoreFloat3(&meshData.vertices[i].normal, n);

		float theta = findAngle(meshData.vertices[i].position.x, meshData.vertices[i].position.z);

		float phi = acosf(meshData.vertices[i].position.y / radius);

		meshData.vertices[i].texC.x = theta / DirectX::XM_2PI;
		meshData.vertices[i].texC.y = phi / DirectX::XM_PI;

		meshData.vertices[i].tangentU.x = -radius * sinf(phi) * sinf(theta);
		meshData.vertices[i].tangentU.y = 0.0f;
		meshData.vertices[i].tangentU.z = radius * sinf(phi) * cosf(theta);

		DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&meshData.vertices[i].tangentU);
		DirectX::XMStoreFloat3(&meshData.vertices[i].tangentU, DirectX::XMVector3Normalize(T));
	}

	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::createBox(float width, float height, float depth, uint32_t numSubdivisions)
{
	MeshData meshData;
	
	// Create the vertices.

	Vertex v[24];

	float w = 0.5f * width;
	float h = 0.5f * height;
	float d = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w, +h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w, +h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w, -h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w, -h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w, +h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w, +h, d, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w, h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w, h, +d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(w, h, +d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(w, h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w, -h, -d, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w, -h, -d, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w, -h, +d, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w, -h, +d, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w, -h, +d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w, +h, +d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w, +h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w, -h, -d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(w, -h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(w, +h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(w, +h, +d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(w, -h, +d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.vertices.assign(&v[0], &v[24]);

	// Create the indices32.

	uint32_t i[36]{};

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.indices32.assign(&i[0], &i[36]);

	numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);

	for (uint32_t i = 0; i < numSubdivisions; ++i)
		subdivide(meshData);

	return meshData;
}

void GeometryGenerator::buildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData)
{
	uint32_t baseIndex = (UINT)meshData.vertices.size();

	float y = 0.5f * height;
	float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

	for (uint32_t i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back(
			Vertex(x, y, z,
				0.0f, 1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				u, v));
	}

	//Cap center vertex
	meshData.vertices.push_back(
		Vertex(0.0f, y, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.5f, 0.5f));

	uint32_t centerIndex = (UINT)meshData.vertices.size() - 1;

	for (uint32_t i = 0; i < sliceCount; ++i)
	{
		meshData.indices32.push_back(centerIndex);
		meshData.indices32.push_back(baseIndex + i + 1);
		meshData.indices32.push_back(baseIndex + i);
	}
}

void GeometryGenerator::buildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData)
{
	uint32_t baseIndex = (UINT)meshData.vertices.size();

	float y = -0.5f * height;
	float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

	for (uint32_t i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back(
			Vertex(x, y, z,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				u, v));
	}

	//Cap center vertex
	meshData.vertices.push_back(
		Vertex(0.0f, y, 0.0f,
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.5f, 0.5f));

	uint32_t centerIndex = (UINT)meshData.vertices.size() - 1;

	for (uint32_t i = 0; i < sliceCount; ++i)
	{
		meshData.indices32.push_back(centerIndex);
		meshData.indices32.push_back(baseIndex + i);
		meshData.indices32.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::subdivide(MeshData& meshData)
{
	// Save a copy of the input geometry.
	MeshData inputCopy = meshData;


	meshData.vertices.resize(0);
	meshData.indices32.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint32_t numTris = (UINT)inputCopy.indices32.size() / 3;
	for (uint32_t i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.vertices[inputCopy.indices32[i * 3 + 0]];
		Vertex v1 = inputCopy.vertices[inputCopy.indices32[i * 3 + 1]];
		Vertex v2 = inputCopy.vertices[inputCopy.indices32[i * 3 + 2]];

		// Generate the midpoints.

		Vertex m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.position = DirectX::XMFLOAT3(
			0.5f * (v0.position.x + v1.position.x),
			0.5f * (v0.position.y + v1.position.y),
			0.5f * (v0.position.z + v1.position.z));

		m1.position = DirectX::XMFLOAT3(
			0.5f * (v1.position.x + v2.position.x),
			0.5f * (v1.position.y + v2.position.y),
			0.5f * (v1.position.z + v2.position.z));

		m2.position = DirectX::XMFLOAT3(
			0.5f * (v0.position.x + v2.position.x),
			0.5f * (v0.position.y + v2.position.y),
			0.5f * (v0.position.z + v2.position.z));

		// Add new geometry.

		meshData.vertices.push_back(v0); // 0
		meshData.vertices.push_back(v1); // 1
		meshData.vertices.push_back(v2); // 2
		meshData.vertices.push_back(m0); // 3
		meshData.vertices.push_back(m1); // 4
		meshData.vertices.push_back(m2); // 5

		meshData.indices32.push_back(i * 6 + 0);
		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 5);

		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 4);
		meshData.indices32.push_back(i * 6 + 5);

		meshData.indices32.push_back(i * 6 + 5);
		meshData.indices32.push_back(i * 6 + 4);
		meshData.indices32.push_back(i * 6 + 2);

		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 1);
		meshData.indices32.push_back(i * 6 + 4);
	}
}

float GeometryGenerator::findAngle(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * DirectX::XM_PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + DirectX::XM_PI; // in [0, 2*pi).

	return theta;
}
