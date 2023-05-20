#include "empty_class.hpp"

namespace ProjectLibrary
{
    bool clockwise(Mesh& mesh, unsigned int id1, unsigned int id2, unsigned int id3)
    {
      Point P0=Point(mesh.Cell0D[id1]);
      Point P1=Point(mesh.Cell0D[id2]);
      Point P2=Point(mesh.Cell0D[id3]);
      return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y)>0;
    }

    double area(const Point& P1,
                const Point& P2,
                const Point& P3)
    {
        return abs((P1.x*(P2.y-P3.y)+P2.x*(P3.y-P1.y)+P3.x*(P1.y-P2.y))/2);
    }

    // ***************************************************************************
    bool ImportCell0Ds(Mesh& mesh, string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
      {
        cerr<<"wrong file";
        return false;
      }
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
        converter >>  id >> marker >> coord(0) >> coord(1);
        mesh.Cell0D[id]=Point(coord(0),coord(1));

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
    bool ImportCell1Ds(Mesh& mesh, string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
      {
        cerr<<"wrong file";
        return false;
      }

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
        mesh.Cell1D[id]=edge;

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
    bool ImportCell2Ds(Mesh& mesh,string nomeFile)
    {

      ifstream file;
      file.open(nomeFile);

      if(file.fail())
      {
        cerr<<"wrong file";
        return false;
      }

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

      unsigned int key;
      vector<OrientedEdge*> orientededges;
      unsigned int k=0;
      for (const string& line : listLines)
      {
        istringstream converter(line);

        Triangle triangle;
        unsigned int id;
        converter >> id;
        for(unsigned int i = 0; i < 3; i++)
          converter >> triangle.vertices[i];
        for(unsigned int i = 0; i < 3; i++){
          converter >> triangle.edges[i];
          OrientedEdge* orientededge = new OrientedEdge(triangle.edges[i],id);
          orientededges.push_back(orientededge);
          k++;
        }
        triangle.area=ProjectLibrary::area(mesh.Cell0D[triangle.vertices[0]],mesh.Cell0D[triangle.vertices[1]],mesh.Cell0D[triangle.vertices[2]]);

        std::unordered_set<unsigned int> temp01, temp02, temp12;
        temp01.insert(triangle.vertices[0]);
        temp01.insert(triangle.vertices[1]);
        temp02.insert(triangle.vertices[0]);
        temp02.insert(triangle.vertices[2]);
        temp12.insert(triangle.vertices[1]);
        temp12.insert(triangle.vertices[2]);
        unsigned int e01, e02, e12;

        if ((mesh.Cell1D[triangle.edges[0]].points==temp01)){
            e01=k-3;
        }else if (mesh.Cell1D[triangle.edges[1]].points==temp01){
            e01=k-2;
        }else{
            e01=k-1;
        }

        if ((mesh.Cell1D[triangle.edges[0]].points==temp02)){
            e02=k-3;
        }else if (mesh.Cell1D[triangle.edges[1]].points==temp02){
            e02=k-2;
        }else{
            e02=k-1;
        }

        if ((mesh.Cell1D[triangle.edges[0]].points==temp12)){
            e12=k-3;
        }else if (mesh.Cell1D[triangle.edges[1]].points==temp12){
            e12=k-2;
        }else{
            e12=k-1;
        }

        if(ProjectLibrary::clockwise(mesh,triangle.vertices[0],triangle.vertices[1],triangle.vertices[2])){
            //e01->e12->e02
            orientededges[e01]->next=orientededges[e12];
            orientededges[e12]->next=orientededges[e02];
            orientededges[e02]->next=orientededges[e01];
        }else{
           //e02->e12->e01
            orientededges[e02]->next=orientededges[e12];
            orientededges[e12]->next=orientededges[e01];
            orientededges[e01]->next=orientededges[e02];
        }
        mesh.Cell2D[id]=triangle;

        for(unsigned int i=k-3;i<k;i++){
            key=triangle.edges[i%3];
            auto it = std::find_if(orientededges.begin(), orientededges.end()-3,
             [&key](const OrientedEdge* edge) { return edge->RealEdge == key; });

            if(it != orientededges.end()-3){
               //found
               orientededges[i]->symmetric=(*it);
               (*it)->symmetric=orientededges[i];
            }else {
              //not found
            }
        }
      }
      mesh.GraphedMesh=orientededges;
      file.close();
      return true;
    }

    inline double distance(const Point& p1, const Point& p2)
    {
      return (sqrt(normSquared(p1.x - p2.x, p1.y - p2.y)));
    }

    double length(Mesh& mesh,OrientedEdge* edge){
        auto a=mesh.Cell1D[edge->RealEdge].points;
        auto p1= mesh.Cell0D[*a.begin()];
        auto p2= mesh.Cell0D[*a.end()];
        return distance(p1,p2);
    }

    bool isLongest(Mesh& mesh,OrientedEdge* edge){
        double edgelength=length(mesh, edge);
        double max=edgelength;
        for(int i=0; i<2; i++){
            edge=edge->next;
            double ltemp = length(mesh,edge);
            max = (max<ltemp) ? ltemp : max;
        }
        return (max==edgelength);
    }

    Point midpoint(const Point& p1, const Point& p2){
        return(Point((p1.x+p2.x)/2,(p1.y+p2.y)/2));
    }


    bool bisectNoSymmetric(Mesh& mesh,OrientedEdge* edge){
        auto a=mesh.Cell1D[edge->RealEdge].points;
        unsigned int idP1= *a.begin();
        unsigned int idP2= *(++a.begin());
        Point p1= mesh.Cell0D[idP1];
        Point p2= mesh.Cell0D[idP2];
        Point pmiddle=midpoint(p1,p2);
        unsigned int idPmiddle=mesh.NumberCell0D;
        mesh.NumberCell0D++;
        auto PointsNext=mesh.Cell1D[edge->next->RealEdge].points;
        auto itN1 = PointsNext.find(idP1);
        auto itN2 = PointsNext.find(idP2);
        unsigned int idENext, idEPrec;
        idENext=edge->next->RealEdge;
        idEPrec=edge->next->next->RealEdge;
        Edge e1m,e2m,eNm;
        e1m=Edge(unordered_set<unsigned int> ({idP1, idPmiddle}));
        unsigned int ide1m=mesh.NumberCell1D;
        mesh.NumberCell1D++;
        e2m=Edge(unordered_set<unsigned int> ({idP2, idPmiddle}));
        unsigned int ide2m=mesh.NumberCell1D;
        mesh.NumberCell1D++;
        unsigned int idPNext;
        if(itN1 != PointsNext.end()){
            if (itN1 == PointsNext.begin()){
                idPNext= *(++PointsNext.begin());
            }else{
                idPNext= *(PointsNext.begin());
            }
        }else{
            if (itN2 == PointsNext.begin()){
                idPNext= *(++PointsNext.begin());
            }else{
                idPNext= *(PointsNext.begin());
            }
        }
        Point pNext =mesh.Cell0D[idPNext];
        eNm=Edge(unordered_set<unsigned int> ({idPNext, idPmiddle}));
        unsigned int ideNm=mesh.NumberCell1D;
        mesh.NumberCell1D++;

        Triangle t1,t2;
        unsigned int idT1=mesh.NumberCell2D;
        mesh.NumberCell2D++;
        unsigned int idT2=mesh.NumberCell2D;
        mesh.NumberCell2D++;
        OrientedEdge* oe1m = new OrientedEdge(ide1m,idT1);
        OrientedEdge* oe2m = new OrientedEdge(ide2m,idT2);
        OrientedEdge* oe1Nm = new OrientedEdge(ideNm,idT1);
        OrientedEdge* oe2Nm = new OrientedEdge(ideNm,idT2);
        oe1Nm->symmetric=oe2Nm;
        oe2Nm->symmetric=oe1Nm;
        double area0, area1;
        area0=area(p1,pNext,pmiddle);
        area1=area(p2,pNext,pmiddle);
        if(ProjectLibrary::clockwise(mesh,idP1,idPNext,idP2)){
            t1.vertices={idP1,idPNext,idPmiddle};
            t2.vertices={idP2,idPNext,idPmiddle};
            t1.edges={idENext,ideNm,ide1m};
            t2.edges={idEPrec,ideNm, ide2m};
            t1.area=area0;
            t2.area=area1;
            OrientedEdge* NextTr2=edge->next->next;
            edge->next->next=oe1Nm;
            oe1Nm->next=oe1m;
            oe1m->next=edge->next;
            NextTr2->next=oe2m;
            oe2m->next=oe2Nm;
            oe2Nm->next=NextTr2;
        }else{
            t1.vertices={idP1,idPNext,idPmiddle};
            t2.vertices={idP2,idPNext,idPmiddle};
            t1.edges={idEPrec,ideNm,ide1m};
            t2.edges={idENext,ideNm, ide2m};
            t1.area=area1;
            t2.area=area0;
            OrientedEdge* NextTr2=edge->next;
            oe1m->next=oe1Nm;
            oe1Nm->next= edge->next->next;
            edge->next->next=oe1m;
            oe2m->next=NextTr2;
            NextTr2->next=oe2Nm;
            oe2Nm->next=oe2m;
        }
        mesh.Cell0D[idPmiddle]=pmiddle;
        mesh.Cell1D[ide1m]=e1m;
        mesh.Cell1D[ide2m]=e2m;
        mesh.Cell1D[ideNm]=eNm;
        mesh.Cell2D[idT1]=t1;
        mesh.Cell2D[idT2]=t2;
        mesh.GraphedMesh.push_back(oe1m);
        mesh.GraphedMesh.push_back(oe2m);
        mesh.GraphedMesh.push_back(oe1Nm);
        mesh.GraphedMesh.push_back(oe2Nm);
        mesh.Cell1D.erase(edge->RealEdge);
        mesh.Cell2D.erase(edge->RealTriangle);
        auto it= find(mesh.GraphedMesh.begin(),mesh.GraphedMesh.begin(),edge);
        mesh.GraphedMesh.erase(it);
        delete edge;
        return true;
    }

    //***************************************************************************
    bool ImportMesh(Mesh& mesh, string file0D, string file1D, string file2D)
    {

      if(!ImportCell0Ds(mesh,file0D))
      {
        cerr<<"FailedCell0";
        return false;
      }

      if(!ImportCell1Ds(mesh,file1D))
      {
        cerr<<"FailedCell1";
        return false;
      }

      if(!ImportCell2Ds(mesh,file2D))
      {
        cerr<<"FailedCell2";
        return false;
      }

      return true;

    }
}
