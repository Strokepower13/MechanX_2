cbuffer cbPerObject : register(b0)
{
	row_major float4x4 p_world;
};

cbuffer cbPass : register(b1)
{
	row_major float4x4 p_view;
	row_major float4x4 p_invView;
	row_major float4x4 p_proj;
	row_major float4x4 p_invProj;
	row_major float4x4 p_viewProj;
	row_major float4x4 p_invViewProj;
	float3 p_eyePosW;
	float cbPerObjectPad1;
	float2 p_renderTargetSize;
	float2 p_invRenderTargetSize;
	float p_nearZ;
	float p_farZ;
	float p_totalTime;
	float p_deltaTime;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}