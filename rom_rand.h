#ifndef rom_rand_h
#define rom_rand_h

#include <iomanip>
#include <random>
#include <limits>
#include <functional>
#include <sstream>

namespace rom {

typedef typename std::mt19937_64 prf_rand_eng;      //prefered c++11 random engine 64bit  :-) mersenne twister 64bit

template <class uint>	//return random value in full range of uintxx_t
inline uint rand_int(void) {
static thread_local prf_rand_eng generator{};
static thread_local std::uniform_int_distribution<uint64_t> u_distr(std::numeric_limits<uint>::min(),std::numeric_limits<uint>::max());
static thread_local auto dice = std::bind(u_distr,generator);
return dice();
}

inline double rand_0_1(){	//returns:   0.0 <= value < 1.0
static thread_local prf_rand_eng generator{};	// 0.000000 und (1.000000-epsilon) are extreme possible return values
static thread_local std::uniform_real_distribution<double> u_distr(rom::_zero,rom::_one); //interval[0.0,1.0)
static thread_local auto dice = std::bind(u_distr,generator);
return dice();
}

inline double rand_sym(double max){	//returns:   -1.0*max <= value < 1.0*max	//almost symetric
static thread_local prf_rand_eng generator{};
static thread_local std::uniform_real_distribution<double> u_distr(rom::_neg_one*max,rom::_one*max); //interval[-max,+max)
static thread_local auto dice = std::bind(u_distr,generator);
return dice();
}

inline int8_t rand_sign() {//should return 1 or -1 with equal probability
static thread_local prf_rand_eng generator{};
static thread_local std::uniform_int_distribution<uint64_t> u_distr(0,1);
static thread_local auto dice = std::bind(u_distr,generator);
return (dice()==1)?(1):(-1);
}

}//namespace rom
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Testfunctions:
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string monte_carlo_pi(uint64_t times) {//calculate pi with your random number generator
uint64_t tries{0},hits{0};
double x,y;
for (tries =0; tries < times; ++tries) {
        x=rom::rand_0_1();
        y=rom::rand_0_1();
        if (sqrt(x*x + y*y) <= 1.0)     {++hits;}
        }
std::ostringstream os;
os << std::fixed << std::setprecision(12) <<(double(hits)*4.0/double(tries));
return os.str();
}

std::string sign_test(uint64_t times) {//calculate zero with your random number generator
double sum{0.0};
for (uint64_t tries =0; tries < times; ++tries) {sum += rom::rand_sign();}
std::ostringstream os;
os << std::fixed << std::setprecision(12) <<(sum/double(times));
return os.str();
}

std::string rom_rand_test(uint64_t i){      // Testfunktion mit Konsolenausgabe
double sum{0.0}, min{2.0}, max{0.0};
for (uint64_t j{0}; j<i ;++j) {
	double number = rom::rand_0_1();
        sum += number;
        if (number < min)       {min = number;}
        if (number > max)       {max = number;}
        }
double average = (sum /double(i));
std::ostringstream os;
os << std::fixed <<std::setprecision(12) << " \tMin: " << min << " \tMax: "<< max << " \tAverage: " <<average;
return os.str();
}

void rom_rand_t(void) {
uint64_t i;
double d;
for (i=1; i<=(2.0*rom::_hecto*rom::_giga);i*=2) {
        std::cout<<"N: "<<i<<" \tPi: " <<monte_carlo_pi(i)<<" \tzero: "<< sign_test(i)<<rom_rand_test(double(i))<<"\n";
	}
}


#endif //rom_rand_h
