#ifndef rom_globals_h
#define rom_globals_h
#include <ratio>
#include <limits>
#include <complex>
#include <cmath>
namespace rom {

template<typename Ra,typename ret>
constexpr ret ratio_value() {return(static_cast<ret>(Ra::num))/static_cast<ret>(Ra::den);}

template<class flt>
constexpr flt _atto(){return ratio_value<std::atto,flt>();}//std::ratio<1, 1000000000000000000>

template<class flt>
constexpr flt _femto(){return ratio_value<std::femto,flt>();}//std::ratio<1, 1000000000000000>

template<class flt>
constexpr flt _pico(){return ratio_value<std::pico,flt>();}//std::ratio<1, 1000000000000>

template<class flt>
constexpr flt _nano(){return ratio_value<std::nano,flt>();}//std::ratio<1, 1000000000>

template<class flt>
constexpr flt _micro(){return ratio_value<std::micro,flt>();}//std::ratio<1, 1000000>

template<class flt>
constexpr flt _milli(){return ratio_value<std::milli,flt>();}//std::ratio<1, 1000>

template<class flt>
constexpr flt _centi(){return ratio_value<std::centi,flt>();}//std::ratio<1, 100>

template<class flt>
constexpr flt _deci(){return ratio_value<std::deci,flt>();}//std::ratio<1, 10>

template<class flt>
constexpr flt _deca(){return ratio_value<std::deca,flt>();}//std::ratio<10, 1>

template<class flt>
constexpr flt _hecto(){return ratio_value<std::hecto,flt>();}//std::ratio<100, 1>

template<class flt>
constexpr flt _kilo(){return ratio_value<std::kilo,flt>();}//std::ratio<1000, 1>

template<class flt>
constexpr flt _mega(){return ratio_value<std::mega,flt>();}//std::ratio<1000000, 1>

template<class flt>
constexpr flt _giga(){return ratio_value<std::giga,flt>();}//std::ratio<1000000000, 1>

template<class flt>
constexpr flt _tera(){return ratio_value<std::tera,flt>();}//std::ratio<1000000000000, 1>

template<class flt>
constexpr flt _peta(){return ratio_value<std::peta,flt>();}//std::ratio<1000000000000000, 1>

template<class flt>
constexpr flt _exa(){return ratio_value<std::exa,flt>();}//std::ratio<1000000000000000000, 1>


constexpr auto _ns_p_s = ratio_value<std::giga,double>();	//nano seconds per second
constexpr auto _ns_p_ms= ratio_value<std::mega,double>();	//nano seconds per milli second
constexpr auto _ns_p_us= ratio_value<std::kilo,double>();	//nano seconds per micro second
constexpr auto _us_p_s = ratio_value<std::mega,double>();	//micro seconds per second
constexpr auto _s_p_us = ratio_value<std::micro,double>();	//seconds per micro second
constexpr auto _s_p_ns = ratio_value<std::nano,double>();	//seconds per nano second
constexpr auto _s_p_ms = ratio_value<std::milli,double>();	//seconds per milli second

template<class flt>
constexpr flt _one() {return (_milli<flt>() / _milli<flt>());} //divide a number by itself to get 1.0; 	floating point 1.0

template<class flt>
constexpr flt _zero(){return (_milli<flt>() - _milli<flt>());} //subtract a number from itslf to get 0.0;floating point 0.0

template<class flt>
constexpr flt _neg_one() {return (_zero<flt>() - _one<flt>());} //you get it; 	floating point -1.0


template<typename ret>		//It has to be a constexpr function
constexpr std::complex<ret> _i(){	//otherwise there will be a problem with "const std::complex<const float>"
return std::complex<ret>{_zero<ret>(),_one<ret>()};	//return sqare root of (-1)
}

template<typename ret>		//same as above
constexpr std::complex<ret> _j(){return rom::_i<ret>();}	//return sqare root of (-1)

template<typename ret>		//same as above
constexpr std::complex<ret> _complex_zero(){
return std::complex<ret>{_zero<ret>(),_zero<ret>()};	//return zero in complex number
}

constexpr uint8_t _HIGH{uint8_t(_one<double>())};		//integer 1
constexpr uint8_t _LOW {uint8_t(_zero<double>())};	//integer 0

constexpr auto _EARTH_G(9.80665);	//[(m/(s*s))/G]  Surface acceleration on Earth (average Value)
constexpr auto _GAUSS_P_TESLA(10.0 * _kilo<double>());		//unit conversion for magnetic flux density
constexpr auto _TESLA_P_GAUSS(_one<double>()/_GAUSS_P_TESLA);	//unit conversion for magnetic flux density
constexpr auto _EARTH_RADIUS(6371000.785);             //meter  (average Value)

template<class flt>
constexpr flt _pit()	{return acos(_neg_one<flt>());}	//can be positive pi or negative pi

template<class flt>
constexpr flt _PI(void)	{//copute pi at compiletime (3.1415..............)
return (_pit<flt>() >= _zero<flt>())? _pit<flt>() : (_neg_one<flt>()*_pit<flt>());
}	//returns positive pi at compiletime

template<class flt>
constexpr flt _e(void) {return ::exp(_one<flt>());}//compute "eulers number" at compilet. (2.718281...)

constexpr auto _GRAD_P_RAD (180.0/rom::_PI<double>());	//conversion factor of angle units
constexpr auto _RAD_P_GRAD (rom::_PI<double>()/180.0);	//conversion factor of angle units

constexpr auto _M_P_FEET(double(0.3048));	//length unit conversion factor
constexpr auto _KNOT_P_MPS(double(1.943844));  	//speed unit conversion factor
constexpr auto _MPS_P_KNOT (_one<double>()/_KNOT_P_MPS);//speed unit conversion factor

constexpr auto _short_int_max 	{std::numeric_limits<short int>::max()};
constexpr auto _int_max 	{std::numeric_limits<int>::max()};
constexpr auto _long_int_max 	{std::numeric_limits<long int>::max()};
constexpr auto _long_long_int_max{std::numeric_limits<long long int>::max()};

constexpr auto _short_int_min 	{std::numeric_limits<short int>::min()};
constexpr auto _int_min 	{std::numeric_limits<int>::min()};
constexpr auto _long_int_min 	{std::numeric_limits<long int>::min()};
constexpr auto _long_long_int_min{std::numeric_limits<long long int>::min()};

constexpr auto _uint8_t_max 	{std::numeric_limits<uint8_t>::max()};
constexpr auto _uint16_t_max 	{std::numeric_limits<uint16_t>::max()};
constexpr auto _uint32_t_max 	{std::numeric_limits<uint32_t>::max()};
constexpr auto _uint64_t_max 	{std::numeric_limits<uint64_t>::max()};

constexpr auto _streamsize_max	{std::numeric_limits <std::streamsize>::max()};

template<typename fltp>		//every floating point type
constexpr fltp _max_acceptable_error(){
return (std::numeric_limits<fltp>::epsilon()*256*1024); 	//i would accept a large multiple of epsilon()
}//after a long chain of calculations, you should be able to calculate back without exceeding this error


}	//namespace rom
#endif	//rom_globals_h



//	---Backup---
/*
#ifndef TRUE	//alternative to keyword "true"
constexpr uint8_t TRUE (_zero < _one); 	//yes that's true ;-)
#endif //TRUE

#ifndef FALSE	//alternative to keyword "false"
constexpr uint8_t FALSE {!TRUE};
#endif //FALSE

constexpr auto _true 	{TRUE};		//this is useles :-)  //you'l better use built in keywords
constexpr auto _false	{FALSE};	//this aswell	;-)
*/
