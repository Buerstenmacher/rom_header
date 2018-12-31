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

/* results on my intel i5 3.2ghz year 2018
N: 1 	Pi: 4.000000000000 	zero: 1.000000000000 	Min: 0.710671228979 	Max: 0.710671228979 	Average: 0.710671228979
N: 2 	Pi: 4.000000000000 	zero: 0.000000000000 	Min: 0.022712438628 	Max: 0.520643152573 	Average: 0.271677795601
N: 4 	Pi: 4.000000000000 	zero: -0.500000000000 	Min: 0.239276620808 	Max: 0.744280519972 	Average: 0.413023823603
N: 8 	Pi: 3.000000000000 	zero: 0.000000000000 	Min: 0.030914706309 	Max: 0.907119712256 	Average: 0.383096512894
N: 16 	Pi: 3.000000000000 	zero: -0.250000000000 	Min: 0.022078598727 	Max: 0.980166389634 	Average: 0.529536818702
N: 32 	Pi: 2.625000000000 	zero: -0.062500000000 	Min: 0.005706126344 	Max: 0.988583904088 	Average: 0.576468873122
N: 64 	Pi: 3.187500000000 	zero: 0.156250000000 	Min: 0.000327772424 	Max: 0.998628814753 	Average: 0.494070983334
N: 128 	Pi: 3.281250000000 	zero: 0.015625000000 	Min: 0.015117184016 	Max: 0.987284955485 	Average: 0.511259171980
N: 256 	Pi: 2.953125000000 	zero: 0.039062500000 	Min: 0.007151896830 	Max: 0.977041259346 	Average: 0.484409491145
N: 512 	Pi: 3.117187500000 	zero: 0.019531250000 	Min: 0.002867272265 	Max: 0.996623215761 	Average: 0.485571887593
N: 1024 	Pi: 3.191406250000 	zero: 0.003906250000 	Min: 0.000698789723 	Max: 0.999990966031 	Average: 0.515766677186
N: 2048 	Pi: 3.171875000000 	zero: 0.002929687500 	Min: 0.001105726117 	Max: 0.998882081295 	Average: 0.504088604455
N: 4096 	Pi: 3.124023437500 	zero: 0.010742187500 	Min: 0.000002213956 	Max: 0.999913662081 	Average: 0.505679305421
N: 8192 	Pi: 3.139648437500 	zero: 0.011230468750 	Min: 0.000083589533 	Max: 0.999972599910 	Average: 0.502328082212
N: 16384 	Pi: 3.127929687500 	zero: 0.004760742188 	Min: 0.000041635023 	Max: 0.999989148643 	Average: 0.501098261810
N: 32768 	Pi: 3.142211914062 	zero: 0.004760742188 	Min: 0.000006938223 	Max: 0.999990893395 	Average: 0.501984470614
N: 65536 	Pi: 3.142517089844 	zero: 0.000152587891 	Min: 0.000017760410 	Max: 0.999955071804 	Average: 0.498697383439
N: 131072 	Pi: 3.139129638672 	zero: 0.002578735352 	Min: 0.000002544976 	Max: 0.999970920451 	Average: 0.499904781876
N: 262144 	Pi: 3.145095825195 	zero: -0.002067565918 	Min: 0.000007730027 	Max: 0.999998377886 	Average: 0.499441054244
N: 524288 	Pi: 3.141265869141 	zero: -0.001659393311 	Min: 0.000001067978 	Max: 0.999999703288 	Average: 0.499962414799
N: 1048576 	Pi: 3.141555786133 	zero: -0.000558853149 	Min: 0.000001663997 	Max: 0.999998804749 	Average: 0.499896893580
N: 2097152 	Pi: 3.140022277832 	zero: -0.000518798828 	Min: 0.000000173364 	Max: 0.999999510213 	Average: 0.499698850504
N: 4194304 	Pi: 3.141699790955 	zero: -0.000276088715 	Min: 0.000000159416 	Max: 0.999999459946 	Average: 0.500294331831
N: 8388608 	Pi: 3.141695976257 	zero: 0.000085830688 	Min: 0.000000084484 	Max: 0.999999862818 	Average: 0.499927085490
N: 16777216 	Pi: 3.140638113022 	zero: 0.000113606453 	Min: 0.000000018510 	Max: 0.999999712773 	Average: 0.499985687470
N: 33554432 	Pi: 3.141685724258 	zero: -0.000004827976 	Min: 0.000000020254 	Max: 0.999999983490 	Average: 0.500010487831
N: 67108864 	Pi: 3.141667485237 	zero: 0.000134497881 	Min: 0.000000008651 	Max: 0.999999989706 	Average: 0.500065546431
N: 134217728 	Pi: 3.141476005316 	zero: 0.000027924776 	Min: 0.000000004857 	Max: 0.999999985651 	Average: 0.499943440434
N: 268435456 	Pi: 3.141551941633 	zero: 0.000076711178 	Min: 0.000000000048 	Max: 0.999999997440 	Average: 0.499984224459
N: 536870912 	Pi: 3.141635119915 	zero: -0.000031005591 	Min: 0.000000004818 	Max: 0.999999999986 	Average: 0.499999606248
N: 1073741824 	Pi: 3.141649372876 	zero: 0.000016491860 	Min: 0.000000000924 	Max: 0.999999999403 	Average: 0.499996810173
N: 2147483648 	Pi: 3.141614813358 	zero: -0.000005506910 	Min: 0.000000001169 	Max: 0.999999999919 	Average: 0.500002967880
N: 4294967296 	Pi: 3.141557605006 	zero: -0.000008974224 	Min: 0.000000000152 	Max: 0.999999999751 	Average: 0.499995640187
N: 8589934592 	Pi: 3.141572627705 	zero: 0.000011244556 	Min: 0.000000000234 	Max: 0.999999999909 	Average: 0.499994059808
N: 17179869184 	Pi: 3.141587557504 	zero: -0.000000162516 	Min: 0.000000000073 	Max: 0.999999999847 	Average: 0.499998979514
N: 34359738368 	Pi: 3.141608504229 	zero: -0.000001212175 	Min: 0.000000000055 	Max: 0.999999999952 	Average: 0.500000080589
N: 68719476736 	Pi: 3.141589586972 	zero: -0.000002728630 	Min: 0.000000000008 	Max: 0.999999999996 	Average: 0.500000395252
N: 137438953472 	Pi: 3.141585188510 	zero: 0.000002590314 	Min: 0.000000000007 	Max: 0.999999999999 	Average: 0.500000169764
*/
