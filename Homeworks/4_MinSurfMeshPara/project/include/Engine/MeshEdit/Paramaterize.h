#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include"MinSurf.h"

enum weight
{
	kuniform,
	kContangent,
	kDefault
};

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);

		bool RunUniform();
		bool RunContangent();
		
	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };
	public:
		bool Parameterize(Ptr<HEMesh<V>> heMesh);
		bool Shape_Preserving_Para(Ptr<HEMesh<V>> heMesh);
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;
	    Ptr<TriMesh> planarMesh;
		weight weight_type;
	};
}
