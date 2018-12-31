//This is a C++11 FFT library;	Description is comming soon.
#include <complex>
#include <type_traits>
#include "rom_globals.h"
#include "rom_prime.h"

#ifndef rom_fft_h
#define rom_fft_h
namespace rom {
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*template <class it> //iterator to floating point
auto real_to_complex(it inp_begin,it inp_end)-> {

}*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class it_i, class it_o> //additional safety for std::copy()
void copy_range_checked(it_i inp_begin,it_i inp_end,it_o out_begin,it_o out_end) {
if (std::distance(inp_begin,inp_end) != std::distance(out_begin,out_end)){	//trigger runtime error
	rom::error("copy_range_checked() has got two iterator-ranges with different size \n");
	}
std::copy(inp_begin,inp_end,out_begin);		//out_end should be just fine  :-)
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class RamIt>//ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class dft {	//template-functor-class
private:	//can perform discrete fourier transformation and it's inverse form
typedef typename std::iterator_traits<RamIt>::value_type valty;//this should be std::complex<float>

public:
dft(void) {}	//nothing to do  ;-)

//dft() performs the discrete fourier transformation as it's described in your math books
//it is simple but will be slow for larger ranges; complexity: O(n*n)
void operator()(RamIt first, RamIt last){	//input range from first to last; dft((ve.begin(),ve.end());
auto n =std::distance(first,last);	//compute the size of the range
std::vector<valty> result(n,rom::_complex_zero());//compare a container for temporary values
for (decltype(n) k = 0; k < n; k++) {		//Perform the discrete fourier transf.
        for (decltype(n) j = 0; j < n; j++) {
                double angle = 2.0 * rom::_PI * k * j / double(n);
                result.at(k) += *(first+j) * std::exp(angle* rom::_i());
                }
        }
rom::copy_range_checked(result.begin(),result.end(),first,last);//copy result back to the input range
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//reverse() performs the inversion of discrete fourier transformation as it's described in your math books
void reverse(RamIt first, RamIt last)	{	//input iterator-range
std::vector<valty> aut(first,last);	//compare a container for temporary values; deeeep copy
for (auto & r:aut) {r = std::conj(r);}	//conjugate all numbers
dft{}(aut.begin(),aut.end());		//perform dft() as usual
for (auto & r:aut) {r = std::conj(r)/double(aut.size());}//conjugate an scale again
rom::copy_range_checked(aut.begin(),aut.end(),first,last);//copy result back to the input range
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
};//class dft

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class RamIt>//ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class ffte {	//template-functor-class
private:
typedef typename std::iterator_traits<RamIt>::value_type valty;//this should be std::complex<float>

public:
ffte(void) {}	//nothing to do  ;-)

void operator ()(RamIt first, RamIt last) {
if (std::distance(first,last)<=1) {return;}	//if input contains 1 sample --> do nothing
auto size = size_t(std::distance(first,last));	//number  of samples
auto primes = rom::prime_splitter(size);	//make a prime-factorization of the sample size,
auto mods = primes.back();/*primes.pop_back();*///get largest primefactor at the end of std::vector
auto modsize = (size/mods);                     //if size is a primenumber -> modsize should be 1
//split input range in to 2d std::vector and recursively feed in to our own class; fist dimension is the number of mods
std::vector<std::vector<valty>>delegator(mods);//2d vector with size() == mods
for (auto& a:delegator) {a.resize(modsize);}    //resize subvectors	//second dimension should be modsize
for (size_t i=0;i<size;++i) {delegator.at(i%mods).at(i/mods)=*(first+i);}	//copy all inputdata into it
for (auto& a:delegator) {ffte{}(a.begin(),a.end());}	//let's perform all the smaller fft's
std::vector<valty> ret(size,valty(rom::_complex_zero()));	//prepare a return vector, initialize it
for (size_t k=0;k<size;++k) {   //output number
        size_t pos = (k % modsize);
        for (decltype(mods) mod=0;mod<mods;++mod) {     //mods
                double alpha = 2.0*rom::_PI*mod*k/double(size);
                auto element = delegator.at(mod).at(pos);
                element *= exp( rom::_i() * alpha); 	//TODO check if this works as expected
                ret.at(k) += element;
                }
        }
rom::copy_range_checked(ret.begin(),ret.end(),first,last);
}

void reverse(RamIt first, RamIt last) {         //inverse fourier transformation for 1 dimensional input
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it);}             //conjugate the complex numbers
ffte{}(first,last);				//perform fft
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it)/double(std::distance(first,last));}   //conjugate the c$
}

};//class ffte
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////







} //namespae rom
#endif //rom_fft_h

