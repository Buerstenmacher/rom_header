#ifndef rom_stream_h
#define rom_stream_h

#include <vector>
#include <list>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>    	//let's teach the compiler how to print out an std::vector
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
os << "std::vector{";
for (auto &ii : v){os << " (" << ii<<")";}
os << " }";
return os;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>    //print std::list to stream std::list
std::ostream& operator << (std::ostream& os, const std::list<T>& v) {
os << "std::list{";
for (auto &ii : v){os << " (" << ii<<")";}
os << " }";
return os;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//rom_stream.h


/*example:
	#include "rom_stream.h"
	int main (void) {
	std::vector<float> testval{0.0,1.7,7.4,0.004};
	std::cout << testval <<std::endl;
	std::list<float> tl{0.0,1.7,7.4,0.004};
	std::cout << tl <<std::endl;
	std::list<float> empty_list{};
	std::cout << empty_list <<std::endl;
	}

output should look like:
	std::vector{ (0) (1.7) (7.4) (0.004) }
	std::list{ (0) (1.7) (7.4) (0.004) }
	std::list{ }
*/

