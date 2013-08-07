#ifndef DEBUG_STUFF
#define DEBUG_STUFF

#include <iostream>

#define GOTHERE std::cerr << "Got here!\n";
#define CLEAR std::cerr << "Cleared!\n";
#define ENDLINE << std::endl;
#define WOOP std::cerr << "WOOP WOOP! Problem at this point!\n";

namespace Debug {
};

#endif