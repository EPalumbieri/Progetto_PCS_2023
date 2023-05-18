#ifndef __EMPTY_H
#define __EMPTY_H

#include <iostream>
#include "Eigen/Eigen"
#include <fstream>
#include "map"

using namespace std;
using namespace Eigen;

namespace ProjectLibrary
{
    struct Edge
    {
        unsigned int id;
        Point p1;
        Point p2;
    };

    struct Triangle
    {
        Point p1;
        Point p2;
        Point p3;
        Edge e1;
        Edge e2;
        Edge e3;
        unsigned int id;
        double area;
    };

    struct OrientedEdge
    {
      Edge* RealEdge;
      Triangle* RealTriangle;
      OrientedEdge* symmetric=nullptr;
      OrientedEdge* next=nullptr;
    };

    struct Point
    {
        double x;
        double y;
        unsigned int id;

        Point(const double& x,
              const double& y,
              const unsigned int& id):
          x(x), y(y), id(id)
        {
        }

        Point(const Point& p):
          x(p.x), y(p.y), id(p.id)
        {
        }
    };

    struct Mesh
    {
        unsigned int NumberCell0D = 0; ///< number of Cell0D
        vector<Point> Cell0D = {}; ///< Cell0D id, size 1 x NumberCell0D
        map<unsigned int, list<unsigned int>> Cell0DMarkers = {}; ///< Cell0D markers, size 1 x NumberCell0D (marker)

        unsigned int NumberCell1D = 0; ///< number of Cell1D
        vector<Edge> Cell1D = {}; ///< Cell1D id, size 1 x NumberCell1D
        map<unsigned int, list<unsigned int>> Cell1DMarkers = {}; ///< Cell1D propertoes, size 1 x NumberCell1D (marker)

        unsigned int NumberCell2D = 0; ///< number of Cell2D
        vector<Triangle> Cell2DId = {}; ///< Cell2D id, size 1 x NumberCell2D

        vector<OrientedEdge> GraphedMesh;
    };

    bool clockwise(Point P1, Point P2, Point P3);

    double Area(Triangle triangle);

    ///\brief Import the triangular mesh and test if the mesh is correct
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportMesh(Mesh& mesh, string file0D, string file1D, string file2D);

    ///\brief Import the Cell0D properties from Cell0Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell0Ds(Mesh& mesh, string nomeFile);

    ///\brief Import the Cell1D properties from Cell1Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell1Ds(Mesh& mesh, string nomeFile);

    ///\brief Import the Cell2D properties from Cell2Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell2Ds(Mesh& mesh, string nomeFile);
}

#endif // __EMPTY_H
