#ifndef __TEST_EMPTY_H
#define __TEST_EMPTY_H

#include <gtest/gtest.h>
#include<chrono>

#include "Mesh.hpp"

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
    mesh.LocalRefine(100);
    //ProjectLibrary::UniformRefine(mesh);

    int i=0;
    for( auto it=mesh.Cell2D.begin();it !=mesh.Cell2D.end();it++)
    {
        if(!mesh.alreadyBisected[i])
        {
        for(auto it1=(*it)->OrEdges.begin();it1!=(*it)->OrEdges.end();it1++)
        ASSERT_EQ((*it1)->next->next->next,*it1);
        }
        i++;
    }
}

TEST(TestRefine, TestRefineSymmetry)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

   mesh.LocalRefine(100);

   int i=0;
   for( auto it=mesh.Cell2D.begin();it !=mesh.Cell2D.end();it++)
   {
       if(!mesh.alreadyBisected[i])
       {
       for(auto it1=(*it)->OrEdges.begin();it1!=(*it)->OrEdges.end();it1++)
       {
        if((*it1)->symmetric!=nullptr)
        {
           ASSERT_EQ((*it1)->symmetric->symmetric,(*it1));
        }
       }
       }
       i++;
   }

}

TEST(TestRefine, TestLocalRefine)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

   int i =mesh.NumberCell2DInitial;
   mesh.LocalRefine(mesh.NumberCell2DInitial);

   for (int j=0;j<i;j++)
   {
      // cout<<j<< " "<<mesh.alreadyBisected[j] <<endl;
      ASSERT_TRUE(mesh.alreadyBisected[j]); // Check that every initial element has been bisected
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


    mesh.bisect(mesh.GraphedMesh[0]);

    ProjectLibrary::Point p=ProjectLibrary::Point(0.5,0);
    ASSERT_TRUE((*mesh.Cell0D[4])==p);

    //ExportMesh(mesh,"../Raffinamento/Dataset/Test3/NewCell0Ds.csv","../Raffinamento/Dataset/Test3/NewCell1Ds.csv", "../Raffinamento/Dataset/Test3/NewCell2Ds.csv");
    //system("python ../Raffinamento/Dataset/Test3/RenderScript.py");

}

TEST(TestTimeComplexity,TestLocalRefineTime)
{
    ProjectLibrary::Mesh mesh;
    ASSERT_TRUE(ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"));

    for(int i =0;i<70;i++)
    {
    int initial=0;
    for (int j=0;j<mesh.NumberCell2D;j++)
    {
        if(!mesh.alreadyBisected[j])
            initial++;
    }
    ProjectLibrary::ResetStartingTriangles(mesh);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    mesh.LocalRefine(mesh.NumberCell2D*0.05);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    //cout<<" Initial Triangles: "<<initial <<" Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[mus]" << std::endl;


    cout<<initial <<" " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<";" <<std::endl;
    }
}

#endif // __TEST_EMPTY_H
