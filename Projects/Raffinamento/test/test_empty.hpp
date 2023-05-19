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



#endif // __TEST_EMPTY_H
