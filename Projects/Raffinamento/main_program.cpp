#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;

  if(!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell2Ds.csv"))
  {

    return 1;
  }
  for( auto it=mesh.GraphedMesh.begin();it !=mesh.GraphedMesh.end();it++)
  {
      //if((*it)->symmetric==(*it))
      cout<<mesh.Cell2D[(*it)->RealTriangle].area<<endl;
  }
  return 0;
}
