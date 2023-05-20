#ifndef __EMPTY_H
#define __EMPTY_H

#include <cmath>
#include <iostream>
#include "Eigen/Eigen"
#include <fstream>
#include "map"
#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace Eigen;

namespace ProjectLibrary
{


    struct Edge
    {
        std::unordered_set<unsigned int> points;
        void operator=(unordered_set<unsigned int> sett)
        {
            points=sett;
        }
        Edge()
        {

        }
        Edge(unordered_set<unsigned int> sett):points(sett)
        {
        }
    };

    struct Triangle
    {
        array<unsigned int,3> vertices;
        array<unsigned int,3> edges;
        double area;
    };

    struct OrientedEdge
    {
      unsigned int RealEdge;
      unsigned int RealTriangle;
      OrientedEdge* symmetric=nullptr;
      OrientedEdge* next=nullptr;

      OrientedEdge(unsigned int realEdge,
                   unsigned int realTriangle):
          RealEdge(realEdge),RealTriangle(realTriangle),symmetric(nullptr), next(nullptr){

      }
    };

    struct Point
    {
        double x;
        double y;


        Point(const double& x,
              const double& y):
          x(x), y(y)
        {
        }

        Point(const Point& p):
          x(p.x), y(p.y)
        {
        }

        Point(){

        }

        void operator=(const Point& point)
        {
            x=point.x;
            y=point.y;
        }

    };

    inline double normSquared(const double& x, const double& y)
    {
      return x * x + y * y;
    }


    struct Mesh
    {
        unsigned int NumberCell0D = 0; ///< number of Cell0D
        map<unsigned int, Point> Cell0D = {}; ///< Cell0D id, size 1 x NumberCell0D
        map<unsigned int, list<unsigned int>> Cell0DMarkers = {}; ///< Cell0D markers, size 1 x NumberCell0D (marker)

        unsigned int NumberCell1D = 0; ///< number of Cell1D
        map<unsigned int, Edge> Cell1D = {}; ///< Cell1D id, size 1 x NumberCell1D
        map<unsigned int, list<unsigned int>> Cell1DMarkers = {}; ///< Cell1D propertoes, size 1 x NumberCell1D (marker)

        unsigned int NumberCell2D = 0; ///< number of Cell2D
        map<unsigned int, Triangle> Cell2D = {}; ///< Cell2D id, size 1 x NumberCell2D

        vector<OrientedEdge*> GraphedMesh;
    };

    bool clockwise(Point P1, Point P2, Point P3);

    double Area(Triangle triangle);

    double distance(const Point& p1, const Point& p2);

    double length(Mesh& mesh,OrientedEdge* edge);

    bool isLongest(Mesh& mesh,OrientedEdge* edge);

    Point midpoint(const Point& p1, const Point& p2);

    bool bisect(Mesh& mesh,OrientedEdge* edge);

    ///\brief Import the triangular mesh and test if the mesh is correct
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportMesh(Mesh& mesh, string file0D, string file1D, string file2D);
    bool ExportMesh(Mesh& mesh, string file0D, string file1D, string file2D);
    ///\brief Import the Cell0D properties from Cell0Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell0Ds(Mesh& mesh, string nomeFile);
    bool ExportCell0Ds(Mesh& mesh, string nomeFile);

    ///\brief Import the Cell1D properties from Cell1Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell1Ds(Mesh& mesh, string nomeFile);
    bool ExportCell1Ds(Mesh& mesh, string nomeFile);

    ///\brief Import the Cell2D properties from Cell2Ds.csv file
    ///\param mesh: a TriangularMesh struct
    ///\return the result of the reading, true if is success, false otherwise
    bool ImportCell2Ds(Mesh& mesh, string nomeFile);
    bool ExportCell2Ds(Mesh& mesh, string nomeFile);
}

#endif // __EMPTY_H
