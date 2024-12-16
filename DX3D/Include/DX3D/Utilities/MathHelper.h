#pragma once
#include <DirectXMath.h>

namespace MathHelper
{
	static DirectX::XMMATRIX inverseTranspose(DirectX::FXMMATRIX M)
	{
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	static float randF()
	{
		return (float)(::rand()) / (float)RAND_MAX;
	}

	static float randF(float a, float b)
	{
		return a + randF() * (b - a);
	}

	static int rand(int a, int b)
	{
		return a + ::rand() % ((b - a) + 1);
	}
}