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

    unsigned int findThirdVertex(unordered_set<unsigned int> PointsNext, unsigned int idP1, unsigned int idP2){
        auto itN1 = PointsNext.find(idP1);
        auto itN2 = PointsNext.find(idP2);
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
        return idPNext;
    }
    bool bisect(Mesh& mesh,OrientedEdge* edge){
        auto a=mesh.Cell1D[edge->RealEdge].points;
        unsigned int idP1= *a.begin();
        unsigned int idP2= *(++a.begin());
        Point p1= mesh.Cell0D[idP1];
        Point p2= mesh.Cell0D[idP2];
        Point pmiddle=midpoint(p1,p2);
        unsigned int idPmiddle=mesh.NumberCell0D;
        mesh.NumberCell0D++;
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

        auto PointsNext=mesh.Cell1D[edge->next->RealEdge].points;
        auto idPNext = findThirdVertex(PointsNext,idP1,idP2);
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
        t1.vertices={idP1,idPNext,idPmiddle};
        t2.vertices={idP2,idPNext,idPmiddle};
        t1.area=area(p1,pNext,pmiddle);
        t2.area=area(p2,pNext,pmiddle);
        bool orientation = ProjectLibrary::clockwise(mesh,idP1,idPNext,idP2);
        if(orientation){
            t1.edges={idENext,ideNm,ide1m};
            t2.edges={idEPrec,ideNm, ide2m};

            oe2m->next=oe2Nm;
            oe2Nm->next=edge->next->next;
            edge->next->next->next=oe2m;

            edge->next->next=oe1Nm;
            oe1Nm->next=oe1m;
            oe1m->next=edge->next;

        }else{
            t1.edges={idEPrec,ideNm,ide1m};
            t2.edges={idENext,ideNm, ide2m};

            oe1m->next=oe1Nm;
            oe1Nm->next= edge->next->next;
            edge->next->next->next=oe1m;

            oe2m->next=edge->next;
            edge->next->next=oe2Nm;
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

        if (edge->symmetric==nullptr){
            mesh.Cell1D.erase(edge->RealEdge);
            mesh.Cell2D.erase(edge->RealTriangle);
            auto it= find(mesh.GraphedMesh.begin(),mesh.GraphedMesh.begin(),edge);
            mesh.GraphedMesh.erase(it);
            delete edge;
            return true;
        }else{
            auto PointsNext=mesh.Cell1D[edge->symmetric->next->RealEdge].points;
            auto idPPrec = findThirdVertex(PointsNext,idP1,idP2);
            Point pPrec = mesh.Cell0D[idPPrec];
            Edge ePm = Edge(unordered_set<unsigned int> ({idPPrec, idPmiddle}));
            unsigned int idePm=mesh.NumberCell1D;
            mesh.NumberCell1D++;
            Triangle t3,t4;
            unsigned int idT3=mesh.NumberCell2D;
            mesh.NumberCell2D++;
            unsigned int idT4=mesh.NumberCell2D;
            mesh.NumberCell2D++;
            unsigned int idEPNext, idEPPrec;
            idEPNext=edge->symmetric->next->RealEdge;
            idEPPrec=edge->symmetric->next->next->RealEdge;
            OrientedEdge* oeP1m = new OrientedEdge(ide1m,idT3);
            OrientedEdge* oeP2m = new OrientedEdge(ide2m,idT4);
            OrientedEdge* oeP1Pm = new OrientedEdge(idePm,idT3);
            OrientedEdge* oeP2Pm = new OrientedEdge(idePm,idT4);
            oeP1Pm->symmetric=oeP2Pm;
            oeP2Pm->symmetric=oeP1Pm;
            t3.area=area(p1,pPrec,pmiddle);
            t4.area=area(p2,pPrec,pmiddle);
            t3.vertices={idP1,idPPrec,idPmiddle};
            t4.vertices={idP2,idPPrec,idPmiddle};
            if(orientation){
                t3.edges={idEPPrec,idePm,ide1m};
                t4.edges={idEPNext,idePm, ide2m};

                oeP1m->next= oeP1Pm;
                oeP1Pm->next=edge->next->next;
                edge->next->next->next=oeP1m;

                oeP2m->next=edge->next;
                edge->next->next=oeP2Pm;
                oeP2Pm->next=oeP2m;
            }else{
                t3.edges={idEPNext,idePm,ide1m};
                t4.edges={idEPPrec,idePm, ide2m};

                oeP2m->next=oeP2Pm;
                oeP2Pm->next=edge->next->next;
                edge->next->next->next=oeP2m;

                oeP1m->next=edge->next;
                edge->next->next=oeP1Pm;
                oeP1Pm->next=oeP1m;
            }
            oeP1m->symmetric=oe1m;
            oe1m->symmetric=oeP1m;
            oeP2m->symmetric=oe2m;
            oe2m->symmetric=oeP2m;

            mesh.Cell1D[idePm]=ePm;
            mesh.Cell2D[idT3]=t3;
            mesh.Cell2D[idT4]=t4;
            mesh.GraphedMesh.push_back(oeP1m);
            mesh.GraphedMesh.push_back(oeP2m);
            mesh.GraphedMesh.push_back(oeP1Pm);
            mesh.GraphedMesh.push_back(oeP2Pm);

            mesh.Cell1D.erase(edge->RealEdge);
            mesh.Cell2D.erase(edge->RealTriangle);
            mesh.Cell1D.erase(edge->symmetric->RealEdge);
            mesh.Cell2D.erase(edge->symmetric->RealTriangle);
            auto it= find(mesh.GraphedMesh.begin(),mesh.GraphedMesh.begin(),edge->symmetric);
            mesh.GraphedMesh.erase(it);
            delete edge->symmetric;
            return true;
            it= find(mesh.GraphedMesh.begin(),mesh.GraphedMesh.begin(),edge);
            mesh.GraphedMesh.erase(it);
            delete edge;
            return true;
        }
    }

    //***************************************************************************
    bool ImportMesh(Mesh& mesh, string file0D, string file1D, string file2D)
    {

      if(!ImportCell0Ds(mesh,file0D))
      {
        cerr<<"FailedImportCell0";
        return false;
      }

      if(!ImportCell1Ds(mesh,file1D))
      {
        cerr<<"FailedImportCell1";
        return false;
      }

      if(!ImportCell2Ds(mesh,file2D))
      {
        cerr<<"FailedImportCell2";
        return false;
      }

      return true;

    }

    bool ExportMesh(Mesh &mesh, string file0D, string file1D, string file2D)
    {
        if(!ExportCell0Ds(mesh,file0D))
        {
          cerr<<"FailedExportCell0";
          return false;
        }

        if(!ExportCell1Ds(mesh,file1D))
        {
          cerr<<"FailedExportCell1";
          return false;
        }

        if(!ExportCell2Ds(mesh,file2D))
        {
          cerr<<"FailedExportCell2";
          return false;
        }

        return true;

    }

    bool ExportCell0Ds(Mesh &mesh, string nomeFile)
    {
        ofstream file;
        file.open(nomeFile);

        if(file.fail())
        {
          cerr<<"wrong file";
          return false;
        }

        file<<"Id X Y\n";
                for(unsigned int i=0;i<mesh.Cell0D.size();i++)
                {
                 Point p=mesh.Cell0D[i];
                 file<<i<<" "<<p.x<<" "<<p.y<<"\n";
                }
         return true;
    }

    bool ExportCell1Ds(Mesh &mesh, string nomeFile)
    {
        ofstream file;
        file.open(nomeFile);

        if(file.fail())
        {
          cerr<<"wrong file";
          return false;
        }

        file<<"Id Origin End\n";
                for(unsigned int i=0;i<mesh.Cell1D.size();i++)
                {
                 Edge e=mesh.Cell1D[i];
                 file<<i;
                 for(auto it=e.points.begin();it!= e.points.end();it++)
                  file<<" "<<*it;
                 file<<"\n";
                }
         return true;
    }
    bool ExportCell2Ds(Mesh &mesh, string nomeFile)
    {
        ofstream file;
        file.open(nomeFile);

        if(file.fail())
        {
          cerr<<"wrong file";
          return false;
        }

        file<<"Id Vertices Edges\n";
                for(unsigned int i=0;i<mesh.Cell2D.size();i++)
                {
                 Triangle t=mesh.Cell2D[i];
                 file<<i<<" "<<t.vertices[0]<<" "<<t.vertices[1]<<" "<<t.vertices[2]
                        <<" "<<t.edges[0]<<" "<<t.edges[1]<<" "<<t.edges[2]<<"\n";
                }
         return true;
    }

}
