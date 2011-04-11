#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>

#include "stringhandling.h"


int main(int argc, char** argv)
{
   std::string s1 = laTeXtoURL("http://bla.de/bla bla_bla MS.htm");
   std::string s2 = urlToLaTeX(s1);

   std::cout << s1 << std::endl;
   std::cout << s2 << std::endl;
   
   return 0;
}
