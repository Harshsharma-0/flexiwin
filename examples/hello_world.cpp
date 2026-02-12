#include "flexiwin/flexiwin.hpp"
#include <iostream>

int main() {
  flexi::windowManager manager;
  manager.initlize((char *)"hello", 250, 250);
  return 0;
}
