////////////////////////////////////////////////////////////////////////////////////////////////////
//This is a timing library
//
//there is no distinction between timepoints and duration forced by the compiler
//the unit for time is always secons (SI-unit); it can be encoded in float, double or long double
//timepoints should a least make use of double (64bit floating-point) for good accuracy
//there is no need for an extra .cpp file but this header depends on, at least, one other header "rom_globals.h"
//
//
//compiling with -std=c++11 is mandatory
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef rom_time_h
#define rom_time_h

#include <time.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <iostream>  //remove after debugging
#include "rom_globals.h"
#include "rom_error.h"

namespace rom {

typedef std::chrono::high_resolution_clock prfclk;	//prefered c++11-clock

inline double mashinetime() {	//returns the nummber of seconds since 1.1.1970 as precise as possible
static prfclk cl;
return cl.now().time_since_epoch().count()*ratio_value<prfclk::period>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
class delay {	//functor for break of dt seconds
public:		//pro:  good accuracy down to 10microsecond on raspberry-pi-zero-w;
		//	down to 200 nanosecons on-intel-i5-2016
delay(void) {}	//contra: cpu stress; power consumtion

inline void operator()(float dt) {			//cause a delay od dt seconds
static double start;	start = mashinetime()+dt;	//it will stress your cpu if delay is longer than 0.1sec
while(mashinetime() < start){/*do nothing*/};
}

}; //class delay
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
class sleepdelay {	//functor for break of dt seconds
public:			//pro:	no stress for cpu;  lowest power consumption
sleepdelay(void){}	//contra:	bad accuracy below 10ms

inline void operator()(float dt) {	//cause a delay od dt seconds
if (dt>=0.999) {			//recursion if dt is longer than 1.0 seconds
	this->operator()(dt/2.0);
	this->operator()(dt/2.0);
	return;
	}
static uint64_t ns;	ns = static_cast<uint64_t>(dt * _ns_p_s);
static struct timespec req={0},rem={0};
req.tv_sec=time_t(0);
req.tv_nsec=ns;
nanosleep(&req,&rem);
}
}; //class sleepdelay
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
class sheepdelay {	//functor for break of dt seconds
private:		//pro:  medium accuracy down to 200nanoseconds on raspberry-pi-zero-w
			//	down to 10 nanosecons on intel-i5-3.2-ghz-2016
float sheep_ps,functioncall_ps;	//contra: 	cpu stress; power consumtion
				//		time for initialisation
inline void sheepcount(volatile uint32_t in) {while(in) {--in;}}

double sheep_p_s(void) {        //bestimmt wieviel der prozessor in einer sekunde zaehlen kann
double t1,t2;                   //ca 100 000 000 schaft der raspberry pi II
uint32_t sheeps = 1000*1000*100;
t1 = mashinetime();
sheepcount(sheeps);
t2 = mashinetime();
return sheeps/(t2-t1);
}

double functioncall_p_s(void) { //bestimmt wie oft der prozessor die leere funktion pro sek ausführen kann
double t1,t2;                   //ca 30 000 000 pro sekunde an rasperry pi II
uint32_t n = 1000*1000*10;
uint32_t i = n;
t1 = mashinetime();
while (n) {sheepcount(0);--n;}
t2 = mashinetime();
return i/(t2-t1);
}

public:
sheepdelay(void):sheep_ps(sheep_p_s()),functioncall_ps(functioncall_p_s()) {}

inline void operator()(float s) {
static float n_sheeps{}; n_sheeps = (s-(2.0/functioncall_ps))*sheep_ps;
if (n_sheeps<=0.0) {return;}
if (n_sheeps>=std::numeric_limits<uint32_t>::max()) {	//recursion to avoid integer overflow
	this->operator()(s/2.0);
	this->operator()(s/2.0);
	return;
	}
sheepcount(static_cast<uint32_t>(n_sheeps));
}

};	//class sheepdelay
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
class autodelay {	//functor for break of dt seconds
private:		//the obove funftion classes force you to make a decision with to us
sheepdelay sheep;	//autodelay is here to make the best choice for you
sleepdelay sleep;
delay	del;

public:
autodelay(double expected_dt= _micro) {}

inline void operator()(float sec) {
if (sec < 1.0 * _micro) {sheep(sec);}
else if (sec > 0.1) {sleep(sec);}
else {del(sec);}
}

};	//class autodelay
////////////////////////////////////////////////////////////////////////////////////////////////////

inline uint8_t is_past(double time)   {return (time <= rom::mashinetime());}

inline uint8_t is_future(double time) {return (time > rom::mashinetime());}

inline void wait_until(double time)   {do {} while (is_future(time));}

////////////////////////////////////////////////////////////////////////////////////////////////////
class humantime {		//This Functor is able to represent the computer time
const std::string TIMENAME;	//in an human readable String, you can provide a double as
				//timestamp or get the actual time.
public:			//Example	//"LOC 2018 Dec 10 18:29:03.856838"
humantime(void):TIMENAME{"LOC "}{}	//"xxx_jjjj_mmm_dd_hh:mm:ss.--us--"
					//The result is optimized for lexikographical comparison
std::string operator()(double mt=rom::mashinetime()) {	// Diese funktion liefert die eingelieferte oder die konvertierte Zeit in einem lesbaren Format als string zurueck
if (mt>rom::_long_int_max) {::rom::error("Sorry your mashine cannot handle dates after "+this->operator()(_long_int_max));}
long int sek = (long int)mt;				//No!!!  Do not change it!!! you neeeed ugly long int!!!
uint32_t usek = uint32_t((mt-double(sek))*_us_p_s);
std::string s = ctime(static_cast<time_t *>(&sek));
std::copy_n(s.begin()+20,4,s.begin());
s.insert (4," ");
s.resize(20);
s = TIMENAME+s+std::to_string(uint32_t(_mega) +  usek);	//here is a problem: to_string() supresses leading zeroes !!!!*
*(s.end()-7 )='.';					//solution is to add 1000 000 and delete the "1" later
return s;
}
};	//class humantime
////////////////////////////////////////////////////////////////////////////////////////////////////

double mashinetime(std::string ht,uint16_t genauigkeit=1023) {
genauigkeit = (ht.size()<genauigkeit)?ht.size():genauigkeit;
if(genauigkeit == 0)	{return  _giga;}
auto mti1{mashinetime(ht,genauigkeit-1)};
auto mti2{mti1};
double muli{-1.01};
auto inkr = 10.0*_nano;
auto mis_it = ht.begin();
do 	{
	mti2 += inkr;
	inkr *= muli;
	auto hz = humantime{}(mti2);
	mis_it = std::mismatch(ht.begin(),ht.end(),hz.begin()).first;
	}while ((std::distance(ht.begin(),mis_it) < genauigkeit) && (fabs(inkr) < 20*_giga));
	if (fabs(inkr) >= 20*_giga)   {return mti1;}	//std::cout <<"Error bei genauigkeit " << genauigkeit<< "\n"; 
return (mti2);   //std::cout << "Die Zeitumrechnung von " << ht << " mit genauigkeit "<< genauigkeit<<" ist abgeschlossen. Der inkremetn war "<< inkr<<"\n" ;
}

}	//namespace rom
#endif	//rom_time_h

