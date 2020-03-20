#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/Asap.h>

#include <Engine/Primitive/TriMesh.h>

#include<Eigen/Sparse>

using namespace Ubpa;
using namespace Eigen;
using namespace std;

Asap::Asap(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void Asap::Clear() {
	triMesh = nullptr;
}

bool Asap::Init(Ptr<TriMesh> triMesh) 
{
	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Asap::Init:\n"
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
		printf("ERROR::Asap::Init:\n"
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

bool Asap::Run() {
	if (heMesh->IsEmpty() || !triMesh)
	{
		printf("ERROR::Asap::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	if (!asap(heMesh))
	{
		printf("ERROR::Asap::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	return true;
}

bool Asap::asap(Ptr<HEMesh<V>> heMesh) {

}
