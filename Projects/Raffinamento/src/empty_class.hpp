#ifndef __EMPTY_H
#define __EMPTY_H

#include <iostream>

namespace ProjectLibrary
{
  class Empty
  {
    private:
    int test
    public:
      void Show() const { std::cout<< "Hello world;"<< std::endl; }
  };
}

#endif // __EMPTY_H
