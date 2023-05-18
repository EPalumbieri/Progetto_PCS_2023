#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;

  if(!ImportMesh(mesh,"./Dataset/Test1/Cell0Ds.csv","./Dataset/Test1/Cell1Ds.csv", "./Dataset/Test1/Cell2Ds.csv"))
  {

    return 1;
  }

  for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
  {
      //if((*it)->symmetric==(*it))
          cout<<(*it)->RealTriangle<<" ";
  }
  return 0;
}
