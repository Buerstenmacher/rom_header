//This is a C++11 FFT library;	Description is comming soon.
#include <complex>
#include <type_traits>
#include "rom_globals.h"

#ifndef rom_fft_h
#define rom_fft_h
namespace rom {

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class it_i, class it_o> //additional safety for std::copy()
void copy_range_checked(it_i inp_begin,it_i inp_end,it_o out_begin,it_o out_end) {
if (std::distance(inp_begin,inp_end) != std::distance(out_begin,out_end))	{
	rom::error("copy_range_checked() has got two Iterator-ranges with different size");
	}
std::copy(inp_begin,inp_end,out_begin);		//out_end should be fine
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//dft() performs the discrete fourier transformation as it's described in your math books
//it is simple but will be slow for larger ranges O(n*n)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class RamIt>	//RamIt should be an Iterator to type std::complex<floating point type>
void dft(RamIt first, RamIt last){	//input range from first to last; dft((ve.begin(),ve.end());
typedef typename RamIt::value_type valtype;	//this should be std::complex<float>
auto n =std::distance(first,last);	//compute the size of the range
std::vector<valtype> result(n,valtype(0.0,0.0));//compare a container for temporary values
for (decltype(n) k = 0; k < n; k++) {		//Perform the discrete fourier transf.
        for (decltype(n) j = 0; j < n; j++) {
                double angle = 2 * rom::_PI * k * j / double(n);
                result.at(k) += *(first+j) * std::exp(angle* rom::_i<>());
                }
        }
rom::copy_range_checked(result.begin(),result.end(),first,last);//copy result back to the input range
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////







} //namespae rom
#endif //rom_fft_h

