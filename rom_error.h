//this is the most basic error handling it does NOT make use of c++ exceptions
//it will just kill the program after writing a messege to standard output
#ifndef rom_error_h
#define rom_error_h

#include <cstdlib>
#include <iostream>
#include <string>

namespace rom {

void error(std::string descr) {	//will kill your program after writing a message
std::cout << descr << std::endl << "This program kills itself. " << std::endl;
exit(EXIT_FAILURE);
}

void ok() {exit(EXIT_SUCCESS);}	//might be used to end a program without any bad errors

}	//namespace rom
#endif
