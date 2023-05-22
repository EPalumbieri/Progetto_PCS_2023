#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;
//!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell2Ds.csv")
  if(!ImportMesh(mesh,"../Raffinamento/Dataset/Test2/Cell0Ds.csv","../Raffinamento/Dataset/Test2/Cell1Ds.csv", "../Raffinamento/Dataset/Test2/Cell2Ds.csv"))
  {
    return 1;
  }
  getStartingTriangles(mesh,100);

  ProjectLibrary::Globalrefine(mesh);

  if(!ExportMesh(mesh,"../Raffinamento/Dataset/Test2/NewCell0Ds.csv","../Raffinamento/Dataset/Test2/NewCell1Ds.csv", "../Raffinamento/Dataset/Test2/NewCell2Ds.csv"))
  {
    return 1;
  }
  for(auto it = mesh.GraphedMesh.begin(); it!=mesh.GraphedMesh.end();it++){

  }
  //system("py C:/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/RenderScript.py");
  system("python ../Raffinamento/Dataset/Test2/RenderScript.py");
  return 0;
}
