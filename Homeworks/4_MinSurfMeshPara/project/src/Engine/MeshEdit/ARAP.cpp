#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/Arap.h>

#include <Engine/Primitive/TriMesh.h>

#include<Eigen/Sparse>

using namespace Ubpa;
using namespace Eigen;
using namespace std;

Arap::Arap(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void Arap::Clear() {
	triMesh = nullptr;
}

bool Arap::Init(Ptr<TriMesh> triMesh)
{
	if(triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Arap::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::Arap::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool Arap::Run() {
	if (heMesh->IsEmpty() || !triMesh)
	{
		printf("ERROR::Asap::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	if (!arap(heMesh))
	{
		printf("ERROR::Asap::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	return true;
}

bool Arap::arap(Ptr<HEMesh<V>> heMesh) {
	

}
