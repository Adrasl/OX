#include <MeshFactory.h>

#include <debugger.h> 


using namespace core;
using namespace core::iprod;


//These tables are used so that everything can be done in little loops that you can look at all at once
// rather than in pages and pages of unrolled code.
//a2fVertexOffset lists the positions, relative to vertex0, of each of the 8 vertices of a cube
const float MeshFactory::a2fVertexOffset[8][3] =
{       {0.0, 0.0, 0.0},{1.0, 0.0, 0.0},{1.0, 1.0, 0.0},{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},{1.0, 0.0, 1.0},{1.0, 1.0, 1.0},{0.0, 1.0, 1.0}
};
//a2iEdgeConnection lists the index of the endpoint vertices for each of the 12 edges of the cube
const int MeshFactory::a2iEdgeConnection[12][2] = 
{       {0,1}, {1,2}, {2,3}, {3,0},
		{4,5}, {5,6}, {6,7}, {7,4},
		{0,4}, {1,5}, {2,6}, {3,7}
};
//a2fEdgeDirection lists the direction vector (vertex1-vertex0) for each edge in the cube
const float MeshFactory::a2fEdgeDirection[12][3] =
{       {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
		{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},{0.0, 0.0, 1.0},{ 0.0, 0.0, 1.0},{0.0,  0.0, 1.0}
};

// For any edge, if one vertex is inside of the surface and the other is outside of the surface
//  then the edge intersects the surface
// For each of the 8 vertices of the cube can be two possible states : either inside or outside of the surface
// For any cube the are 2^8=256 possible sets of vertex states
// This table lists the edges intersected by the surface for all 256 possible vertex states
// There are 12 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1
const int MeshFactory::CubeEdgeFlags[256] =
{
		0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, 0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00, 
		0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, 0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90, 
		0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c, 0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30, 
		0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac, 0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0, 
		0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c, 0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60, 
		0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc, 0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0, 
		0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c, 0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950, 
		0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc, 0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0, 
		0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, 0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0, 
		0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, 0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650, 
		0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, 0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0, 
		0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, 0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460, 
		0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, 0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0, 
		0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, 0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230, 
		0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, 0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190, 
		0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, 0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};

//  For each of the possible vertex states listed in CubeEdgeFlags there is a specific triangulation
//  of the edge intersection points.  TriangleConnectionTable lists all of them in the form of
//  0-5 edge triples with the list terminated by the invalid value -1.
//  For example: TriangleConnectionTable[3] list the 2 triangles formed when corner[0] 
//  and corner[1] are inside of the surface, but the rest of the cube is not.
const int MeshFactory::TriangleConnectionTable[256][16] =  
{
		{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
		{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
		{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
		{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
		{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
		{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
		{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
		{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
		{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
		{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
		{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
		{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
		{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
		{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
		{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
		{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
		{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
		{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
		{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
		{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
		{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
		{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
		{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
		{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
		{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
		{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
		{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
		{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
		{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
		{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
		{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
		{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
		{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
		{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
		{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
		{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
		{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
		{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
		{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
		{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
		{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
		{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
		{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
		{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
		{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
		{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
		{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
		{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
		{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
		{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
		{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
		{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
		{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
		{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
		{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
		{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
		{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
		{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
		{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
		{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
		{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
		{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
		{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
		{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
		{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
		{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
		{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
		{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
		{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
		{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
		{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
		{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
		{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
		{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
		{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
		{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
		{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
		{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
		{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
		{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
		{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
		{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
		{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
		{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
		{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
		{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
		{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
		{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
		{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
		{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
		{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
		{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
		{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
		{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
		{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
		{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
		{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
		{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
		{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
		{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
		{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
		{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
		{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
		{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
		{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
		{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
		{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
		{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
		{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
		{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
		{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
		{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
		{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
		{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
		{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
		{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
		{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
		{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
		{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
		{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
		{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
		{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
		{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
		{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
		{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
		{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
		{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
		{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
		{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
		{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
		{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
		{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
		{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
		{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
		{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
		{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
		{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
		{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
		{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
		{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
		{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
		{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
		{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
		{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
		{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
		{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
		{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
		{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
		{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
		{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
		{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
		{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
		{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
		{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
		{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
		{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};	

std::vector<int> MeshFactory::RTree_search_results;
std::vector<int> MeshFactory::global_rTree_search_results;
std::vector<int> MeshFactory::grid_rTree_search_results;


bool RegisterPointIDIntoSearchResults_callback(int id, void* arg) 
{	
	return MeshFactory::RegisterPointIDIntoSearchResults(id);
}

bool RegisterPointIDIntoGlobalSearchResults_callback(int id, void* arg) 
{	
	return MeshFactory::RegisterPointIDIntoGlobalSearchResults(id);
}

bool RegisterGridPointIDIntoSearchResults_callback(int id, void* arg) 
{	
	return MeshFactory::RegisterGridPointIDIntoSearchResults(id);
}


MeshFactory::MeshFactory() : a_stream(NULL), dataset_size(16), field_size(10), voxelscope_distance(0.005f), metaball_weightfactor(0.085)
{
	fSample = &MeshFactory::DistanceToWeightedPointsInRange;
	vMarchCube = &MeshFactory::vMarchCube1;

	//SetUpSpatialGridIndex();
}

void MeshFactory::SetMarchingCubesConfig(const unsigned int &datasetSize, const float &fieldSize, 
							const float &voxelscopeDistance, const float &metaballWeightfactor, const float &targetValue)
{
	dataset_size = datasetSize;
	field_size = fieldSize;
	voxelscope_distance = voxelscopeDistance;
	metaball_weightfactor = metaballWeightfactor;
	//fTargetValue = (fieldSize/datasetSize)*targetValue; //target value as voxel size proportion //retomar
	fTargetValue = targetValue;

	SetUpSpatialGridIndex();
}

void MeshFactory::SetUpSpatialGridIndex()
{
		double timestamp = (double)clock()/CLOCKS_PER_SEC;



	spatial_grid_index.RemoveAll();
	gridpoints_indexed.clear();
	
	int point_id = 0;
	float fStepSizeinv = 1;//field_size/dataset_size; //dataset_size/field_size;
	float weight_offset = 0.5;
	int iX, iY, iZ;
	for(iX = 0; iX < dataset_size; iX++)
	for(iY = 0; iY < dataset_size; iY++)
	for(iZ = 0; iZ < dataset_size; iZ++)
	{
		int jX, jY, jZ, jXini, jYini, jZini, jXend, jYend, jZend;
		int grid_coord_x, grid_coord_y, grid_coord_z;

		//GRID cells coordinates 0->dataset_size
		grid_coord_x = iX*fStepSizeinv;
		grid_coord_y = iY*fStepSizeinv;
		grid_coord_z = iZ*fStepSizeinv;
		jXini = ((iX - weight_offset) > 0) ? (iX - weight_offset)*fStepSizeinv : 0;
		jYini = ((iY - weight_offset) > 0) ? (iY - weight_offset)*fStepSizeinv : 0;
		jZini = ((iZ - weight_offset) > 0) ? (iZ - weight_offset)*fStepSizeinv : 0;
		jXend = ((iX + weight_offset) > 0) ? (iX + weight_offset)*fStepSizeinv : 0;
		jYend = ((iY + weight_offset) > 0) ? (iY + weight_offset)*fStepSizeinv : 0;
		jZend = ((iZ + weight_offset) > 0) ? (iZ + weight_offset)*fStepSizeinv : 0;

		Rect3F point_rect(jXini,jYini,jZini,jXend,jYend,jZend);
		spatial_grid_index.Insert(point_rect.min, point_rect.max, point_id);
		corePDU3D<double> grid_cell;
		grid_cell.position.x = iX*fStepSizeinv;
		grid_cell.position.y = iY*fStepSizeinv;
		grid_cell.position.z = iZ*fStepSizeinv;
		gridpoints_indexed[point_id] = grid_cell;
		point_id++;
	}

		double timestamp2 = (double)clock()/CLOCKS_PER_SEC;
		double dif_time = timestamp2 - timestamp;
		cout << "Setting up Spatial grid: " << dif_time << "sec\n";

	//RTFileStream grid_stream; //stream not implemented
	//spatial_grid_index.Save(grid_stream);
	//spatial_grid_index.Save("c:/etc/grid_spatial_index.txt");
	
	//Descomentame - no seguro si se usa
	//cout << "Saving Grid: please wait, this may take a while.\n";
	//if (a_stream)
	//	delete a_stream;
	//a_stream = new RTFileStream();
	//spatial_grid_index.Save(*a_stream);
}

bool MeshFactory::RegisterPointIDIntoSearchResults(int id) 
{	//printf("Hit data rect %d\n", id);
	RTree_search_results.push_back(id);
	return true; // keep going
}

bool MeshFactory::RegisterPointIDIntoGlobalSearchResults(int id) 
{	//printf("Hit data rect %d\n", id);
	global_rTree_search_results.push_back(id);
	return true; // keep going
}

bool MeshFactory::RegisterGridPointIDIntoSearchResults(int id) 
{	//printf("Hit data rect %d\n", id);
	grid_rTree_search_results.push_back(id);
	return true; // keep going
}

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
float MeshFactory::fGetOffset(float fValue1, float fValue2, float fValueDesired)
{
        double fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}

void MeshFactory::vGetColor(vector3F &rfColor, vector3F &rfPosition, vector3F &rfNormal)
{
        float fX = rfNormal.x;
        float fY = rfNormal.y;
        float fZ = rfNormal.z;
		rfColor.x = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
        rfColor.y = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
        rfColor.z = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);
}

void MeshFactory::vNormalizeVector(vector3F &rfVectorResult, vector3F &rfVectorSource)
{
        float fOldLength;
        float fScale;

        fOldLength = sqrtf( (rfVectorSource.x * rfVectorSource.x) +
                            (rfVectorSource.y * rfVectorSource.y) +
                            (rfVectorSource.z * rfVectorSource.z) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.x = rfVectorSource.x;
                rfVectorResult.y = rfVectorSource.y;
                rfVectorResult.z = rfVectorSource.z;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.x = rfVectorSource.x*fScale;
                rfVectorResult.y = rfVectorSource.y*fScale;
                rfVectorResult.z = rfVectorSource.z*fScale;
        }
}

float MeshFactory::DistanceToWeightedPointsInRange(float fX, float fY, float fZ)
{
    double fResult = 0.0;
    double fDx, fDy, fDz;

	//float search_delta = voxelscopeDistance * dataset_size / field_size;
	float search_delta = voxelscope_distance;
	RTree_search_results.clear();
	Rect3F search_rect(fX-search_delta,fY-search_delta,fZ-search_delta, fX+search_delta,fY+search_delta,fZ+search_delta);
	int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults_callback, NULL);

	for (int i = 0; i < RTree_search_results.size(); i++)
	{	std::map< int, corePDU3D<double> >::iterator iter = source_weighted_points_indexed.find(RTree_search_results[i]);
		if (iter  != source_weighted_points_indexed.end())
		{	fDx = fX - (iter->second).position.x;
			fDy = fY - (iter->second).position.y;
			fDz = fZ - (iter->second).position.z;
			std::map<int, int>::iterator iter2 = weight_index.find(iter->first);
			if ( iter2 != weight_index.end() ) 
				fResult += (metaball_weightfactor * (float(iter2->second)))/(fDx*fDx + fDy*fDy + fDz*fDz);
		}
	}
    return fResult;
}

//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
void MeshFactory::vGetNormal(vector3F &rfNormal, float fX, float fY, float fZ)
{
        rfNormal.x = (this->*fSample)(fX-0.01, fY, fZ) - (this->*fSample)(fX+0.01, fY, fZ);
        rfNormal.y = (this->*fSample)(fX, fY-0.01, fZ) - (this->*fSample)(fX, fY+0.01, fZ);
        rfNormal.z = (this->*fSample)(fX, fY, fZ-0.01) - (this->*fSample)(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal, rfNormal);
}

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
int  MeshFactory::vMarchCube1(float fX, float fY, float fZ, float fScale, std::vector<vector3F> &vertex, std::vector<vector3F> &color, std::vector<vector3F> &edge_normal)
{
        extern int CubeEdgeFlags[256];
        extern int TriangleConnectionTable[256][16];
		int nTriangles = 0;

        int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        float fOffset;
        //GLvector sColor;
		vector3F sColor;
        float CubeValue[8];
        //GLvector asEdgeVertex[12];
        //GLvector asEdgeNorm[12];
        vector3F asEdgeVertex[12];
        vector3F asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                CubeValue[iVertex] = (this->*fSample)(fX + a2fVertexOffset[iVertex][0]*fScale,
													  fY + a2fVertexOffset[iVertex][1]*fScale,
													  fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(CubeValue[iVertexTest] <= fTargetValue) 
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = CubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0) 
        {
                return 0;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = fGetOffset(CubeValue[ a2iEdgeConnection[iEdge][0] ], 
                                             CubeValue[ a2iEdgeConnection[iEdge][1] ],
											 fTargetValue);

						asEdgeVertex[iEdge].x = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
						asEdgeVertex[iEdge].y = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
						asEdgeVertex[iEdge].z = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

						vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z);
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube

        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(TriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = TriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
                        //vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                        //glColor3f(sColor.fX, sColor.fY, sColor.fZ);
                        //glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                        //glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
						vertex[nTriangles*3+iCorner].x = asEdgeVertex[iVertex].x;
						vertex[nTriangles*3+iCorner].y = asEdgeVertex[iVertex].y;
						vertex[nTriangles*3+iCorner].z = asEdgeVertex[iVertex].z;

						edge_normal[nTriangles*3+iCorner].x = asEdgeNorm[iVertex].x;
						edge_normal[nTriangles*3+iCorner].y = asEdgeNorm[iVertex].y;
						edge_normal[nTriangles*3+iCorner].z = asEdgeNorm[iVertex].z;

						color[nTriangles*3+iCorner].x = sColor.x;
						color[nTriangles*3+iCorner].y = sColor.y;
						color[nTriangles*3+iCorner].z = sColor.z;
                }
				nTriangles++;
        }
	return nTriangles;
}
       

//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
void MeshFactory::vMarchingCubes()
{
        int iX, iY, iZ;
        for(iX = 0; iX < iDataSetSize; iX++)
        for(iY = 0; iY < iDataSetSize; iY++)
        for(iZ = 0; iZ < iDataSetSize; iZ++)
        {
			std::vector<vector3F> vertex;
			std::vector<vector3F> color;
			std::vector<vector3F> edge_normal;
			vector3F zero_vector;
			zero_vector.x = zero_vector.y =zero_vector.z = 0;
			vertex.resize(3, zero_vector);
			color.resize(3, zero_vector);
			edge_normal.resize(3, zero_vector);
            (this->*vMarchCube)(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, vertex, color, edge_normal);
        }
}
/*
NodePath* MeshFactory::CreateVoxelized(std::map< int, std::vector<vector3F> > source_weighted_data)
{
	//fSample = fSample1;
	//fSample = fSample3;
	//fSample = DistanceToWeightedPoints;
	fSample = &MeshFactory::DistanceToWeightedPointsInRange;
	vMarchCube = &MeshFactory::vMarchCube1;

	//RTree<int, float, 3, float> spatial_index;

	int point_id = 0;
	spatial_index.RemoveAll();
	weight_index.clear();
	source_weighted_points.clear();
	source_weighted_points_indexed.clear();
	

	source_weighted_points = source_weighted_data;
	for (std::map< int, std::vector<vector3F> >::iterator iter = source_weighted_data.begin(); iter != source_weighted_data.end(); iter++)
	{	for (std::vector<vector3F>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{	float envelope = 0.1*(iter->first);
			Rect3F point_rect((*iter2).x-envelope,(*iter2).y-envelope,(*iter2).z-envelope,
			                  (*iter2).x+envelope,(*iter2).y+envelope,(*iter2).z+envelope);
			spatial_index.Insert(point_rect.min, point_rect.max, point_id);
			source_weighted_points_indexed[point_id] = (*iter2);
			weight_index[point_id] = iter->first;
			point_id++;
		}
	}

	CPT(GeomVertexFormat) format = GeomVertexFormat::get_v3n3c4t2();
	PT(GeomVertexData) vdata = new GeomVertexData("vertices", format, GeomEnums::UH_static);

	GeomVertexWriter vertex, normal, color, texcoord;
	vertex = GeomVertexWriter(vdata, "vertex");
	normal = GeomVertexWriter(vdata, "normal");
	color = GeomVertexWriter(vdata, "color");
	texcoord = GeomVertexWriter(vdata, "texcoord");
	
	//CONTENT-------------------
	
	//vSetTime(0.5);
	int nVertices=0;

	iDataSetSize = dataset_size;
	fStepSize = field_size/dataset_size;
	float fStepSizeinv = dataset_size/field_size;
	
	float iX, iY, iZ;

	float search_delta = voxelscope_distance;
	global_rTree_search_results.clear();
	Rect3F search_rect(0,0,0, field_size,field_size,field_size);
	int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoGlobalSearchResults_callback, NULL);

	std::map<std::string, bool> visited;

	RTree<int, float, 3, float> aux_spatial_grid_index;// = spatial_grid_index;
	//aux_spatial_grid_index.Load("c:/etc/grid_spatial_index.txt");
	if (a_stream)
	{	a_stream->Begin();
		aux_spatial_grid_index.Load(*a_stream);//TOO SLOWWWWWWWWWWWWW
		//La clave está en clonar el árbol sinque tarde.... ¬¬'
	}
	Rect3F grid_search_rect_whole(0, 0, 0, dataset_size, dataset_size, dataset_size);
	int grid_overlapping_size_whole = aux_spatial_grid_index.Search(grid_search_rect_whole.min, grid_search_rect_whole.max, RegisterGridPointIDIntoSearchResults_callback, NULL);


////----------------------
//	RTree<int, float, 3, float> aux_spatial_grid_index;// = spatial_grid_index;
//	RTree<int, float, 3, float> aux_spatial_grid_index2;// = spatial_grid_index;
//
//	Rect3F point_rect(10,10,10,11,11,11);
//	Rect3F point_rect2(12,12,12,13,13,13);
//	aux_spatial_grid_index.Insert(point_rect.min, point_rect.max, 1);
//	aux_spatial_grid_index.Insert(point_rect2.min, point_rect2.max, 2);
//
//	if (a_stream)
//		delete a_stream;
//	a_stream = new RTFileStream();
//	aux_spatial_grid_index.Save(*a_stream);
//
//	if (a_stream)
//		aux_spatial_grid_index2.Load(*a_stream);
//
//	RTFileStream *b_stream;
//	aux_spatial_grid_index2.Save(*b_stream);
//
//	Rect3F grid_search_rect_whole(0, 0, 0, 100, 100, 100);
//	int grid_overlapping_size_whole = aux_spatial_grid_index2.Search(grid_search_rect_whole.min, grid_search_rect_whole.max, RegisterGridPointIDIntoSearchResults_callback, NULL);
////----------------------


	for (std::vector<int>::iterator s_iter = global_rTree_search_results.begin(); s_iter != global_rTree_search_results.end(); s_iter++)
	{
		std::map< int, vector3F >::iterator id_iter = source_weighted_points_indexed.find(*s_iter);
		if (id_iter == source_weighted_points_indexed.end())
			continue;

		iX = id_iter->second.x;
		iY = id_iter->second.y;
		iZ = id_iter->second.z;
		float weight_offset = 0;// weight_index[id_iter->first]/2;
		//-----------------------------
		//FIX!!!! IGNORES TOO MANY GRID CELLS, but this approach is dimension explosive
		//-----------------------------

		int jX, jY, jZ, jXini, jYini, jZini, jXend, jYend, jZend;
		int grid_coord_x, grid_coord_y, grid_coord_z;

		//GRID cells coordinates 0->dataset_size
		grid_coord_x = iX*fStepSizeinv;
		grid_coord_y = iY*fStepSizeinv;
		grid_coord_z = iZ*fStepSizeinv;
		jXini = ((iX - weight_offset) > 0) ? (iX - weight_offset)*fStepSizeinv : 0;
		jYini = ((iY - weight_offset) > 0) ? (iY - weight_offset)*fStepSizeinv : 0;
		jZini = ((iZ - weight_offset) > 0) ? (iZ - weight_offset)*fStepSizeinv : 0;
		jXend = ((iX + weight_offset) > 0) ? (iX + weight_offset)*fStepSizeinv+1 : 0;
		jYend = ((iY + weight_offset) > 0) ? (iY + weight_offset)*fStepSizeinv+1 : 0;
		jZend = ((iZ + weight_offset) > 0) ? (iZ + weight_offset)*fStepSizeinv+1 : 0;

		grid_rTree_search_results.clear();
		Rect3F grid_search_rect(jXini, jYini, jZini, jXend, jYend, jZend);
		int grid_overlapping_size = aux_spatial_grid_index.Search(grid_search_rect.min, grid_search_rect.max, RegisterGridPointIDIntoSearchResults_callback, NULL);

		//for (jX = jXini; jX <= jXend; jX++)
		//for (jY = jYini; jY <= jYend; jY++)
		//for (jZ = jZini; jZ <= jZend; jZ++)
		for (std::vector<int>::iterator g_iter = grid_rTree_search_results.begin(); g_iter != grid_rTree_search_results.end(); g_iter++)
		{
			int kX, kY, kZ;
			std::map< int, vector3F >::iterator grid_iterator = gridpoints_indexed.find(*g_iter);
			if (grid_iterator != gridpoints_indexed.end())
			{
				kX = grid_iterator->second.x;
				kY = grid_iterator->second.y;
				kZ = grid_iterator->second.z;
				vector3F zero_vector;
				zero_vector.x = zero_vector.y =zero_vector.z = 0;
				std::vector<vector3F> v_vertex;
				std::vector<vector3F> v_color;
				std::vector<vector3F> v_edge_normal;
				v_vertex.resize(dataset_size, zero_vector);
				v_color.resize(dataset_size, zero_vector);
				v_edge_normal.resize(dataset_size, zero_vector);
											//coordenada real del cubo 0->field_size
				int nTriangles = (this->*vMarchCube)(kX*fStepSize, kY*fStepSize, kZ*fStepSize, fStepSize, v_vertex, v_color, v_edge_normal);
				for ( int i = 0; i < nTriangles; i++)
				{
					vertex.add_data3f(v_vertex[i*3].x,v_vertex[i*3].y,v_vertex[i*3].z);
					normal.add_data3f(v_edge_normal[i*3].x,v_edge_normal[i*3].y,v_edge_normal[i*3].z);
					color.add_data4f(v_color[i*3].x,v_color[i*3].y,v_color[i*3].z, 1.0);
					texcoord.add_data2f(1, 0);

					vertex.add_data3f(v_vertex[i*3+1].x,v_vertex[i*3+1].y,v_vertex[i*3+1].z);
					normal.add_data3f(v_edge_normal[i*3+1].x,v_edge_normal[i*3+1].y,v_edge_normal[i*3+1].z);
					color.add_data4f(v_color[i*3+1].x,v_color[i*3+1].y,v_color[i*3+1].z, 1.0);
					texcoord.add_data2f(1, 1);

					vertex.add_data3f(v_vertex[i*3+2].x,v_vertex[i*3+2].y,v_vertex[i*3+2].z);
					normal.add_data3f(v_edge_normal[i*3+2].x,v_edge_normal[i*3+2].y,v_edge_normal[i*3+2].z);
					color.add_data4f(v_color[i*3+2].x,v_color[i*3+2].y,v_color[i*3+2].z, 1.0);
					texcoord.add_data2f(0, 1);
				}
				nVertices += nTriangles*3;
			}

			aux_spatial_grid_index.Remove(grid_search_rect.min, grid_search_rect.max, *g_iter);
			//grid_rTree_search_results.erase(*g_iter);
		}
	}

	//TRIANGLES
	PT(GeomTriangles) tris;
	tris = new GeomTriangles(Geom::UH_static);
	for (int i = 0; i<nVertices; i = i+3)
	{	tris->add_vertex(i+0);
		tris->add_vertex(i+1);
		tris->add_vertex(i+2);
		tris->close_primitive();
	}

	PT(Geom) squareGeom = new Geom(vdata) ;
	squareGeom->add_primitive(tris) ;
	PT(GeomNode) squareGN = new GeomNode("square") ;
	squareGN->add_geom(squareGeom) ;

	NodePath* quad = new NodePath( (PandaNode*)squareGN );
	quad->set_two_sided(true);
	quad->set_texture_off();

	//Texture *text = TexturePool::load_texture("/c/etc/Iris.png");
	//quad->set_transparency(TransparencyAttrib::M_alpha);
	//quad->set_texture(text);

	return quad;
}*/


NodePath* MeshFactory::CreateVoxelized(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data)
{
	//retomar, crear tambien collisionbox


	//----------------
//----------------------
	RTree<int, float, 3, float> aux_spatial_grid_index;// = spatial_grid_index;
	RTree<int, float, 3, float> aux_spatial_grid_index2;// = spatial_grid_index;

	Rect3F point_rect(10,10,10,24,24,24);
	Rect3F point_rect2(22,22,22,33,33,33);
	aux_spatial_grid_index.Insert(point_rect.min, point_rect.max, 1);
	aux_spatial_grid_index.Insert(point_rect2.min, point_rect2.max, 2);
	Rect3F grid_search_rect_whole(0, 0, 0,5, 5, 5);
	int grid_overlapping_size_whole = aux_spatial_grid_index.Search(grid_search_rect_whole.min, grid_search_rect_whole.max, RegisterGridPointIDIntoSearchResults_callback, NULL);
//----------------------
	//----------------
	
	//fSample = fSample1;
	//fSample = fSample3;
	//fSample = DistanceToWeightedPoints;
	fSample = &MeshFactory::DistanceToWeightedPointsInRange;
	vMarchCube = &MeshFactory::vMarchCube1;

	//RTree<int, float, 3, float> spatial_index;

	int point_id = 0;
	spatial_index.RemoveAll();
	weight_index.clear();
	source_weighted_points.clear();
	source_weighted_points_indexed.clear();

	int avatarcolliderindex = 0;
	source_weighted_points = source_weighted_data;
	for (std::map< int, std::vector<corePDU3D<double>> >::iterator iter = source_weighted_data.begin(); iter != source_weighted_data.end(); iter++)
	{	for (std::vector<corePDU3D<double>>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{	float envelope = 0.1*(iter->first);
			Rect3F point_rect((*iter2).position.x-envelope,(*iter2).position.y-envelope,(*iter2).position.z-envelope,
			                  (*iter2).position.x+envelope,(*iter2).position.y+envelope,(*iter2).position.z+envelope);
			spatial_index.Insert(point_rect.min, point_rect.max, point_id);
			source_weighted_points_indexed[point_id] = (*iter2);
			weight_index[point_id] = iter->first;
			point_id++;
		}
	}

	CPT(GeomVertexFormat) format = GeomVertexFormat::get_v3n3c4t2();
	PT(GeomVertexData) vdata = new GeomVertexData("vertices", format, GeomEnums::UH_static);

	GeomVertexWriter vertex, normal, color, texcoord;
	vertex = GeomVertexWriter(vdata, "vertex");
	normal = GeomVertexWriter(vdata, "normal");
	color = GeomVertexWriter(vdata, "color");
	texcoord = GeomVertexWriter(vdata, "texcoord");
	
	//CONTENT-------------------
	
	//vSetTime(0.5);
	int nVertices=0;

	iDataSetSize = dataset_size;
	fStepSize = field_size/dataset_size;
	float fStepSizeinv = dataset_size/field_size;
	
	float iX, iY, iZ;

	float search_delta = voxelscope_distance;
	global_rTree_search_results.clear();
	Rect3F search_rect(0,0,0, field_size,field_size,field_size);
	int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoGlobalSearchResults_callback, NULL);

	std::map<std::string, bool> visited;

	for (std::vector<int>::iterator s_iter = global_rTree_search_results.begin(); s_iter != global_rTree_search_results.end(); s_iter++)
	{
		std::map< int, corePDU3D<double> >::iterator id_iter = source_weighted_points_indexed.find(*s_iter);
		if (id_iter == source_weighted_points_indexed.end())
			continue;

		iX = id_iter->second.position.x;
		iY = id_iter->second.position.y;
		iZ = id_iter->second.position.z;
		float weight_offset = 0;// weight_index[id_iter->first]/2;
		//-----------------------------
		//FIX!!!! IGNORES TOO MANY GRID CELLS, but this approach is dimension explosive
		//-----------------------------

		int jX, jY, jZ, jXini, jYini, jZini, jXend, jYend, jZend;
		int grid_coord_x, grid_coord_y, grid_coord_z;

		//GRID cells coordinates 0->dataset_size
		grid_coord_x = iX*fStepSizeinv;
		grid_coord_y = iY*fStepSizeinv;
		grid_coord_z = iZ*fStepSizeinv;
		jXini = ((iX - weight_offset) > 0) ? (iX - weight_offset)*fStepSizeinv : 0;
		jYini = ((iY - weight_offset) > 0) ? (iY - weight_offset)*fStepSizeinv : 0;
		jZini = ((iZ - weight_offset) > 0) ? (iZ - weight_offset)*fStepSizeinv : 0;
		jXend = ((iX + weight_offset) > 0) ? (iX + weight_offset)*fStepSizeinv+1 : 0;
		jYend = ((iY + weight_offset) > 0) ? (iY + weight_offset)*fStepSizeinv+1 : 0;
		jZend = ((iZ + weight_offset) > 0) ? (iZ + weight_offset)*fStepSizeinv+1 : 0;

		for (jX = jXini; jX <= jXend; jX++)
		for (jY = jYini; jY <= jYend; jY++)
		for (jZ = jZini; jZ <= jZend; jZ++)
		{
			std::stringstream point_str;
			point_str << jX << ":" << jY << ":" << jZ;
			std::map<std::string, bool>::iterator v_iter = visited.find(point_str.str());
			if (v_iter == visited.end())
			{
				visited[point_str.str()] = true;

				//vector3F v_vertex[dataset_size-1];
				//vector3F v_color[dataset_size-1];
				//vector3F v_edge_normal[dataset_size-1];
				vector3F zero_vector, centeredOriginOffset_vector;
				zero_vector.x = zero_vector.y =zero_vector.z = 0;
				centeredOriginOffset_vector.x = centeredOriginOffset_vector.y = centeredOriginOffset_vector.z = field_size/2;
				std::vector<vector3F> v_vertex;
				std::vector<vector3F> v_color;
				std::vector<vector3F> v_edge_normal;
				v_vertex.resize(dataset_size, zero_vector);
				v_color.resize(dataset_size, zero_vector);
				v_edge_normal.resize(dataset_size, zero_vector);
											//coordenada real del cubo 0->field_size
				int nTriangles = (this->*vMarchCube)(jX*fStepSize, jY*fStepSize, jZ*fStepSize, fStepSize, v_vertex, v_color, v_edge_normal);
				for ( int i = 0; i < nTriangles; i++)
				{	//retomar centrar malla en el origen de verdad de la buena
					vertex.add_data3f(v_vertex[i*3].x-7.5,v_vertex[i*3].y+5,v_vertex[i*3].z-2.5);
					normal.add_data3f(v_edge_normal[i*3].x,v_edge_normal[i*3].y,v_edge_normal[i*3].z);
					color.add_data4f(v_color[i*3].x,v_color[i*3].y,v_color[i*3].z, 1.0);
					texcoord.add_data2f(1, 0);

					vertex.add_data3f(v_vertex[i*3+1].x-7.5,v_vertex[i*3+1].y+5,v_vertex[i*3+1].z-2.5);
					normal.add_data3f(v_edge_normal[i*3+1].x,v_edge_normal[i*3+1].y,v_edge_normal[i*3+1].z);
					color.add_data4f(v_color[i*3+1].x,v_color[i*3+1].y,v_color[i*3+1].z, 1.0);
					texcoord.add_data2f(1, 1);

					vertex.add_data3f(v_vertex[i*3+2].x-7.5,v_vertex[i*3+2].y+5,v_vertex[i*3+2].z-2.5);
					normal.add_data3f(v_edge_normal[i*3+2].x,v_edge_normal[i*3+2].y,v_edge_normal[i*3+2].z);
					color.add_data4f(v_color[i*3+2].x,v_color[i*3+2].y,v_color[i*3+2].z, 1.0);
					texcoord.add_data2f(0, 1);
				}
				nVertices += nTriangles*3;
			}
		}
	}

	//TRIANGLES
	PT(GeomTriangles) tris;
	tris = new GeomTriangles(Geom::UH_static);
	for (int i = 0; i<nVertices; i = i+3)
	{	tris->add_vertex(i+0);
		tris->add_vertex(i+1);
		tris->add_vertex(i+2);
		tris->close_primitive();
	}

	PT(Geom) squareGeom = new Geom(vdata) ;
	squareGeom->add_primitive(tris) ;
	PT(GeomNode) squareGN = new GeomNode("square") ;
	squareGN->add_geom(squareGeom) ;

	NodePath* quad = new NodePath( (PandaNode*)squareGN );
	quad->set_two_sided(true);
	quad->set_texture_off();

	//Texture *text = TexturePool::load_texture("/c/etc/Iris.png");
	//quad->set_transparency(TransparencyAttrib::M_alpha);
	//quad->set_texture(text);

	return quad;
}