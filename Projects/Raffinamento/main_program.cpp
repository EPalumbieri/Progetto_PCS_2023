#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;
//!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell2Ds.csv")
  if(!ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"))
  {
    return 1;
  }
  for ( auto it = mesh.Cell2D.begin(); it != mesh.Cell2D.end();it++ ){
      cout<<(it)->second.area<<endl;
  }
  getStartingTriangles(mesh,5);
  for (int i=0; i<5;i++){
      cout<<mesh.Cell2D[mesh.StartingTriangles[i]].area<<endl;
  }

  bisect(mesh,*(mesh.GraphedMesh.begin()+10));
  bisect(mesh,*(mesh.GraphedMesh.begin()+20));
  bisect(mesh,*(mesh.GraphedMesh.begin()+30));
  bisect(mesh,*(mesh.GraphedMesh.begin()+50));
  bisect(mesh,*(mesh.GraphedMesh.begin()+60));
  if(!ExportMesh(mesh,"../Raffinamento/Dataset/Test1/NewCell0Ds.csv","../Raffinamento/Dataset/Test1/NewCell1Ds.csv", "../Raffinamento/Dataset/Test1/NewCell2Ds.csv"))
  {
    return 1;
  }
  //system("cd ../Raffinamento/Dataset/Test1" );
  //system("dir");
  system("py C:/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/RenderScript.py");
  return 0;
}
