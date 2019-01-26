//This is a C++11 FFT library;	Description is comming soon.
#include <algorithm>
#include <complex>
#include <type_traits>
#include <thread>
#include "rom_error.h"
#include "rom_globals.h"
#include "rom_prime.h"
#include "rom_rand.h"
#include "rom_time.h"

#ifndef rom_fft_h
#define rom_fft_h
namespace rom {
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class it> //iterator to floating point
auto real_to_complex(it inp_begin,it inp_end)->std::vector<std::complex<typename std::iterator_traits<it>::value_type>> {
typedef typename std::iterator_traits<it>::value_type valty;//float, double or long double
std::vector<std::complex<valty>> ret{};
for (auto iter=inp_begin;iter!=inp_end;++iter) {ret.push_back(std::complex<valty>(*iter));}
return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class it_i, class it_o> //additional safety for std::copy()
void copy_range_checked(it_i inp_begin,it_i inp_end,it_o out_begin,it_o out_end) {
if (std::distance(inp_begin,inp_end) != std::distance(out_begin,out_end)){	//trigger runtime error
	rom::error("copy_range_checked() has got two iterator-ranges with different size \n");
	}
std::copy(inp_begin,inp_end,out_begin);		//out_end should be just fine  :-)
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
} //namespae rom

namespace  {//anonymous!!!
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class iter>	//at least bidirectional iterator
class iter_range;	//pre declaration
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class iter>	//at least bidirectional iterator
class iter_range {	//this class should add functionality of std::vector to a range of iterators
private:
iter first;		//points to the first element in the lange
iter last;		//ponts to the last element inrange, not one past the last!!!!!
size_t stp;		//how often you mus increment an iterator to get the next element

public:
typedef typename std::iterator_traits<iter>::value_type value_type;

//get the number of elements in the range
size_t size(void) const {return (std::distance(first,last)+stp)/stp;}

//get the spacing between elements in range
size_t step(void) const {return stp;}

//get iterator to first element
iter begin(void) const {return first;}

//get element at position  //use it like std::vector<>.at()
value_type& at(size_t pos) {return *std::next(first,pos*stp);}

value_type& operator [](size_t pos) {return *(first+(pos*stp));}

//constructor overload default 2 parameters begin() and end()
iter_range(iter beg_in, iter end_in,size_t step_in = 1):first(),last(),stp() {
if (size_t(std::distance(beg_in,end_in))%step_in) {
	rom::error("You are trying to access an IteratorRange of size "+
		std::to_string(std::distance(beg_in,end_in))+" with a step of "+
		std::to_string(step_in)+" its size would be "+
		std::to_string(float(std::distance(beg_in,end_in))/float(step_in))+" .");
	}
stp=	step_in;
first=	beg_in;
last=	std::prev(end_in,stp);  	//points to the last element
}

//constructor overload if you try to split an range into smaler iter_range
iter_range(iter beg_in, iter end_in,size_t step_in,size_t ofset_in):first(),last(),stp() {
size_t size_inp = std::distance(beg_in,end_in);
if (size_inp % step_in) {
	rom::error("You are trying to access an IteratorRange of size "+
		std::to_string(std::distance(beg_in,end_in))+" with a step of "+
		std::to_string(step_in)+" its size would be "+
		std::to_string(float(std::distance(beg_in,end_in))/float(step_in))+" .");
	}
if (ofset_in >= step_in) {rom::error("offset for iterator_range has to be smaller than step");}
first=	std::next(beg_in,ofset_in);
stp=	step_in;
size_t size = size_inp / step_in; 	//integeger division should work fine here
last = 	std::next(first,(size-1)*stp);
}

//constructor overload if you try to create an iterator_range based on another iterator_range
iter_range(const iter_range& range_in,size_t step_in,size_t ofset_in):first(),last(),stp() {
if (range_in.size() % step_in) {
	rom::error("You are trying to access an IteratorRange of size "+
		std::to_string(range_in.size())+" with a step of "+
		std::to_string(step_in)+" its size would be "+
		std::to_string(range_in.size()/float(step_in))+" .");
	}
if (ofset_in >= step_in) {rom::error("offset for iterator_range has to be smaller than step");}
first =	std::next(range_in.begin(),ofset_in*range_in.step());
stp =	step_in*range_in.step();
size_t size = range_in.size() / step_in; 	//integeger division should work fine here
last = 	std::next(first,(size-1)*stp);
}

iter_range<iter>& operator=(const std::vector<value_type>& other) {// copy assignment
if (other.size() != this->size()) {rom::error("assignment of wrong sized std::vector to iter_range");}
for (size_t k=0; k != other.size(); ++k) {this->operator[](k) = other[k];}
return *this;
}

// assume the object holds reusable storage, such as a heap-allocated buffer mArray
/*T& operator=(const T& other) // copy assignment
{
    if (this != &other) { // self-assignment check expected
        if (other.size != size) {         // storage cannot be reused
            delete[] mArray;              // destroy storage in this
            size = 0;
            mArray = nullptr;             // preserve invariants in case next line throws
            mArray = new int[other.size]; // create storage in this
            size = other.size;
        }
        std::copy(other.mArray, other.mArray + other.size, mArray);
    }
    return *this;
}

*/
};

//this function should help to split a range of iterators for fft functions
template<class iter>
std::vector<iter_range<iter>> iter_splitter(iter_range<iter> range_in, size_t n) {
if (range_in.size() % n) {rom::error("cannot perform splitting of iterator range");}
std::vector<iter_range<iter>> ret;
for (size_t i=0; i!=n;++i)  {ret.push_back(iter_range<iter>(range_in,n,i));}
return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class RamIt>//ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class dft {	//template-functor-class
public:		//can perform discrete fourier transformation and it's inverse form
typedef typename std::iterator_traits<RamIt>::value_type value_type;//this should be std::complex<float>
typedef typename value_type::value_type flt;		//float, double or long double

dft(void) {}	//nothing to do  ;-)

//dft() performs the discrete fourier transformation as it's described in your math books
//it is simple but will be slow for larger ranges; complexity: O(n*n)
void operator()(RamIt first, RamIt last){	//input range from first to last; dft((ve.begin(),ve.end());
auto n =std::distance(first,last);	//compute the size of the range
std::vector<value_type> result(n,rom::_complex_zero<flt>());//prepare a container for temporary values
for (decltype(n) k = 0; k < n; k++) {		//Perform the discrete fourier transf.
        for (decltype(n) j = 0; j < n; j++) {
                flt angle = 2.0 * rom::_PI<flt>() * k * j / flt(n);
                result.at(k) += *(first+j) * std::exp(angle * rom::_i<flt>());
                }
        }
rom::copy_range_checked(result.begin(),result.end(),first,last);//copy result back to the input range
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//overload for rom::iter_range
void operator()(iter_range<RamIt> itvec) {
auto n=itvec.size();
std::vector<value_type> result(0,rom::_complex_zero<flt>());	//prepare a container for temporary values
for (size_t k = 0; k < n; ++k) {				//Perform the discrete fourier transf.
	result.push_back(rom::_complex_zero<flt>());
        for (size_t j = 0; j < n; ++j) {
		flt angle = 2.0 * rom::_PI<flt>() * k * j / flt(n);
                result.at(k) += itvec.at(j) * std::exp(angle * rom::_i<flt>());
		}
	}
//for (size_t k = 0; k < n; ++k) {itvec.at(k) = result.at(k);}
itvec = result;
}

//reverse() performs the inversion of discrete fourier transformation as it's described in your math books
void reverse(RamIt first, RamIt last)	{	//input iterator-range
std::vector<value_type> aut(first,last);	//compare a container for temporary values; deeeep copy
for (auto & r:aut) {r = std::conj(r);}	//conjugate all numbers
dft{}(aut.begin(),aut.end());		//perform dft() as usual
for (auto & r:aut) {r = std::conj(r)/flt(aut.size());}//conjugate an scale again
rom::copy_range_checked(aut.begin(),aut.end(),first,last);//copy result back to the input range
}

};//class dft
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fast but takes a lot of RaM
template<class RamIt>	//ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class ffte {		//template-functor-class
public:
typedef typename std::iterator_traits<RamIt>::value_type value_type;//this should be std::complex<float>
typedef typename value_type::value_type flt;		//float, double or long double

ffte(void) {}	//nothing to do  ;-)

void operator ()(RamIt first, RamIt last) {
if (std::distance(first,last)<=1) {return;}	//if input contains 1 sample --> do nothing
auto size = size_t(std::distance(first,last));	//number  of samples
auto primes = rom::prime_splitter(size);	//make a prime-factorization of the sample size,
auto mods = primes.front();/*primes.pop_back();*///get largest primefactor at the end of std::vector
auto modsize = (size/mods);                     //if size is a primenumber -> modsize should be 1
//split input range in to 2d std::vector and recursively feed in to our own class; fist dimension is the number of mods
std::vector<std::vector<value_type>>delegator(mods);//2d vector with size() == mods
for (auto& del:delegator) {del.resize(modsize);}    //resize subvectors	//second dimension should be modsize
auto a = first;
for (size_t i=0;i<size;++i) {delegator.at(i%mods).at(i/mods)=*(a++);}	//copy all inputdata into it
for (auto& del:delegator) {ffte{}(del.begin(),del.end());}	//let's perform all the smaller fft's
a=first;
for (size_t k=0; k<size;++k,++a) {	//output number
	(*a) = rom::_complex_zero<flt>();
        size_t pos = (k % modsize);
        for (decltype(mods) mod=0;mod<mods;++mod) {	//mods
                flt alpha = 2.0*rom::_PI<flt>()*mod*k/flt(size);
                auto element = delegator.at(mod).at(pos);
                element *= exp( rom::_i<flt>() * alpha); //ToDo check if this works as expected
                (*a) += element;
		}
        }
}

void reverse(RamIt first, RamIt last) {         //inverse fourier transformation for 1 dimensional input
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it);}             //conjugate the complex numbers
ffte{}(first,last);				//perform fft
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it)/flt(std::distance(first,last));}   //conjugate the c$
}

};//class ffte
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Mutithread is even faster but takes more RaM
template<class RamIt>	//random-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class mt_ffte {    	//template-functor-class
public:
typedef typename std::iterator_traits<RamIt>::value_type value_type;//this should be std::complex<float>
typedef typename value_type::value_type flt;            //float, double or long double

mt_ffte(void) {}   //nothing to do  ;-)

void operator ()(RamIt first, RamIt last,uint32_t maxthr=32) {
if (std::distance(first,last)<=1) {return;}     //if input contains 1 sample --> do nothing
auto size = size_t(std::distance(first,last));  //number  of samples
auto primes = rom::prime_splitter(size);        //make a prime-factorization of the sample size,
auto mods = primes.front();			//get smalest primefactor at the begin of std::vector
auto modsize = (size/mods);                     //if size is a primenumber -> modsize should be 1
//split input range in to 2d std::vector and recursively feed in to our own class; fist dimension is the number of mods
std::vector<std::vector<value_type>>delegator(mods);//2d vector with size() == mods
for (auto& a:delegator) {a.resize(modsize);}    //resize subvectors     //second dimension should be modsize
auto frs = first;
for (size_t i=0;i<size;++i) {delegator.at(i%mods).at(i/mods)=*(frs++);} //copy all inputdata into it
if (maxthr>=mods) {	//multithread
        std::vector<std::thread> thr;	//vector of threads
	thr.clear();
	for (auto& a:delegator) {
		thr.push_back(std::thread(mt_ffte<RamIt>{},a.begin(),a.end(),(maxthr/mods)));
		}
	for (auto& one:thr)       {one.join();}
	}
	else {		//single thread
	for (auto& a:delegator) {mt_ffte{}(a.begin(),a.end(),(maxthr/mods));}    //let's perform all the smaller fft's
	}
frs=first;
for (size_t k=0; k<size;++k,++frs) {   //output number
        (*frs) = rom::_complex_zero<flt>();
        size_t pos = (k % modsize);
        for (decltype(mods) mod=0;mod<mods;++mod) {     //mods
                flt alpha = 2.0*rom::_PI<flt>()*mod*k/flt(size);
                auto element = delegator.at(mod).at(pos);
                element *= exp( rom::_i<flt>() * alpha);        //TODO check if this works as expected
                (*frs) += element;
                }
        }
}

void reverse(RamIt first, RamIt last) {         //inverse fourier transformation for 1 dimensional input
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it);}             //conjugate the complex numbers
mt_ffte{}(first,last);                             //perform fft
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it)/flt(std::distance(first,last));}	//conjugate the again
}

};//class mt_ffte
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Mutithread with reduced RaM demand; ideal for giant fft's with size above 2^20
template<class RamIt>   //ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class exp_ffte {	//template-functor-class
private:

static void engine(iter_range<RamIt> itvec,size_t maxthr=32) {
uint32_t sz = itvec.size();
uint32_t mods,modsize;
if (sz <= 1) {return;}	//break recursion
	{		//block scope for local variables
	auto primes = rom::prime_splitter(sz);
	if (primes.size()==1) {	//shortcut //no acceleration if size of input is prime number; sorry
		dft<RamIt>{}(itvec);
		return;
		}
	mods = primes.front();
	modsize = (sz/mods);
	}	//destroy primes
std::vector<iter_range<RamIt>> delegator = iter_splitter(itvec,mods);
if (maxthr>=mods) {	//multithread
        std::vector<std::thread> thr{};	//vector of threads
	for (auto& del:delegator) {
		exp_ffte<RamIt> obj;
		thr.push_back(std::thread(exp_ffte<RamIt>{}.engine,del,(maxthr/mods)));
//		thr.push_back(std::thread(exp_ffte<RamIt>{},del,(maxthr/mods)));	//does not work
		}
	for (auto& one:thr)       {one.join();}
	}
else	{for (auto& del:delegator) {engine(del,maxthr/mods);}}	//single thread
std::vector<value_type> retvec(0,rom::_complex_zero<flt>());
constexpr flt two_pi{rom::_PI<flt>()*2.0};		//precompute this values or use compiler optimisation "-O2"
constexpr std::complex<flt> imaginary {rom::_i<flt>()};	//if you compute everything inside the for loop
flt alpha{0.0};
value_type element{rom::_complex_zero<flt>()};
for (size_t k=0; k<sz;++k) {   //output number
	auto ret = rom::_complex_zero<flt>();
        size_t pos = (k % modsize);
	flt precalc = two_pi * k /static_cast<flt>(sz);
        for (size_t mod=0;mod<mods;++mod) {     //mods
		alpha = precalc * mod;
		element = delegator.at(mod).at(pos);
                element *= exp( imaginary * alpha); 	//TODO check if this works as expected
                ret += element;
                }
	retvec.push_back(ret);
        }
//for (size_t k=0; k!=sz; ++k)	{itvec.at(k) = retvec.at(k);}
itvec = retvec;
}

public:
typedef typename std::iterator_traits<RamIt>::value_type value_type;//this should be std::complex<float>
typedef typename value_type::value_type flt;            //float, double or long double

exp_ffte(void) {}   //nothing to do  ;-)

void operator() (iter_range<RamIt> itv)	  {this->engine(itv);}	//delegate to engine()
void operator() (RamIt first, RamIt last) {this->engine(iter_range<RamIt>(first,last));}//delegate to engine()

void reverse(RamIt first, RamIt last) {         //inverse fourier transformation for 1 dimensional input
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it);}             //conjugate the complex numbers
exp_ffte{}(first,last);                             //perform fft
for (auto it=first;it!=last;++it) {(*it) = std::conj(*it)/flt(std::distance(first,last));}   //conjugate the c$
}

}; //class exp_ffte
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} //namespace unnamed

namespace rom {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class RamIt>   //ranom-access-iter to std::complex<double>, std::complex<float> or std::complex<long double>
class auto_fft {	//template-functor-class
public:
typedef typename std::iterator_traits<RamIt>::value_type value_type;//this should be std::complex<float>
typedef typename value_type::value_type flt;            //float, double or long double

auto_fft(void) {}

void operator() (RamIt first, RamIt last) {//delegate to fft classes
size_t size = std::distance(first,last);
if (size<=rom::_kilo<flt>()) 	{dft<RamIt>{}(first,last);}//use discrete fourier transf. for small sizes
else if (size<=rom::_mega<flt>())		{mt_ffte<RamIt>{}(first,last);}//use fastest solution for larger sizes
else 	{exp_ffte<RamIt>{}(first,last);}	//use the solution with the least memory demand for giant sizes
}

void reverse(RamIt first, RamIt last) {//delegate to fft classes
size_t size = std::distance(first,last);
if (size<=rom::_kilo<flt>()) 	{dft<RamIt>{}.reverse(first,last);}
else if (size<rom::_mega<flt>())		{mt_ffte<RamIt>{}.reverse(first,last);}
else 	{exp_ffte<RamIt>{}.reverse(first,last);}
}

};	//class auto_fft

//-----Testfunction-----performance-testing
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class fu=ffte<std::vector<std::complex<double>>::iterator>, uint32_t size=100000, uint32_t times=5>
class fourier_test{
private:
std::vector<typename fu::value_type> inp,copy;
fu fur;
double start, stop;

void generate_inp(void) {
inp.resize(size);
std::generate(inp.begin(),inp.end(),rom::rand_0_1<typename fu::flt>);
copy = inp;
}

void calculate (void) {
fur(inp.begin(),inp.end());
fur.reverse(inp.begin(),inp.end());
}

uint8_t compare(void) {
typename fu::flt largest {0.0};
for (decltype(size) i=0;i<size; i++)    {largest = std::max(std::max(std::abs(copy.at(i)),std::abs(inp.at(i))),largest);}
for (decltype(size) i=0;i<size; i++)    {
        auto unc = std::abs(largest*rom::_max_acceptable_error<typename fu::flt>());
        auto dif = std::abs(copy.at(i)-inp.at(i));
        if (dif > unc) {return 0;}
        }
return 1;
}

public:
fourier_test(void):inp(),copy(),fur(),start(),stop() {}

void operator()(void) {
start = rom::mashinetime();
for (decltype(times) i=0; i<times;i++) {
        generate_inp();
        calculate();
        if (!compare()) {rom::error("There was an error at calculating a Fourier transformation!");}
        else {std::cout << i+1 << " Fourier transformations were correct! \n";}
        }
stop = rom::mashinetime();
std::cout<<times<< " fourier transformations of size " << size <<" took " <<(stop-start)<<" seconds.\n";
}

};  //class fourier_test
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} //namespae rom

#endif //rom_fft_h

