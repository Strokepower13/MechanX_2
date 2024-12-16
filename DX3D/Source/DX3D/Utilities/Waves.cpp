#include <DX3D/Utilities/Waves.h>
#include <ppl.h>
#include <algorithm>
#include <cassert>

Waves::Waves(int m, int n, float dx, float dt, float speed, float damping)
{
    p_numRows = m;
    p_numCols = n;
    
    p_vertexCount = m * n;
    p_triangleCount = (m - 1) * (n - 1) * 2;
    
    p_timeStep = dt;
    p_spatialStep = dx;
    
    float d = damping * dt + 2.0f;
    float e = (speed * speed) * (dt * dt) / (dx * dx);
    p_k1 = (damping * dt - 2.0f) / d;
    p_k2 = (4.0f - 8.0f * e) / d;
    p_k3 = (2.0f * e) / d;
    
    p_prevSolution.resize((size_t)m * n);
    p_currSolution.resize((size_t)m * n);
    p_normals.resize((size_t)m * n);
    p_tangentX.resize((size_t)m * n);
    
    float halfWidth = (n - 1) * dx * 0.5f;
    float halfDepth = (m - 1) * dx * 0.5f;
    for (int i = 0; i < m; ++i)
    {
        float z = halfDepth - i * dx;
        for (int j = 0; j < n; ++j)
        {
            float x = -halfWidth + j * dx;
            
            p_prevSolution[(size_t)i * n + j] = DirectX::XMFLOAT3(x, 0.0f, z);
            p_currSolution[(size_t)i * n + j] = DirectX::XMFLOAT3(x, 0.0f, z);
            p_normals[(size_t)i * n + j] = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
            p_tangentX[(size_t)i * n + j] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
        }
    }
}

Waves::~Waves()
{
}

int Waves::rowCount() const
{
    return p_numRows;
}

int Waves::columnCount() const
{
    return p_numCols;
}

int Waves::vertexCount() const
{
    return p_vertexCount;
}

int Waves::triangleCount() const
{
    return p_triangleCount;
}

float Waves::width() const
{
    return p_numCols * p_spatialStep;
}

float Waves::depth() const
{
    return p_numRows * p_spatialStep;
}

void Waves::update(float dt)
{
	static float t = 0;

	t += dt;

	if (t >= p_timeStep)
	{
		concurrency::parallel_for(1, p_numRows - 1, [this](int i)
		//for(int i = 1; i < mNumRows-1; ++i)
		{
			for (int j = 1; j < p_numCols - 1; ++j)
			{
				p_prevSolution[(size_t)i * p_numCols + j].y =
					p_k1 * p_prevSolution[(size_t)i * p_numCols + j].y +
					p_k2 * p_currSolution[(size_t)i * p_numCols + j].y +
					p_k3 * (p_currSolution[(size_t)(i + 1) * p_numCols + j].y +
						p_currSolution[(size_t)(i - 1) * p_numCols + j].y +
						p_currSolution[(size_t)i * p_numCols + j + 1].y +
						p_currSolution[(size_t)i * p_numCols + j - 1].y);
			}
		});

		std::swap(p_prevSolution, p_currSolution);

		t = 0.0f;

        concurrency::parallel_for(1, p_numRows - 1, [this](int i)
        //for(int i = 1; i < mNumRows - 1; ++i)
        {
            for (int j = 1; j < p_numCols - 1; ++j)
            {
                float l = p_currSolution[(size_t)i * p_numCols + j - 1].y;
                float r = p_currSolution[(size_t)i * p_numCols + j + 1].y;
                float t = p_currSolution[(size_t)(i - 1) * p_numCols + j].y;
                float b = p_currSolution[(size_t)(i + 1) * p_numCols + j].y;
                p_normals[(size_t)i * p_numCols + j].x = -r + l;
                p_normals[(size_t)i * p_numCols + j].y = 2.0f * p_spatialStep;
                p_normals[(size_t)i * p_numCols + j].z = b - t;

                DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&p_normals[(size_t)i * p_numCols + j]));
                DirectX::XMStoreFloat3(&p_normals[(size_t)i * p_numCols + j], n);

                p_tangentX[(size_t)i * p_numCols + j] = DirectX::XMFLOAT3(2.0f * p_spatialStep, r - l, 0.0f);
                DirectX::XMVECTOR T = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&p_tangentX[(size_t)i * p_numCols + j]));
                DirectX::XMStoreFloat3(&p_tangentX[(size_t)i * p_numCols + j], T);
            }
        });
	}
}

void Waves::disturb(int i, int j, float magnitude)
{
    assert(i > 1 && i < p_numRows - 2);
    assert(j > 1 && j < p_numCols - 2);

    float halfMag = 0.5f * magnitude;

    p_currSolution[(size_t)i * p_numCols + j].y += magnitude;
    p_currSolution[(size_t)i * p_numCols + j + 1].y += halfMag;
    p_currSolution[(size_t)i * p_numCols + j - 1].y += halfMag;
    p_currSolution[(size_t)(i + 1) * p_numCols + j].y += halfMag;
    p_currSolution[(size_t)(i - 1) * p_numCols + j].y += halfMag;
}
