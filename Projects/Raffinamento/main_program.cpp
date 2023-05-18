#include "empty_class.hpp"

int main()
{

  ProjectLibrary::Mesh mesh;

  if(!ImportMesh(mesh,"Cell0D","Cell1D", "Cell2D"))
  {
    return 1;
  }

  return 0;
}
