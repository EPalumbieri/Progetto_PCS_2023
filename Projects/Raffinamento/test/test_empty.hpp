#ifndef __TEST_EMPTY_H
#define __TEST_EMPTY_H

#include <gtest/gtest.h>

#include "empty_class.hpp"

using namespace testing;

TEST(TestMesh, TestTriangleLoop)
{
   ProjectLibrary::Mesh mesh;
   ASSERT_TRUE(ImportMesh(mesh,"./Dataset/Test1/Cell0Ds.csv","./Dataset/Test1/Cell1Ds.csv", "./Dataset/Test1/Cell2Ds.csv"));

   for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
   {
       ASSERT_EQ((*it)->next->next,*it);
   }
}

#endif // __TEST_EMPTY_H
