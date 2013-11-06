// This code is public domain.
//

#include "stdio.h"
#include "math.h"
#include <core/types.h>
#include <boost/thread.hpp>
#include <RTreeTemplate/RTree.h>
#include <pandaFramework.h>
#include <pandaSystem.h>
#include <NodePathCollection.h>
#include <GeomVertexFormat.h>
#include <GeomVertexData.h>
#include <GeomVertexWriter.h>
#include <GeomTriangles.h>

//#define DATASET_SIZE 75 // Subdivisions // heavy impact on time
#define DATASET_SIZE 75 // Subdivisions // heavy impact on time 16x16x16
#define FIELD_SIZE 10.0 // Mesh Space (0,0,0)-(FS,FS,FS)
#define DELTA_RANGE 0.005 // scope for measuring distance
#define WEIGHT_FACTOR 0.085 // weight factor

//using namespace core;
//struct vector3F
//{
//        float x;
//        float y;
//        float z;     
//};

namespace core
{
	namespace iprod
	{
		class MeshFactory
		{
			public:
				MeshFactory();
				~MeshFactory();
				void SetMarchingCubesConfig(const unsigned int &datasetSize = 16, const float &fieldSize = 10, 
											const float &voxelscopeDistance = 0.005, const float &metaballWeightfactor = 0.085, const float &targetValue = 48.0);

				NodePath* CreateVoxelized(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data);

				static bool RegisterPointIDIntoSearchResults(int id);
				static bool RegisterPointIDIntoGlobalSearchResults(int id);
				static bool RegisterGridPointIDIntoSearchResults(int id);

			private:

				void vMarchingCubes();
				int vMarchCube1(float fX, float fY, float fZ, float fScale, std::vector<vector3F> &vertex, std::vector<vector3F> &color, std::vector<vector3F> &edge_normal);
				float DistanceToWeightedPointsInRange(float fX, float fY, float fZ);
				//bool RegisterPointIDIntoSearchResults(int id, void* arg);
				//bool RegisterPointIDIntoGlobalSearchResults(int id, void* arg);

				float fGetOffset(float fValue1, float fValue2, float fValueDesired);
				void vGetNormal(vector3F &rfNormal, float fX, float fY, float fZ);
				void vGetColor(vector3F &rfColor, vector3F &rfPosition, vector3F &rfNormal);
				void vNormalizeVector(vector3F &rfVectorResult, vector3F &rfVectorSource);

				void SetUpSpatialGridIndex();
				
			private:
				
				boost::try_mutex m_mutex;

				RTFileStream *a_stream;
				unsigned int dataset_size;
				float field_size, voxelscope_distance, metaball_weightfactor;
				int     iDataSetSize;
				float   fStepSize;
				float   fTargetValue;
				std::map< int, std::vector<corePDU3D<double>> > source_weighted_points; // <weight, point>
				RTree<int, float, 3, float> spatial_index, spatial_grid_index;
				static std::vector<int> RTree_search_results;
				static std::vector<int> global_rTree_search_results;
				static std::vector<int> grid_rTree_search_results;
				std::map< int, corePDU3D<double> > source_weighted_points_indexed; // <id, point>
				std::map< int, corePDU3D<double> > gridpoints_indexed; // <id, point>
				std::map< int, int > weight_index; // <id, weight>
				float (MeshFactory::*fSample)(float fX, float fY, float fZ);// = DistanceToWeightedPointsInRange;
				int (MeshFactory::*vMarchCube)(float fX, float fY, float fZ, float fScale, std::vector<vector3F> &vertex, std::vector<vector3F> &color, std::vector<vector3F> &edge_normal);// = vMarchCube1;

				//These tables are used so that everything can be done in little loops that you can look at all at once
				// rather than in pages and pages of unrolled code.
				//a2fVertexOffset lists the positions, relative to vertex0, of each of the 8 vertices of a cube
				static const float a2fVertexOffset[8][3];
				//a2iEdgeConnection lists the index of the endpoint vertices for each of the 12 edges of the cube
				static const int a2iEdgeConnection[12][2];
				//a2fEdgeDirection lists the direction vector (vertex1-vertex0) for each edge in the cube
				static const float a2fEdgeDirection[12][3];

				// For any edge, if one vertex is inside of the surface and the other is outside of the surface
				//  then the edge intersects the surface
				// For each of the 8 vertices of the cube can be two possible states : either inside or outside of the surface
				// For any cube the are 2^8=256 possible sets of vertex states
				// This table lists the edges intersected by the surface for all 256 possible vertex states
				// There are 12 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1
				static const int CubeEdgeFlags[256];

				//  For each of the possible vertex states listed in CubeEdgeFlags there is a specific triangulation
				//  of the edge intersection points.  TriangleConnectionTable lists all of them in the form of
				//  0-5 edge triples with the list terminated by the invalid value -1.
				//  For example: TriangleConnectionTable[3] list the 2 triangles formed when corner[0] 
				//  and corner[1] are inside of the surface, but the rest of the cube is not.
				static const int TriangleConnectionTable[256][16];


		};


	}
}
