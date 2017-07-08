#include <stdio.h>

int f(int a);
int g(int a)
{
  return 0;
}

int main()
{
  f(1);
  return 0;
}

extern "C"
{
  void __visit_item(int loc)
  {
    printf("visit_item: %d\n", loc);
  }
}

