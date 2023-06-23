
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float aspectRatio;
};

struct VStoGS
{
	float3 position	    : POSITION;
	float  size			: SIZE;
	float4 color        : COLOR;
};


struct GStoPS
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD0;
};


[maxvertexcount(4)]
void main(point VStoGS input[1], inout TriangleStream<GStoPS> outStream)
{
	GStoPS output;

	// Offsets for triangles
	// Order must match "triangle strip" order!
	float2 offsets[4];
	offsets[0] = float2(-1.0f, -1.0f);  // BL
	offsets[1] = float2(-1.0f, +1.0f);  // TL
	offsets[2] = float2(+1.0f, -1.0f);  // BR
	offsets[3] = float2(+1.0f, +1.0f);  // TR

	float2 uvs[4];
	uvs[0] = float2(0, 1);  // BL
	uvs[1] = float2(0, 0);  // TL
	uvs[2] = float2(1, 1);  // BR
	uvs[3] = float2(1, 0);  // TR

	// Calculate world view proj matrix
	matrix wvp = mul(mul(world, view), projection);

	// For each vert, spit out a small triangle
	[unroll]
	for (int o = 0; o < 4; o++)
	{
		// Calc position of this corner
		float3 pos = input[0].position;
		pos += float3(view._11, view._21, view._31) * offsets[o].x * input[0].size;
		pos += float3(view._12, view._22, view._32) * offsets[o].y * input[0].size;

		// Create a single vertex and add to the stream
		output.position = mul(float4(pos, 1.0f), wvp);

		// Pass through
		output.color = input[0].color;
		output.uv = saturate(uvs[o]);

		// Done
		outStream.Append(output);
	}

}