#include "empty_class.hpp"
#include<chrono>

int main()
{

  ProjectLibrary::Mesh mesh;
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

//!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/Cell2Ds.csv")
  if(!ImportMesh(mesh,"../Raffinamento/Dataset/Test1/Cell0Ds.csv","../Raffinamento/Dataset/Test1/Cell1Ds.csv", "../Raffinamento/Dataset/Test1/Cell2Ds.csv"))
  {
    return 1;
  }
 int numTriangles=144;
 unsigned int TotIterations=50000;
 while (mesh.numberIterations<TotIterations){
 ProjectLibrary::Globalrefine(mesh,numTriangles);
 if (mesh.numberIterations<TotIterations){
 TrianglesToBisect(mesh,numTriangles);
 numTriangles*=1.25;
 }
 }


  if(!ExportMesh(mesh,"../Raffinamento/Dataset/Test1/NewCell0Ds.csv","../Raffinamento/Dataset/Test1/NewCell1Ds.csv", "../Raffinamento/Dataset/Test1/NewCell2Ds.csv"))
  {
    return 1;
  }

  //system("py C:/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test1/RenderScript.py");
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

  system("py ../Raffinamento/Dataset/Test1/RenderScript.py");
  return 0;
}
