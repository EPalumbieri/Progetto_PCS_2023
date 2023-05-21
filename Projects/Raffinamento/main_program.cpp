#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;
//!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell2Ds.csv")
  if(!ImportMesh(mesh,"../Raffinamento/Dataset/Test2/Cell0Ds.csv","../Raffinamento/Dataset/Test2/Cell1Ds.csv", "../Raffinamento/Dataset/Test2/Cell2Ds.csv"))
  {
    return 1;
  }
  getStartingTriangles(mesh,186);

  ProjectLibrary::Globalrefine(mesh);
//  for (int i=0;i<mesh.NumberCell2D;i++)
//    cout<<getBiggestEdge(mesh,i)->RealEdge<<endl;

  //refine(mesh,mesh.StartingTriangles[0]);
  //bisect(mesh,ProjectLibrary::getBiggestEdge(mesh,mesh.StartingTriangles[0]));
  //bisect(mesh,ProjectLibrary::getBiggestEdge(mesh,mesh.StartingTriangles[0]));

//  bisect(mesh,*(mesh.GraphedMesh.begin()+20));
//  bisect(mesh,*(mesh.GraphedMesh.begin()+30));
//  bisect(mesh,*(mesh.GraphedMesh.begin()+50));
//  bisect(mesh,*(mesh.GraphedMesh.begin()+60));
  if(!ExportMesh(mesh,"../Raffinamento/Dataset/Test2/NewCell0Ds.csv","../Raffinamento/Dataset/Test2/NewCell1Ds.csv", "../Raffinamento/Dataset/Test2/NewCell2Ds.csv"))
  {
    return 1;
  }
  for(auto it = mesh.GraphedMesh.begin(); it!=mesh.GraphedMesh.end();it++){

  }
  //system("cd ../Raffinamento/Dataset/Test2" );
  //system("dir");
  //system("py C:/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/RenderScript.py");
  system("py ../Raffinamento/Dataset/Test2/RenderScript.py");
  return 0;
}
