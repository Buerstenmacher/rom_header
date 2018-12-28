//this header file should help you deal with large (and small) positive integer numbers. i recomend "uint64_t"
#include <set>
#include <vector>

#ifndef ROM_PRIME_H
#define ROM_PRIME_H

namespace rom {

template <class uintxx_t = uint64_t>    //that's fine :-)
uint8_t isprime(uintxx_t b) {
if (b<4) {return (b < 2)?(0):(1);}      //handle verry small numbers
uintxx_t sqr = uintxx_t(sqrt(b));
for (uintxx_t a{2};a<=sqr;++a)   {if (!(b%a))  {return(0);}}
return(1);
}

template <class uintxx_t = uint64_t>	//this function will return the smallest prime number that is larger than inp
uintxx_t next_prime(uintxx_t inp) {
do {++inp;} while (!isprime(inp));
return inp;
}

template <class uintxx_t = uint64_t>	//this function takes a unsigned integer as input and performs a Prime Factorization
std::vector<uintxx_t> prime_splitter(uintxx_t inp) {// if inp is 0 it will cause the whole program to stop!!,if it's 1 it will return an empty vector
std::vector<uintxx_t> ret{};	//return vector; empty at initialisation
if (inp == 0) {rom::error("It is ipossible to make a factorisation of zero");}
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

//now we are ready to crunsh full sized 64 bit integers on mshines like RaspberryPi-Zero;  yes yes yes :-)
template <class uintxx_t = uint64_t>	//yes i am cool because i use trailing return types ;-)
auto isprime_low_ram_use(uintxx_t inp) ->decltype(rom::_true)	{return (prime_splitter(inp).size()==1)?rom::_true:rom::_false;}


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
}	//namespace rom
#endif 	//ROM_PRIME_H


