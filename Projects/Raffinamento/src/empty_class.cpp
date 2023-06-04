#include "empty_class.hpp"

namespace ProjectLibrary
{
    unsigned int Mesh::biggestEdge(array<unsigned int,3> edges){
        double maxx=-1;
        double tmpp=0;
        unsigned int idMax = 0;
        for(unsigned int i = 0; i < 3; i++){
          tmpp=this->Cell1D[edges[i]].lenght;
          if(maxx<tmpp){
              maxx=tmpp;
              idMax=edges[i];
          }
        }
        return idMax;
    }

    void Mesh::clearEdgeData(OrientedEdge * edge)
    {

        if(edge->symmetric!=nullptr)
        {
        this->alreadyBisected[edge->symmetric->RealTriangle]=true;
        this->Cell2D.erase(edge->symmetric->RealTriangle);

        this->GraphedMesh.erase(find(GraphedMesh.begin(),GraphedMesh.end(),edge->symmetric));
        delete edge->symmetric;
        }

        this->Cell1D.erase(edge->RealEdge);
        this->GraphedMesh.erase(find(GraphedMesh.begin(),GraphedMesh.end(),edge));
        this->alreadyBisected[edge->RealTriangle]=true;
        this->Cell2D.erase(edge->RealTriangle);
        delete edge;
    }

//    bool clockwise(Mesh& mesh, unsigned int id1, unsigned int id2, unsigned int id3)
//    {
//      Point P0=Point(mesh.Cell0D[id1]);
//      Point P1=Point(mesh.Cell0D[id2]);
//      Point P2=Point(mesh.Cell0D[id3]);
//      return !((P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y)>0);
//    }

    inline bool clockwise(Point& P0,Point& P1, Point& P2)
    {
      return !((P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y)>0);
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
        cerr<<"wrong file 1D";
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

       unsigned int id;
      for (const string& line : listLines)
      {
        istringstream converter(line);

        unsigned int marker;
        Vector2i vertices;


        converter >>  id >> marker >> vertices(0) >> vertices(1);
        Edge edge=(unordered_set<unsigned int> ({(unsigned int) vertices(0),(unsigned int) vertices(1)}));
        edge.lenght=distance(mesh.Cell0D[vertices(0)],mesh.Cell0D[vertices(1)]);
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
      mesh.NumberCell2DInitial=mesh.NumberCell2D;

      mesh.alreadyBisected.resize(mesh.NumberCell2DInitial);

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
        triangle.longestEdge=mesh.biggestEdge(triangle.edges);
        triangle.area=ProjectLibrary::area(mesh.Cell0D[triangle.vertices[0]],mesh.Cell0D[triangle.vertices[1]],mesh.Cell0D[triangle.vertices[2]]);
        if (mesh.StartingTriangles.find(triangle.area) == mesh.StartingTriangles.end())
          mesh.StartingTriangles.insert({triangle.area, {id}});
        else
          mesh.StartingTriangles[triangle.area].push_back(id);

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

        if(clockwise(mesh.Cell0D[triangle.vertices[0]],mesh.Cell0D[triangle.vertices[1]],mesh.Cell0D[triangle.vertices[2]])){
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
        mesh.GraphedMesh=orientededges;
      }
      file.close();
      return true;
    }

    double distance(const Point& p1, const Point& p2)
    {
      return (sqrt(normSquared(p1.x - p2.x, p1.y - p2.y)));
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

    OrientedEdge* Mesh::getOrientedEdge( unsigned int idTriangle, unsigned int idEdge){

        for ( auto it=this->GraphedMesh.begin();it!=this->GraphedMesh.end();it++){
            if(((*it)->RealTriangle==idTriangle)&&((*it)->RealEdge==idEdge)){
                return (*it);
            }
        }
        return nullptr;
    }

    bool Mesh::bisect(OrientedEdge* edge){
        auto a=   this->Cell1D[edge->RealEdge].points;
        unsigned int idP1= *a.begin();
        unsigned int idP2= *(++a.begin());
        Point p1=    this->Cell0D[idP1];
        Point p2=    this->Cell0D[idP2];
        Point pmiddle=midpoint(p1,p2);
        unsigned int idPmiddle=   this->NumberCell0D;
           this->NumberCell0D++;
        unsigned int idENext, idEPrec;
        idENext=edge->next->RealEdge;
        idEPrec=edge->next->next->RealEdge;
        Edge e1m,e2m,eNm;
        e1m=Edge(unordered_set<unsigned int> ({idP1, idPmiddle}));
        e1m.lenght=distance(p1,pmiddle);
        unsigned int ide1m=   this->NumberCell1D;
           this->NumberCell1D++;
        e2m=Edge(unordered_set<unsigned int> ({idP2, idPmiddle}));
        e2m.lenght=distance(p2,pmiddle);
        unsigned int ide2m=   this->NumberCell1D;
           this->NumberCell1D++;

        unordered_set<unsigned int> PointsNext=   this->Cell1D[edge->next->RealEdge].points;
        unsigned int idPNext = findThirdVertex(PointsNext,idP1,idP2);
        Point pNext =   this->Cell0D[idPNext];
        eNm=Edge(unordered_set<unsigned int> ({idPNext, idPmiddle}));
        eNm.lenght=distance(pNext,pmiddle);
        unsigned int ideNm=   this->NumberCell1D;
           this->NumberCell1D++;

        Triangle t1,t2;
           this->alreadyBisected.push_back(false);
           this->alreadyBisected.push_back(false);
        unsigned int idT1=   this->NumberCell2D;
           this->NumberCell2D++;
        unsigned int idT2=   this->NumberCell2D;
           this->NumberCell2D++;
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
        bool orientation = clockwise(Cell0D[idP1],Cell0D[idPNext],Cell0D[idP2]);
        if(orientation){
            edge->next->RealTriangle=idT1;
            edge->next->next->RealTriangle=idT2;

            t1.edges={idENext,ideNm,ide1m};
            t2.edges={idEPrec,ideNm, ide2m};

            oe2m->next=oe2Nm;
            oe2Nm->next=edge->next->next;
            edge->next->next->next=oe2m;

            edge->next->next=oe1Nm;
            oe1Nm->next=oe1m;
            oe1m->next=edge->next;

        }else{
            edge->next->next->RealTriangle=idT1;
            edge->next->RealTriangle=idT2;

            t1.edges={idEPrec,ideNm,ide1m};
            t2.edges={idENext,ideNm, ide2m};

            oe1m->next=oe1Nm;
            oe1Nm->next= edge->next->next;
            edge->next->next->next=oe1m;

            oe2m->next=edge->next;
            edge->next->next=oe2Nm;
            oe2Nm->next=oe2m;
        }
           this->Cell0D[idPmiddle]=pmiddle;
           this->Cell1D[ide1m]=e1m;
           this->Cell1D[ide2m]=e2m;
           this->Cell1D[ideNm]=eNm;
        t1.longestEdge=   this->biggestEdge(t1.edges);
        t2.longestEdge=   this->biggestEdge(t2.edges);
           this->Cell2D[idT1]=t1;
           this->Cell2D[idT2]=t2;
           this->GraphedMesh.push_back(oe1m);
           this->GraphedMesh.push_back(oe2m);
           this->GraphedMesh.push_back(oe1Nm);
           this->GraphedMesh.push_back(oe2Nm);

        if (edge->symmetric==nullptr){
            return true;
        }else{
            auto PointsNext=   this->Cell1D[edge->symmetric->next->RealEdge].points;
            auto idPPrec = findThirdVertex(PointsNext,idP1,idP2);
            Point pPrec =    this->Cell0D[idPPrec];
            Edge ePm = Edge(unordered_set<unsigned int> ({idPPrec, idPmiddle}));
            ePm.lenght=distance(pPrec,pmiddle);
            unsigned int idePm=   this->NumberCell1D;
               this->NumberCell1D++;
            Triangle t3,t4;
               this->alreadyBisected.push_back(false);
               this->alreadyBisected.push_back(false);
            unsigned int idT3=   this->NumberCell2D;
               this->NumberCell2D++;
            unsigned int idT4=   this->NumberCell2D;
               this->NumberCell2D++;
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
                edge->symmetric->next->RealTriangle=idT4;
                edge->symmetric->next->next->RealTriangle=idT3;

                t3.edges={idEPPrec,idePm,ide1m};
                t4.edges={idEPNext,idePm, ide2m};

                oeP1m->next= oeP1Pm;
                oeP1Pm->next=edge->symmetric->next->next;
                edge->symmetric->next->next->next=oeP1m;

                oeP2m->next=edge->symmetric->next;
                edge->symmetric->next->next=oeP2Pm;
                oeP2Pm->next=oeP2m;
            }else{
                edge->symmetric->next->RealTriangle=idT3;
                edge->symmetric->next->next->RealTriangle=idT4;

                t3.edges={idEPNext,idePm,ide1m};
                t4.edges={idEPPrec,idePm, ide2m};

                oeP2m->next=oeP2Pm;
                oeP2Pm->next=edge->symmetric->next->next;
                edge->symmetric->next->next->next=oeP2m;

                oeP1m->next=edge->symmetric->next;
                edge->symmetric->next->next=oeP1Pm;
                oeP1Pm->next=oeP1m;
            }
               this->Cell1D[idePm]=ePm;
            t3.longestEdge=   this->biggestEdge(t3.edges);
            t4.longestEdge=   this->biggestEdge(t4.edges);

            oeP1m->symmetric=oe1m;
            oe1m->symmetric=oeP1m;
            oeP2m->symmetric=oe2m;
            oe2m->symmetric=oeP2m;

               this->Cell2D[idT3]=t3;
               this->Cell2D[idT4]=t4;
               this->GraphedMesh.push_back(oeP1m);
               this->GraphedMesh.push_back(oeP2m);
               this->GraphedMesh.push_back(oeP1Pm);
               this->GraphedMesh.push_back(oeP2Pm);
        }
        return true;
    }

    void Mesh::Globalrefine(int numberTriagles)
    {

        int j=0;
        for (auto it=   this->StartingTriangles.begin(); it!=    this->StartingTriangles.end();it++) {

            for (auto it2=(*(it)).second.begin();it2!=(*(it)).second.end();it2++){
                if(j==numberTriagles)return;
                j++;
                unsigned int triangle= (*it2);
                if(!   this->alreadyBisected[triangle]&&triangle<   this->NumberCell2DInitial){
                refine(getOrientedEdge(triangle,   this->Cell2D[triangle].longestEdge));
                }

            }
        }
    }

    void Mesh::refine( OrientedEdge* lEdge)
    {
        unsigned int triangle=lEdge->RealTriangle;
        cout<<"refining triangle:"<<triangle<<endl;
        if(lEdge->symmetric==nullptr)
        {
            cout<<"borderTriangle\n\n";

            bisect(lEdge);
            clearEdgeData(lEdge);
//            this->alreadyBisected[triangle]=true;
//            this->Cell2D.erase(triangle);
//            this->Cell1D.erase(lEdge->RealEdge);
//            this->GraphedMesh.erase(find(GraphedMesh.begin(),GraphedMesh.end(),lEdge));
//            delete lEdge;
            return;
        }
        else
            {
                cout<<"next"<<lEdge->symmetric->RealTriangle<<endl;
                OrientedEdge* nextLongest=getOrientedEdge(lEdge->symmetric->RealTriangle,this->Cell2D[lEdge->symmetric->RealTriangle].longestEdge);
                if (nextLongest->symmetric==lEdge)
                {
                    cout<<"mustBisectHere into"<<lEdge->symmetric->RealTriangle<<"\n\n";

                    bisect(lEdge);
                    clearEdgeData(lEdge);
//                    this->alreadyBisected[triangle]=true;
//                    this->Cell2D.erase(triangle);

//                    this->alreadyBisected[lEdge->symmetric->RealTriangle]=true;
//                    this->Cell2D.erase(lEdge->symmetric->RealTriangle);

//                    this->Cell1D.erase(lEdge->RealEdge);
//                    this->GraphedMesh.erase(find(GraphedMesh.begin(),GraphedMesh.end(),lEdge));
//                    this->GraphedMesh.erase(find(GraphedMesh.begin(),GraphedMesh.end(),lEdge->symmetric));
//                    delete lEdge->symmetric;
//                    delete lEdge;
//
                    return;
                }
                else
                {
                    //if(!mesh.alreadyBisected[nextLongest->RealTriangle])
                      refine(nextLongest);
                    cout<<"Returning to"<<lEdge->RealTriangle;
                    //if(!mesh.alreadyBisected[lEdge->RealTriangle])
                      refine(lEdge);
                    return;
                }

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
        for(auto it=mesh.Cell1D.begin();it!=mesh.Cell1D.end();it++)
            {
                file<<(*it).first;
                file<<" "<<(*(*it).second.points.begin())<<" "<<(*++(*it).second.points.begin());
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
          cerr<<"wrong file ";
          return false;
        }

        file<<"Id Vertices Edges\n";
                for(auto it=mesh.Cell2D.begin();it!=mesh.Cell2D.end();it++)
                {
                 Triangle t=(*it).second;
                 //if(t.edges[0]!=0 && t.edges[1]!=0 && t.edges[2]!=0)
                   file<<(*it).first<<" "<<t.vertices[0]<<" "<<t.vertices[1]<<" "<<t.vertices[2]
                        <<" "<<t.edges[0]<<" "<<t.edges[1]<<" "<<t.edges[2]<<"\n";
                }
         return true;
    }
}
