#include "empty_class.hpp"

namespace ProjectLibrary
{
    OrientedEdge* biggestEdge(Mesh& mesh, vector<OrientedEdge*> edges){
        double maxx=-1;
        double tmpp=0;
        OrientedEdge* Max = nullptr;
        for(unsigned int i = 0; i < 3; i++){
          tmpp=(mesh.Cell1D[edges[i]->RealEdge])->lenght;
          if(maxx<tmpp){
              maxx=tmpp;
              Max=edges[i];
          }
        }
        return Max;
    }

    bool clockwise(Mesh& mesh, unsigned int id1, unsigned int id2, unsigned int id3)
    {
      Point P0=Point(*mesh.Cell0D[id1]);
      Point P1=Point(*mesh.Cell0D[id2]);
      Point P2=Point(*mesh.Cell0D[id3]);
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
        Point *p1= new Point(coord(0),coord(1));
        mesh.Cell0D.push_back(p1);

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
      mesh.alreadyBisectedEdge.resize(mesh.NumberCell1D*5);


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
        Edge *edge= new Edge(unordered_set<unsigned int> ({(unsigned int) vertices(0),(unsigned int) vertices(1)}));
        edge->lenght=distance(*mesh.Cell0D[vertices(0)],*mesh.Cell0D[vertices(1)]);
        mesh.Cell1D.push_back(edge);

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
      mesh.Cell2D.reserve(mesh.NumberCell2D*100);

      mesh.alreadyBisected.resize(mesh.NumberCell2DInitial*10);

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

        Triangle *triangle = new Triangle();
        unsigned int id;
        converter >> id;
        for(unsigned int i = 0; i < 3; i++)
          converter >> triangle->vertices[i];
        for(unsigned int i = 0; i < 3; i++){
          converter >> triangle->edges[i];
          OrientedEdge* orientededge = new OrientedEdge(triangle->edges[i],id);
          orientededges.push_back(orientededge);
          triangle->OrEdges.push_back(orientededge);
          k++;
        }
        triangle->longestEdge=biggestEdge(mesh,triangle->OrEdges);
        triangle->area=ProjectLibrary::area(*mesh.Cell0D[triangle->vertices[0]],*mesh.Cell0D[triangle->vertices[1]],*mesh.Cell0D[triangle->vertices[2]]);
        if(triangle->area>mesh.AreaTol){
            mesh.TrianglesToBisect.push_back(id);
        }

        std::unordered_set<unsigned int> temp01, temp02, temp12;
        temp01.insert(triangle->vertices[0]);
        temp01.insert(triangle->vertices[1]);
        temp02.insert(triangle->vertices[0]);
        temp02.insert(triangle->vertices[2]);
        temp12.insert(triangle->vertices[1]);
        temp12.insert(triangle->vertices[2]);
        unsigned int e01, e02, e12;

        if ((mesh.Cell1D[triangle->edges[0]]->points==temp01)){
            e01=k-3;
        }else if (mesh.Cell1D[triangle->edges[1]]->points==temp01){
            e01=k-2;
        }else{
            e01=k-1;
        }

        if ((mesh.Cell1D[triangle->edges[0]]->points==temp02)){
            e02=k-3;
        }else if (mesh.Cell1D[triangle->edges[1]]->points==temp02){
            e02=k-2;
        }else{
            e02=k-1;
        }

        if ((mesh.Cell1D[triangle->edges[0]]->points==temp12)){
            e12=k-3;
        }else if (mesh.Cell1D[triangle->edges[1]]->points==temp12){
            e12=k-2;
        }else{
            e12=k-1;
        }

        if(ProjectLibrary::clockwise(mesh,triangle->vertices[0],triangle->vertices[1],triangle->vertices[2])){
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
        mesh.Cell2D.push_back(triangle);

        for(unsigned int i=k-3;i<k;i++){
            key=triangle->edges[i%3];
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


    unsigned int CurrentMax(Mesh &mesh) {
        unsigned int idMax=0;
        double max=-1;
        unsigned int i=0;
        for (auto it=mesh.Cell2D.begin();it!=mesh.Cell2D.end();it++){
            if(mesh.alreadyBisected[i]==0){
                if((*it)->area>max){
                    max=(*it)->area;
                    idMax=(i);
                }
            }
            i++;
        }
        return idMax;
    }

    bool bisect(Mesh& mesh,OrientedEdge* edge){
        auto a=mesh.Cell1D[edge->RealEdge]->points;
        unsigned int idP1= *a.begin();
        unsigned int idP2= *(++a.begin());
        Point *p1= mesh.Cell0D[idP1];
        Point *p2= mesh.Cell0D[idP2];
        Point *pmiddle= new Point(midpoint(*p1,*p2));
        unsigned int idPmiddle=mesh.NumberCell0D;
        mesh.NumberCell0D++;
        unsigned int idENext, idEPrec;
        idENext=edge->next->RealEdge;
        idEPrec=edge->next->next->RealEdge;
        Edge *e1m= new Edge(unordered_set<unsigned int> ({idP1, idPmiddle}));
        mesh.alreadyBisectedEdge.push_back(false);
        e1m->lenght=distance(*p1,*pmiddle);
        unsigned int ide1m=mesh.NumberCell1D;
        mesh.NumberCell1D++;
        Edge *e2m= new Edge(unordered_set<unsigned int> ({idP2, idPmiddle}));
        mesh.alreadyBisectedEdge.push_back(false);
        e2m->lenght=distance(*p2,*pmiddle);
        unsigned int ide2m=mesh.NumberCell1D;
        mesh.NumberCell1D++;

        unordered_set<unsigned int> PointsNext=mesh.Cell1D[edge->next->RealEdge]->points;
        unsigned int idPNext = findThirdVertex(PointsNext,idP1,idP2);
        Point *pNext =mesh.Cell0D[idPNext];
        Edge *eNm= new Edge(unordered_set<unsigned int> ({idPNext, idPmiddle}));
        mesh.alreadyBisectedEdge.push_back(false);
        eNm->lenght=distance(*pNext,*pmiddle);
        unsigned int ideNm=mesh.NumberCell1D;
        mesh.NumberCell1D++;

        Triangle *t1=new Triangle();
        Triangle *t2= new Triangle();
        mesh.alreadyBisected.push_back(false);
        mesh.alreadyBisected.push_back(false);
        unsigned int idT1=mesh.NumberCell2D;
        mesh.NumberCell2D++;
        unsigned int idT2=mesh.NumberCell2D;
        mesh.NumberCell2D++;
        OrientedEdge* oe1m = new OrientedEdge(ide1m,idT1);
        OrientedEdge* oe2m = new OrientedEdge(ide2m,idT2);
        OrientedEdge* oe1Nm = new OrientedEdge(ideNm,idT1);
        OrientedEdge* oe2Nm = new OrientedEdge(ideNm,idT2);

        t1->OrEdges.push_back(oe1m);
        t1->OrEdges.push_back(oe1Nm);
        t2->OrEdges.push_back(oe2m);
        t2->OrEdges.push_back(oe2Nm);

        oe1Nm->symmetric=oe2Nm;
        oe2Nm->symmetric=oe1Nm;
        t1->vertices={idP1,idPNext,idPmiddle};
        t2->vertices={idP2,idPNext,idPmiddle};
        t1->area=area(*p1,*pNext,*pmiddle);
        t2->area=area(*p2,*pNext,*pmiddle);
        if(t2->area>mesh.AreaTol){
            mesh.TrianglesToBisect.push_back(idT2);
        }
        if(t1->area>mesh.AreaTol){
            mesh.TrianglesToBisect.push_back(idT1);
        }
        bool orientation = ProjectLibrary::clockwise(mesh,idP1,idPNext,idP2);
        if(orientation){
            edge->next->RealTriangle=idT1;
            edge->next->next->RealTriangle=idT2;
            t1->OrEdges.push_back(edge->next);
            t2->OrEdges.push_back(edge->next->next);

            edge->next->RealTriangle=idT1;
            edge->next->next->RealTriangle=idT2;

            t1->edges={idENext,ideNm,ide1m};
            t2->edges={idEPrec,ideNm, ide2m};

            oe2m->next=oe2Nm;
            oe2Nm->next=edge->next->next;
            edge->next->next->next=oe2m;

            edge->next->next=oe1Nm;
            oe1Nm->next=oe1m;
            oe1m->next=edge->next;

        }else{
            edge->next->RealTriangle=idT2;
            edge->next->next->RealTriangle=idT1;
            t2->OrEdges.push_back(edge->next);
            t1->OrEdges.push_back(edge->next->next);

            edge->next->next->RealTriangle=idT1;
            edge->next->RealTriangle=idT2;

            t1->edges={idEPrec,ideNm,ide1m};
            t2->edges={idENext,ideNm, ide2m};

            oe1m->next=oe1Nm;
            oe1Nm->next= edge->next->next;
            edge->next->next->next=oe1m;

            oe2m->next=edge->next;
            edge->next->next=oe2Nm;
            oe2Nm->next=oe2m;
        }

        mesh.Cell0D.push_back(pmiddle);
        mesh.Cell1D.push_back(e1m);
        mesh.Cell1D.push_back(e2m);
        mesh.Cell1D.push_back(eNm);

        mesh.Cell2D.push_back(t1);
        mesh.Cell2D.push_back(t2);

        t1->longestEdge=biggestEdge(mesh,t1->OrEdges);
        t2->longestEdge=biggestEdge(mesh,t2->OrEdges);

        mesh.GraphedMesh.push_back(oe1m);
        mesh.GraphedMesh.push_back(oe2m);
        mesh.GraphedMesh.push_back(oe1Nm);
        mesh.GraphedMesh.push_back(oe2Nm);

        if (edge->symmetric==nullptr){
            return true;
        }else{
            auto PointsNext=mesh.Cell1D[edge->symmetric->next->RealEdge]->points;
            auto idPPrec = findThirdVertex(PointsNext,idP1,idP2);
            Point *pPrec = mesh.Cell0D[idPPrec];
            Edge *ePm = new Edge(unordered_set<unsigned int> ({idPPrec, idPmiddle}));
            mesh.alreadyBisectedEdge.push_back(false);
            ePm->lenght=distance(*pPrec,*pmiddle);
            unsigned int idePm=mesh.NumberCell1D;
            mesh.NumberCell1D++;
            Triangle *t3 = new Triangle();
            Triangle *t4 = new Triangle();
            mesh.alreadyBisected.push_back(false);
            mesh.alreadyBisected.push_back(false);
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

            t3->OrEdges.push_back(oeP1m);
            t4->OrEdges.push_back(oeP2m);
            t3->OrEdges.push_back(oeP1Pm);
            t4->OrEdges.push_back(oeP2Pm);

            oeP1Pm->symmetric=oeP2Pm;
            oeP2Pm->symmetric=oeP1Pm;
            t3->area=area(*p1,*pPrec,*pmiddle);
            t4->area=area(*p2,*pPrec,*pmiddle);
            if(t3->area>mesh.AreaTol){
                mesh.TrianglesToBisect.push_back(idT4);
            }
            if(t4->area>mesh.AreaTol){
                mesh.TrianglesToBisect.push_back(idT3);
            }
            t3->vertices={idP1,idPPrec,idPmiddle};
            t4->vertices={idP2,idPPrec,idPmiddle};
            if(orientation){
                edge->symmetric->next->RealTriangle=idT4;
                edge->symmetric->next->next->RealTriangle=idT3;

                t4->OrEdges.push_back(edge->symmetric->next);
                t3->OrEdges.push_back(edge->symmetric->next->next);

                edge->symmetric->next->RealTriangle=idT4;
                edge->symmetric->next->next->RealTriangle=idT3;

                t3->edges={idEPPrec,idePm,ide1m};
                t4->edges={idEPNext,idePm, ide2m};

                oeP1m->next= oeP1Pm;
                oeP1Pm->next=edge->symmetric->next->next;
                edge->symmetric->next->next->next=oeP1m;

                oeP2m->next=edge->symmetric->next;
                edge->symmetric->next->next=oeP2Pm;
                oeP2Pm->next=oeP2m;
            }else{
                edge->symmetric->next->RealTriangle=idT3;
                edge->symmetric->next->next->RealTriangle=idT4;

                t3->OrEdges.push_back(edge->symmetric->next);
                t4->OrEdges.push_back(edge->symmetric->next->next);

                edge->symmetric->next->RealTriangle=idT3;
                edge->symmetric->next->next->RealTriangle=idT4;

                t3->edges={idEPNext,idePm,ide1m};
                t4->edges={idEPPrec,idePm, ide2m};

                oeP2m->next=oeP2Pm;
                oeP2Pm->next=edge->symmetric->next->next;
                edge->symmetric->next->next->next=oeP2m;

                oeP1m->next=edge->symmetric->next;
                edge->symmetric->next->next=oeP1Pm;
                oeP1Pm->next=oeP1m;
            }
            mesh.Cell1D.push_back(ePm);

            oeP1m->symmetric=oe1m;
            oe1m->symmetric=oeP1m;
            oeP2m->symmetric=oe2m;
            oe2m->symmetric=oeP2m;

            mesh.Cell2D.push_back(t3);
            mesh.Cell2D.push_back(t4);


            t3->longestEdge=biggestEdge(mesh,t3->OrEdges);
            t4->longestEdge=biggestEdge(mesh,t4->OrEdges);

            mesh.GraphedMesh.push_back(oeP1m);
            mesh.GraphedMesh.push_back(oeP2m);
            mesh.GraphedMesh.push_back(oeP1Pm);
            mesh.GraphedMesh.push_back(oeP2Pm);
        }
        return true;
    }

    void Globalrefine(Mesh& mesh,int numberTriagles)
    {

        int j=0;
        for (auto it=mesh.StartingTriangles.begin(); it!= mesh.StartingTriangles.end();it++) {

            for (auto it2=(*(it)).second.begin();it2!=(*(it)).second.end();it2++){
                if(j==numberTriagles)return;
                unsigned int triangle= (*it2);
                j++;
                if(!mesh.alreadyBisected[triangle]){
                refine(mesh,mesh.Cell2D[triangle]->longestEdge);
                mesh.numberIterations++;
                }
            }
        }
    }



    void refine(Mesh& mesh, OrientedEdge* lEdge)
    {
        unsigned int triangle=lEdge->RealTriangle;
        if(lEdge->symmetric==nullptr)
        {
            //mesh.DestroyedTriangles.push_back(triangle);
            bisect(mesh,lEdge);
            mesh.alreadyBisected[triangle]=true;

            delete lEdge;
            return;
        }
        else
            {
                unsigned int idd = lEdge->symmetric->RealTriangle;
                OrientedEdge* nextLongest=mesh.Cell2D[idd]->longestEdge;
                if (nextLongest->symmetric==lEdge)
                {
                    //mesh.DestroyedTriangles.push_back(triangle);
                    //mesh.DestroyedTriangles.push_back(lEdge->symmetric->RealTriangle);
                    bisect(mesh,lEdge);

                    mesh.alreadyBisected[triangle]=true;


                    mesh.alreadyBisected[lEdge->symmetric->RealTriangle]=true;

                    delete lEdge->symmetric;
                    delete lEdge;
                    return;
                }
                else
                {
                    if(!mesh.alreadyBisected[nextLongest->RealTriangle])
                    refine(mesh,nextLongest);
                    if(!mesh.alreadyBisected[lEdge->RealTriangle])
                    refine(mesh,lEdge);
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
                 if(mesh.alreadyBisectedEdge[i]==0){
                 Point *p=mesh.Cell0D[i];
                 file<<i<<" "<<p->x<<" "<<p->y<<"\n";
                 }
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
        int i=0;
        for(auto it=mesh.Cell1D.begin();it!=mesh.Cell1D.end();it++)
            {
                if(!mesh.alreadyBisectedEdge[i]){
                file<<i;
                file<<" "<<(*(*it)->points.begin())<<" "<<(*++(*it)->points.begin());
                file<<"\n";
                }
            i++;
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
        int i=0;
                for(auto it=mesh.Cell2D.begin();it!=mesh.Cell2D.end();it++)
                {
                 if(!mesh.alreadyBisected[i]){
                 Triangle *t=(*it);
                 if(t->edges[0]!=0 && t->edges[1]!=0 && t->edges[1]!=0)
                   file<<i<<" "<<t->vertices[0]<<" "<<t->vertices[1]<<" "<<t->vertices[2]
                        <<" "<<t->edges[0]<<" "<<t->edges[1]<<" "<<t->edges[2]<<"\n";
                 }
                 i++;
                }
         return true;
    }

    void TrianglesToBisect(Mesh &mesh, unsigned int n)
    {
        mesh.StartingTriangles.clear();
        unsigned int i=0;
        for(auto it=mesh.Cell2D.begin();it!=mesh.Cell2D.end();it++)
        {
         if(!mesh.alreadyBisected[i]){
         Triangle *t=(*it);
         if (mesh.StartingTriangles.find(t->area) == mesh.StartingTriangles.end())
           mesh.StartingTriangles.insert({t->area, {i}});
         else
           mesh.StartingTriangles[t->area].push_back(i);
         }
         i++;
        }
    }

}
