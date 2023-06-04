#include "empty_class.hpp"
#include<chrono>

int main()
{

  ProjectLibrary::Mesh mesh;
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

//!ImportMesh(mesh,"/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell0Ds.csv","/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell1Ds.csv", "/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/Cell2Ds.csv")
  if(!ImportMesh(mesh,"../Raffinamento/Dataset/Test2/Cell0Ds.csv","../Raffinamento/Dataset/Test2/Cell1Ds.csv", "../Raffinamento/Dataset/Test2/Cell2Ds.csv"))
  {
    return 1;
  }
  mesh.Globalrefine(mesh.NumberCell2DInitial);


  if(!ExportMesh(mesh,"../Raffinamento/Dataset/Test2/NewCell0Ds.csv","../Raffinamento/Dataset/Test2/NewCell1Ds.csv", "../Raffinamento/Dataset/Test2/NewCell2Ds.csv"))
  {
    return 1;
  }

  //system("py C:/Users/filip/Desktop/Progetto_PCS_2023/Projects/Raffinamento/Dataset/Test2/RenderScript.py");
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

  system("python ../Raffinamento/Dataset/Test2/RenderScript.py");
  return 0;
}
