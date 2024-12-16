#pragma once
#include <vector>
#include <DirectXMath.h>

class Waves
{
public:
    Waves(int m, int n, float dx, float dt, float speed, float damping);
    Waves(const Waves& rhs) = delete;
    Waves& operator=(const Waves& rhs) = delete;
    ~Waves();

    int rowCount()const;
    int columnCount()const;
    int vertexCount()const;
    int triangleCount()const;
    float width()const;
    float depth()const;

    const DirectX::XMFLOAT3& position(int i)const { return p_currSolution[i]; }

    const DirectX::XMFLOAT3& normal(int i)const { return p_normals[i]; }

    const DirectX::XMFLOAT3& tangentX(int i)const { return p_tangentX[i]; }

    void update(float dt);
    void disturb(int i, int j, float magnitude);

private:
    int p_numRows = 0;
    int p_numCols = 0;

    int p_vertexCount = 0;
    int p_triangleCount = 0;

    float p_k1 = 0.0f;
    float p_k2 = 0.0f;
    float p_k3 = 0.0f;

    float p_timeStep = 0.0f;
    float p_spatialStep = 0.0f;

    std::vector<DirectX::XMFLOAT3> p_prevSolution;
    std::vector<DirectX::XMFLOAT3> p_currSolution;
    std::vector<DirectX::XMFLOAT3> p_normals;
    std::vector<DirectX::XMFLOAT3> p_tangentX;
};

