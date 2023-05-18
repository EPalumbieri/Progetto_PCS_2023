#include "empty_class.hpp"

namespace ProjectLibrary
{
    bool clockwise(const Point& P0,
                   const Point& P1,
                   const Point& P2)
    {
      return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y)>0;
    }

    double area(const Point& P1,
                const Point& P2,
                const Point& P3)
    {
        return abs((P1.x*(P2.y-P3.y)+P2.x*(P3.y-P1.y)+P3.x*(P1.y-P2.y))/2)
    }

    int GetEdgeFromPoints(int id1, int id2)
    {
        it = std::find_if(mesh.GraphedMesh.begin(), mesh.GraphedMesh.end(),
        [&triangle](const OrientedEdge* edge) { return edge->RealEdge == triangle.edges[i]; });
    }
    // ***************************************************************************
    bool ImportCell0Ds(TriangularMesh& mesh, string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
        return false;

      list<string> listLines;
      string line;
      while (getline(file, line))
        listLines.push_back(line);

      file.close();

      listLines.pop_front();

      mesh.NumberCell0D = listLines.size();

      if (mesh.NumberCell0D == 0)
      {
        cerr << "There is no cell 0D" << endl;
        return false;
      }


      for (const string& line : listLines)
      {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;
        Point coordinates;
        converter >>  id >> marker >> coord(0) >> coord(1);
        coordinates.x=coord(0);
        coordinates.y=coord(1);
        mesh.Cell0D.insert(id,coordinates);

        if( marker != 0)
        {
          if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            mesh.Cell0DMarkers.insert({marker, {id}});
          else
            mesh.Cell0DMarkers[marker].push_back(id);
        }

      }
      file.close();
      return true;
    }

    // ***************************************************************************
    bool ImportCell1Ds(TriangularMesh& mesh, string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
        return false;

      list<string> listLines;
      string line;
      while (getline(file, line))
        listLines.push_back(line);

      listLines.pop_front();

      mesh.NumberCell1D = listLines.size();

      if (mesh.NumberCell1D == 0)
      {
        cerr << "There is no cell 1D" << endl;
        return false;
      }


      for (const string& line : listLines)
      {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
        Edge edge;

        converter >>  id >> marker >> vertices(0) >> vertices(1);
        edge.points.insert(vertices(0));
        edge.points.insert(vertices(1));
        mesh.Cell1D.insert(id,edge);

        if( marker != 0)
        {
          if (mesh.Cell1DMarkers.find(marker) == mesh.Cell1DMarkers.end())
            mesh.Cell1DMarkers.insert({marker, {id}});
          else
            mesh.Cell1DMarkers[marker].push_back(id);
        }
      }

      file.close();

      return true;
    }
    // ***************************************************************************
    bool ImportCell2Ds(TriangularMesh& mesh,string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
        return false;

      list<string> listLines;
      string line;
      while (getline(file, line))
        listLines.push_back(line);

      listLines.pop_front();

      mesh.NumberCell2D = listLines.size();

      if (mesh.NumberCell2D == 0)
      {
        cerr << "There is no cell 2D" << endl;
        return false;
      }



      for (const string& line : listLines)
      {
        istringstream converter(line);

        Triangle triangle;
        array<OrientedEdge*,3> ptrs;
        array<OrientedEdge,3> orientededges;
        converter >> id;
        for(unsigned int i = 0; i < 3; i++)
          converter >> triangle.vertices[i];
        for(unsigned int i = 0; i < 3; i++){
          ptrs[i]=&orientededges[i];
          converter >> triangle.edges[i];
          ptrs[i]->RealEdge=triangle.edges[i];
          ptrs[i]->RealTriangle=id;
          it = std::find_if(mesh.GraphedMesh.begin(), mesh.GraphedMesh.end(),
          [&triangle](const OrientedEdge* edge) { return edge->RealEdge == triangle.edges[i]; });

          if(it != mesh.GraphedMesh.end()){
             //found
             ptrs[i]->symmetric=*it;
             (*it)->symmetric=ptrs[i];
          }else {
            //not found

          }
        }
        triangle.area=area(mesh.Cell0D[triangle.vertices[0]],mesh.Cell0D[triangle.vertices[1]],mesh.Cell0D[triangle.vertices[2]]);

        unordered_set<unsigned int> temp01, temp02, temp12;
        temp01.insert(triangle.vertices[0]);
        temp01.insert(triangle.vertices[1]);
        temp02.insert(triangle.vertices[0]);
        temp02.insert(triangle.vertices[2]);
        temp12.insert(triangle.vertices[1]);
        temp12.insert(triangle.vertices[2]);
        unsigned int e01, e02, e12;

        if ((mesh.Cell1D[triangle.edge[0]].points==temp01)){
            e01=0;
        }else if (mesh.Cell1D[triangle.edge[1]].points==temp01){
            e01=1;
        }else{
            e01=2;
        }

        if ((mesh.Cell1D[triangle.edge[0]].points==temp02)){
            e02=0;
        }else if (mesh.Cell1D[triangle.edge[1]].points==temp02){
            e02=1;
        }else{
            e02=2;
        }

        if ((mesh.Cell1D[triangle.edge[0]].points==temp12)){
            e12=0;
        }else if (mesh.Cell1D[triangle.edge[1]].points==temp12){
            e12=1;
        }else{
            e12=2;
        }

        if(clockwise(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2])){
            //e01->e12->e02
            ptrs[e01]->next=ptrs[e12];
            ptrs[e12]->next=ptrs[e02];
            ptrs[e02]->next=ptrs[e01];
        }else{
           //e02->e12->e01
            ptrs[e02]->next=ptrs[e12];
            ptrs[e12]->next=ptrs[e01];
            ptrs[e01]->next=ptrs[e02];
        }

        mesh.Cell2D.push_back(triangle);
        for(int i=0;i<3;i++){
           mesh.GraphedMesh.push_back(ptrs[i]);
        }
      }
      file.close();
      return true;
    }

    //***************************************************************************
    bool ImportMesh(TriangularMesh& mesh, string file0D, string file1D, string file2D)
    {

      if(!ImportCell0Ds(mesh,file0D))
      {
        return false;
      }

      if(!ImportCell1Ds(mesh,file1D))
      {
        return false;
      }

      if(!ImportCell2Ds(mesh,file2D))
      {
        return false;
      }

      return true;

    }
}
