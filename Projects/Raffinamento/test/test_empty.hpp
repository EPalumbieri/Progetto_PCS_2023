#ifndef __TEST_EMPTY_H
#define __TEST_EMPTY_H

#include <gtest/gtest.h>

#include "empty_class.hpp"

using namespace testing;

TEST(TestMesh, TestTriangleLoop)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

   for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
   {
       ASSERT_EQ((*it)->next->next->next,(*it));
   }
}

TEST(TestMesh, TestSymmetry)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

   for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
   {
       if((*it)->symmetric==nullptr) //if no symmetric check that edge is at the border (Valid only for initial mesh)
       {
           list<unsigned int> innerEdges=mesh.Cell1DMarkers[0];
           ASSERT_TRUE(std::find(innerEdges.begin(), innerEdges.end(), (*it)->RealEdge) == innerEdges.end());
       }
       else
       ASSERT_EQ((*it)->symmetric->symmetric,(*it));
   }
}

TEST(TestRefine,TestRefineTriangleLoop)
{
    ProjectLibrary::Mesh mesh;
    ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));
    mesh.Globalrefine(100);
    for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
    {
        ASSERT_EQ((*it)->next->next->next,(*it));
    }
}

TEST(TestRefine, TestRefineSymmetry)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

   mesh.Globalrefine(100);

   for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
   {
       if((*it)->symmetric!=nullptr)
       ASSERT_EQ((*it)->symmetric->symmetric,(*it));
   }
}

TEST(TestBisection,TestFindThirdVertex)
{

    ProjectLibrary::Mesh mesh;
    ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test3/Cell0Ds.csv","../Raffinamento/Dataset/Test3/Cell1Ds.csv", "../Raffinamento/Dataset/Test3/Cell2Ds.csv"));

    ASSERT_EQ(ProjectLibrary::findThirdVertex({0,2},0,1),2);
    ASSERT_EQ(ProjectLibrary::findThirdVertex({1,2},0,1),2);

}

TEST(TestBisection,TestBisectOuter)
{
    ProjectLibrary::Mesh mesh;
    ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test3/Cell0Ds.csv","../Raffinamento/Dataset/Test3/Cell1Ds.csv", "../Raffinamento/Dataset/Test3/Cell2Ds.csv"));

    //Pick an Edge
    // Get extremes + Oreiented Edge
    // Bisect + check that a midpoint exists
    //ASSERT_TRUE(mesh.bisect(*mesh.GraphedMesh.begin()));

    mesh.bisect(mesh.getOrientedEdge(0,0));

    ProjectLibrary::Point p=ProjectLibrary::Point(0.5,0);
    //auto it=mesh.Cell0D.begin();
    ASSERT_TRUE(mesh.Cell0D[4]==p);

    //ExportMesh(mesh,"../Raffinamento/Dataset/Test3/NewCell0Ds.csv","../Raffinamento/Dataset/Test3/NewCell1Ds.csv", "../Raffinamento/Dataset/Test3/NewCell2Ds.csv");
    //system("python ../Raffinamento/Dataset/Test3/RenderScript.py");

}

TEST(TestBisection,TestBisectInner)
{
    ProjectLibrary::Mesh mesh;
    ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test3/Cell0Ds.csv","../Raffinamento/Dataset/Test3/Cell1Ds.csv", "../Raffinamento/Dataset/Test3/Cell2Ds.csv"));

    mesh.bisect((mesh.getOrientedEdge(0,2)));

    ProjectLibrary::Point p=ProjectLibrary::Point(0.5,0.5);
    ASSERT_TRUE(mesh.Cell0D[4]==p);

    ExportMesh(mesh,"../Raffinamento/Dataset/Test3/NewCell0Ds.csv","../Raffinamento/Dataset/Test3/NewCell1Ds.csv", "../Raffinamento/Dataset/Test3/NewCell2Ds.csv");
    system("python ../Raffinamento/Dataset/Test3/RenderScript.py");
}

#endif // __TEST_EMPTY_H
