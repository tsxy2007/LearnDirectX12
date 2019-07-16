#include "GeometryGenrator.h"
#include <algorithm>

using namespace DirectX;
using MeshData = GeometryGenerator::MeshData;
using Vertex = GeometryGenerator::Vertex;

MeshData GeometryGenerator::CreateBox(float width, float height, float depth, uint32 numSubdivisions)
{
	MeshData meshData;

	Vertex v[24];
	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;
	
	// 前面
	v[0] = Vertex(-w2, -h2, -d2, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 0.f, 1.f);
	v[1] = Vertex(-w2, +h2, -d2, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	v[2] = Vertex(+w2, +h2, -d2, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	v[3] = Vertex(+w2, -h2, -d2, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 1.f, 1.f);
	// 后面
	v[4] = Vertex(-w2, -h2, +d2, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f);
	v[5] = Vertex(+w2, -h2, +d2, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 0.f, 1.f);
	v[6] = Vertex(+w2, +h2, +d2, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f);
	v[7] = Vertex(-w2, +h2, +d2, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f);
	// top
	v[8] = Vertex(-w2, +h2, -d2, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
	v[9] = Vertex(-w2, +h2, +d2, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	v[10] = Vertex(+w2, +h2, +d2, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	v[11] = Vertex(+w2, +h2, -d2, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f);
	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);


	meshData.Vertices.assign(&v[0], &v[24]);

	uint32 i[36];
	
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

	meshData.Indices32.assign(&i[0], &i[36]);

	numSubdivisions = min<uint32>(numSubdivisions, 6u);
	for (uint32 i = 0; i < numSubdivisions; ++i)
	{
		Subdivide(meshData);
	}
	return meshData;
}

MeshData GeometryGenerator::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount)
{
	MeshData meshData;

	Vertex topVertex(0.f, +radius, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	Vertex bottomVertex(0.f, -radius, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);

	meshData.Vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.f * XM_PI / sliceCount;

	for (uint32 i = 1; i < stackCount; i++)
	{
		float phi = i * phiStep;
		for (uint32 j = 0 ;j<sliceCount;j++)
		{
			float theta = j * thetaStep;
			Vertex v;
			v.Position.x = radius * sinf(phi)* cosf(theta);
			v.Position.y = radius * cosf(phi);
			v.Position.z = radius * sinf(phi)*sinf(theta);

			v.TangentU.x = -radius * sinf(phi)*sinf(theta);
			v.TangentU.y = 0.f;
			v.TangentU.z = radius * sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.TangentU);
			XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.Position);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.TexC.x = theta / XM_2PI;
			v.TexC.y = phi / XM_PI;
			meshData.Vertices.push_back(v);
		}
	}
	meshData.Vertices.push_back(bottomVertex);

	uint32 Count = meshData.Vertices.size() - 1;
	uint32 LastIndex = Count - 1;

	// north
	for (uint32 i = 0; i <= sliceCount; i++)
	{
		meshData.Indices32.push_back(0);
		meshData.Indices32.push_back(i);
		meshData.Indices32.push_back(i + 1);
	}

	// mid
	uint32 baseIndex = 1;
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0 ;i<stackCount - 2;i++)
	{
		for (uint32 j = 0 ;j<sliceCount;j++)
		{
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// sourth
	for (uint32 i = 0; i < sliceCount; i++)
	{
		meshData.Indices32.push_back(LastIndex);
		meshData.Indices32.push_back(LastIndex - i - 1);
		meshData.Indices32.push_back(LastIndex - i - 2);
	}


	return meshData;
}

MeshData GeometryGenerator::CreateGeosphere(float radius, uint32 numSubdivisions)
{
	return MeshData();
}

MeshData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount)
{
	return MeshData();
}

MeshData GeometryGenerator::CreateGrid(float width, float depth, uint32 m, uint32 n)
{
	return MeshData();
}

MeshData GeometryGenerator::CreateQuad(float x, float y, float w, float h, float depth)
{
	return MeshData();
}

void GeometryGenerator::Subdivide(MeshData & meshData)
{
}

Vertex GeometryGenerator::MidPoint(const Vertex & v0, const Vertex & v1)
{
	return Vertex();
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData & meshData)
{
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData & meshData)
{
}
