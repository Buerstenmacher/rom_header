//this header file should help you deal with large (and small) positive integer numbers. i recomend "uint64_t"
#include <set>
#include <vector>
#include <algorithm>
#include <numeric>
#include "rom_error.h"
#include "rom_time.h"

#ifndef ROM_PRIME_H
#define ROM_PRIME_H

namespace rom {

template <class uintxx_t = uint64_t>    //that's fine :-)
inline uint8_t isprime(uintxx_t b) {	//this function is optimized for performance not for readability
if (b<4) {return (b < 2)?(0):(1);}      //handle verry small numbers
uintxx_t sqr = uintxx_t(sqrt(b));
for (uintxx_t a{2};(a<13)&&(a<=sqr);++a)   	{if (!(b%a))  {return(0);}}	//test every number
for (uintxx_t a{13};(a<23)&&(a<=sqr);a+=2)	{if (!(b%a))  {return(0);}}	//test every second number
for (uintxx_t a{23};(a<131)&&(a<=sqr);a+=4)	{				//test every number%6 == 5
	if (!(b%a))  {return(0);}						//and	     number%6 == 1
	a+=2;
	if (!(b%a))  {return(0);}
	}
for (uintxx_t a{131};(a<=sqr);a+=4)	{//there is no need to check the loop condition everytime
	if (!(b%a))  {return(0);}	 //so it's fastest to do some repetitions inside the loop
	a+=2;
	if (!(b%a))  {return(0);}	//(!(b%a))  is exactly as fast as ((b%a)==0)
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	a+=4;
	if (!(b%a))  {return(0);}
	a+=2;
	if (!(b%a))  {return(0);}
	//a+=4; this wil be done by the end of the for loop
	}
return(1);
}

template <class uintxx_t = uint64_t>	//this function will return the smallest prime number that is larger than inp
uintxx_t next_prime(uintxx_t inp) {
do {inp++;} while (!isprime(inp));
return inp;
}

template <class uintxx_t = uint64_t>	//this function takes a unsigned integer as input and performs a Prime Factorization
std::vector<uintxx_t> prime_splitter(uintxx_t inp) {// if inp is 0 it will cause the whole program to stop!!,if it's 1 it will return an empty vector
std::vector<uintxx_t> ret{};	//return vector; empty at initialisation
if (inp == 0) {rom::error("It is impossible to make a factorisation of zero");}
for (uintxx_t i=2;inp!=1;++i)  {
        if (i>sqrt(inp)) {
                ret.push_back(inp);
                inp/=inp;  //input will be 1  -> for loop will end
                }
        while ((inp%i)==0){		//test if input is divisible by i
                ret.push_back(i);	//if it is -> push i to the result and
                inp /= i;		//split the size of  inp by i
                }
        }
return ret;
}

template <class uintxx_t = uint64_t>
void one_thread(uintxx_t start, uintxx_t ntimes,double& kw,uintxx_t& n) {
kw = 0.0;
n =0;
for (uintxx_t i=start; i!=(start+ntimes);++i) {
        if (isprime<uintxx_t>(i)){
                ++n;
                kw += (1.0/i);
                }
        }
}

template <class uintxx_t = uint64_t>
double prime_benchmark(uintxx_t nthread=1,uint8_t  output=1) { //can be used to benchmark your cpu
constexpr uintxx_t nthprime{54617891};  //at this primenumber the threshold of pi should be exceedet
constexpr uintxx_t nthpos{3260806};     //at this primenumber the threshold of pi should be exceedet
constexpr double rp2_time(846);         //This much seconds it takes on a Raspberry pi 2 at 900MHZ Cpu -Freq
uintxx_t n=0;
double kw =0;
double start = rom::mashinetime();
std::vector <uintxx_t> n_tmp    (nthread,0);
std::vector<double> kw_tmp      (nthread,0.0);
uintxx_t i_per_thread = nthprime/nthread/uint32_t(64);          //yes, this should be integer division
uintxx_t round = 0;
std::vector<std::thread> thr;  //vector of threads
do      {
        thr.clear();
        for (uintxx_t i=0;i!=nthread;++i) {
                thr.push_back(std::thread(one_thread<uintxx_t>,uintxx_t(i_per_thread*(i+round*nthread)),i_per_thread,std::ref(kw_tmp.at(i)),std::ref(n_tmp.at(i))));
                }
        for (auto& one:thr)       {one.join();}         //syncronize them all
        kw =    std::accumulate(kw_tmp.begin(),kw_tmp.end(),kw,std::plus<double>());
        n  =    std::accumulate(n_tmp.begin(), n_tmp.end(),n, std::plus<uintxx_t>());
        round++;
        if (output) {
                std::cout <<  n << " prime numbers until " << nthread*i_per_thread*round <<". The sum of all Prime Numbers to the power of -1 until here is: ";
                std::cout << kw <<std::endl;
                }
        } while (kw<rom::_PI<double>());    //until overrun
round--;                                                                //calculate back bevore overrun
kw =    std::accumulate(kw_tmp.begin(),kw_tmp.end(),kw,std::minus<double>());           //calculate back
n  =    std::accumulate(n_tmp.begin(), n_tmp.end(),n, std::minus<uintxx_t>());          //calculate back
if (output) {
        std::cout <<  n << " prime numbers until " << nthread*i_per_thread*round<<". The sum of all Prime Numbers to the power of -1 until here is: ";
        std::cout << kw <<std::endl;
        }
for (uintxx_t i=round*i_per_thread*nthread;i<std::numeric_limits<uintxx_t>::max();i++) {
        if (isprime(i)) {
                n++;
                kw += (1.0/i);
                if (kw>= rom::_PI<double>()) {
                        if (output) {
                                std::cout << i << " is the " << n << " nd prime number. "<<std::endl;
				std::cout <<"The sum of all Prime Numbers to the power of -1 until here is: ";
                                std::cout << kw << ", " << " this is: " << kw - rom::_PI<double>()<<" larger than "<< rom::_PI<double>()<< std::endl;
                                }
                        break;
                        }
                }
        }
double end = rom::mashinetime();
if (n-nthpos) {rom::error("Failed to calculate correct");}
if (output) {
        std::cout<<"It took "<< end-start << " seconds to prove that the sum of all primes to the power of -1 is greater then PI" << std::endl;
        std::cout<< "This was "<< rp2_time/(end-start) << " times faster than on a Raspberry Pi 2 running on 900MHZ (single thread)" <<std::endl;
        }
return (end-start);
}


void prime_t(void) {::rom::prime_benchmark<uint32_t>(32,1);}

}	//namespace rom
//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************




//now we are ready to crunsh full sized 64 bit integers on mshines like RaspberryPi-Zero;  yes yes yes :-)
template <class uintxx_t = uint64_t>	//yes i am cool because i use trailing return types ;-)
uint8_t isprime_low_ram_use(uintxx_t inp) {return (prime_splitter(inp).size()==1)?1:0;}

namespace {   //unnamed--> do not use this
//note:  all functions below work perfect, they have been tested more than billion times
	//but prime_splitter() is mutch faster with large numbers than next_prime() and isprime()
	//even if it's result contains more information and it will use much less memory
	//if prime_splitter() returns a vector with one element, its input is proven to be a prime number
	//the complexity of isprime() likel is counterproductive
//--> the improvement is on top of the header

//this function tells you if thethe input variable "b" is a prime number or not
//recomendet template parameters are: "uint16_t" for small numbers, "uint32_t" for fast calculation on 32 bit cpu's
//and "uint64_t" for maximum compatibility
//result is 1 if it is prime and 0 otherwise
template <class uintxx_t = uint64_t>    //that's fine :-)
uint8_t isprime_vector(uintxx_t b) {
if (b<4) {return (b < 2)?(0):(1);}      //handle verry small numbers, this should break recursion
static thread_local std::vector<uintxx_t> primes  {2,3,5,7,11,13};  //this vector should hold the first prime numbers without gap
uintxx_t test = primes.back();
uintxx_t sqr = uintxx_t(sqrt(b));
while (primes.back()<=sqr)      {if (isprime(++test))   {primes.push_back(test);}}
for (uintxx_t a=0;primes[a]<=sqr;++a)   {if (!(b%primes[a]))  {return(0);}}
return(1);
}

//this is overload of the previous function can be used if you already have calculated a large vector of consecutive prime number
//it exists to prevent recalculation and will probably save cpu time but will not reduce memory usage
template <class uintxx_t = uint64_t>    //that's better :-)
uint8_t isprime_vector(uintxx_t b,typename std::vector<uintxx_t>::iterator begin,typename std::vector<uintxx_t>::iteror end) {	//OverLord
if (b<4) {return (b < 2)?(0):(1);}      //handle verry small numbers, this should break recursion
static thread_local std::vector<uintxx_t> primes  {2,3,5,7,11,13};
if (size_t(std::distance<typename std::vector<uintxx_t>::iterator>(begin,end)) > primes.size()) {primes = std::vector<uintxx_t>(begin,end);}
uintxx_t test = primes.back();
uintxx_t sqr = uintxx_t(sqrt(b));
while (primes.back()<=sqr)      {if (isprime(++test))   {primes.push_back(test);}}
for (uintxx_t a=0;primes[a]<=sqr;++a)   {if (!(b%primes[a]))  {return(0);}}
return(1);
}

template <class uintxx_t = uint64_t>    //that's fine :-)
uint8_t isprime_set(uintxx_t b) {
if (b<4) {return (b < 2)?(0):(1);}      //handle verry small numbers, this should break recursion
static thread_local std::set<uintxx_t> primes  {2,3,5,7,11,13};  //this vector should hold the first prime numbers without gap
uintxx_t test = *(primes.end()--);		//get the last and largest element
uintxx_t sqr = uintxx_t(sqrt(b));
while (*(primes.end()--)<=sqr)      {if (isprime(++test))   {primes.insert(test);}}
for (auto& a:primes)   {
	if (!(b%a))  {return(0);}
	if (a > sqr) {return(1);}
	}
}


}       //namespace "unnamed"
#endif 	//ROM_PRIME_H


