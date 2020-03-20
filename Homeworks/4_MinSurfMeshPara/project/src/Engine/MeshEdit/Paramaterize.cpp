#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include<Eigen/Sparse>

#define pi 3.1415827

using namespace Ubpa;
using namespace Eigen;
using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) 
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
	weight_type = kDefault;
}

void Paramaterize::Clear() {
	triMesh = nullptr;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Parameterize::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::Paramaterize::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool Paramaterize::RunUniform() {
	if (heMesh->IsEmpty() || !triMesh)
	{
		printf("ERROR::Paramaterize::RunUniform\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	weight_type = kuniform;
	std::vector<pointf2> texcoords = triMesh->GetTexcoords();
	if (!Parameterize(heMesh))
	{
		printf("ERROR::Paramaterize::RunUniform\n"
			"\t""Parameterize Error");
	}
	triMesh->Update(texcoords);
	return true;
}

bool Paramaterize::RunContangent()
{
	if (heMesh->IsEmpty() || !triMesh)
	{
		printf("ERROR::Paramaterize::RunContagent\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	weight_type = kContangent;
	if (!Parameterize(heMesh))
	{
		printf("ERROR::Paramaterize::RunContagent\n"
			"\t""Parameterize Error");
	}
	return true;
}

bool Paramaterize::Parameterize(Ptr<HEMesh<V>> heMesh)
{
	if (!heMesh->HaveBoundary())
		printf("ERROR::MinSurf::Minimize\n"
			"\t""heMesh->HaveBoundary\n");

	std::vector<std::vector<THalfEdge<V, E, P>*>> bdy = heMesh->Boundaries();
	int size = heMesh->NumVertices();
	SparseMatrix<double> L(size, size);
	std::vector<Triplet<double>> coefficient;
	VectorXd bx(size), by(size);
	bx.setZero(); by.setZero();
	int degree = 0;
	int bdysize = bdy[0].size();
	double angle = 2*pi / (double(bdysize));
	vecf3 arg,adj_v,adj_v1,adj_v2,v0,v1,v2;
	double arg1, arg2;
	double w;
	for (auto vertice : heMesh->Vertices())
	{
		int degree = 0;
		vector<double> weight;
		if (!vertice->IsBoundary())
		{
			weight.clear();
			vecf3 v = vertice->pos.cast_to<vecf3>();

			vector<V*> adj_vec = vertice->AdjVertices();
			int adj_vec_size = adj_vec.size();
			for (int t=0;t<adj_vec.size();t++)
			{
				degree++;;
				if (!vertice->AdjVertices()[t]->IsBoundary())
				{
					if (weight_type == kuniform)
						coefficient.push_back(Triplet<double>(heMesh->Index(vertice), heMesh->Index(adj_vec[t]), -1));
					else if (weight_type == kContangent)
					{
						adj_v1 = adj_vec[(t+adj_vec_size-1)%adj_vec_size]->pos.cast_to<vecf3>();
						adj_v2 = adj_vec[(t + 1) % adj_vec_size]->pos.cast_to<vecf3>();
						adj_v = adj_vec[t]->pos.cast_to<vecf3>();
						v1 = adj_v1 - v;
						v2 = adj_v2 - v;
						v0 = adj_v - v;
						arg = v1.cos_theta(v0); arg1 = arg[0];
						arg = v2.cos_theta(v0); arg2 = arg[0];
						w = sqrt((1 - arg1) / (1 + arg1)) + sqrt((1 - arg2) / (1 + arg2));
						w /= v0.norm();
						weight.push_back(w);
						coefficient.push_back(Triplet<double>(heMesh->Index(vertice), heMesh->Index(adj_vec[t]), -w));
					}
				}
				else
				{
					int i=0;
					for (; i < bdy[0].size(); i++)
					{
						if (bdy[0][i]->Origin()->pos == adj_vec[t]->pos)
						{
							break;
						}
					}
					if (weight_type == kuniform)
					{
						by[heMesh->Index(vertice)] += sin(i * angle);
						bx[heMesh->Index(vertice)] += cos(i * angle);
						break;
					}
					else if (weight_type == kContangent)
					{
						adj_v1 = adj_vec[(t + adj_vec_size - 1) % adj_vec_size]->pos.cast_to<vecf3>();
						adj_v2 = adj_vec[(t + 1) % adj_vec_size]->pos.cast_to<vecf3>();
						adj_v = adj_vec[t]->pos.cast_to<vecf3>();
						v1 = adj_v1 - v;
						v2 = adj_v2 - v;
						v0 = adj_v - v;
						arg = v1.cos_theta(v0); arg1 = arg[0];
						arg = v2.cos_theta(v0); arg2 = arg[0];
						w = sqrt((1 - arg1) / (1 + arg1)) + sqrt((1 - arg2) / (1 + arg2));
						w /= v0.norm();
						weight.push_back(w);
						by[heMesh->Index(vertice)] += w * sin(i * angle);
						bx[heMesh->Index(vertice)] += w * cos(i * angle);
					}
				}
			}
			if (weight_type == kuniform)
			{
				coefficient.push_back(Triplet<double>(heMesh->Index(vertice), heMesh->Index(vertice), degree));
			}
			else if (weight_type == kContangent)
			{
				w = 0;
				for (auto it : weight)
					w += it;
				coefficient.push_back(Triplet<double>(heMesh->Index(vertice), heMesh->Index(vertice), w));
			}
		}
		else
		{
			coefficient.push_back(Triplet<double>(heMesh->Index(vertice), heMesh->Index(vertice), 1));
			for (int i = 0; i < bdy[0].size(); i++)
			{
				if (bdy[0][i]->Origin()->pos == vertice->pos)
				{
					by[heMesh->Index(vertice)] = sin(i * angle);
					bx[heMesh->Index(vertice)] = cos(i * angle);
					break;
				}
			}
			
		}
	}
	L.setFromTriplets(coefficient.begin(), coefficient.end());
	SparseLU<SparseMatrix<double>>  solver;
	solver.compute(L);
	if (solver.info() != Success)
	{
		cout << "Compute Matrix ERROR" << endl;
	}
	VectorXd x = solver.solve(bx);
	VectorXd y = solver.solve(by);

	size_t nV = heMesh->NumVertices();
	vector<pointf3> positions;
	positions.reserve(nV);
	for (int i=0;i<heMesh->NumVertices();i++)
		positions.push_back(pointf3(x[i],y[i],0));
		triMesh->Update(positions);

}

bool Paramaterize::Shape_Preserving_Para(Ptr<HEMesh<V>> heMesh) {

}