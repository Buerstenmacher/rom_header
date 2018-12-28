#ifndef rom_globals_h
#define rom_globals_h
#include <ratio>
#include <limits>
#include <complex>

namespace rom {

template<typename Ra,typename ret=long double>
constexpr ret ratio_value() {return(static_cast<ret>(Ra::num))/static_cast<ret>(Ra::den);}

constexpr auto _atto =	ratio_value<std::atto >();//std::ratio<1, 1000000000000000000>
constexpr auto _femto =	ratio_value<std::femto>();//std::ratio<1, 1000000000000000>
constexpr auto _pico =	ratio_value<std::pico >();//std::ratio<1, 1000000000000>
constexpr auto _nano =	ratio_value<std::nano >();//std::ratio<1, 1000000000>
constexpr auto _micro =	ratio_value<std::micro>();//std::ratio<1, 1000000>
constexpr auto _milli =	ratio_value<std::milli>();//std::ratio<1, 1000>
constexpr auto _centi =	ratio_value<std::centi>();//std::ratio<1, 100>
constexpr auto _deci =	ratio_value<std::deci >();//std::ratio<1, 10>
constexpr auto _deca =	ratio_value<std::deca >();//std::ratio<10, 1>
constexpr auto _hecto =	ratio_value<std::hecto>();//std::ratio<100, 1>
constexpr auto _kilo =	ratio_value<std::kilo >();//std::ratio<1000, 1>
constexpr auto _mega =	ratio_value<std::mega >();//std::ratio<1000000, 1>
constexpr auto _giga =	ratio_value<std::giga >();//std::ratio<1000000000, 1>
constexpr auto _tera =	ratio_value<std::tera >();//std::ratio<1000000000000, 1>
constexpr auto _peta =	ratio_value<std::peta >();//std::ratio<1000000000000000, 1>
constexpr auto _exa =	ratio_value<std::exa  >();//std::ratio<1000000000000000000, 1>

constexpr auto _ns_p_s =	ratio_value<std::giga>();	//nano seconds per second
constexpr auto _ns_p_ms=	ratio_value<std::mega>();	//nano seconds per milli second
constexpr auto _ns_p_us=	ratio_value<std::kilo>();	//nano seconds per micro second
constexpr auto _us_p_s =	ratio_value<std::mega>();	//micro seconds per second
constexpr auto _s_p_us =	ratio_value<std::micro>();	//seconds per micro second
constexpr auto _s_p_ns =	ratio_value<std::nano>();	//seconds per nano second
constexpr auto _s_p_ms =	ratio_value<std::milli>();	//seconds per milli second

constexpr auto _one {_milli / _milli}; //divide a number by itself to get 1.0; 	floating point 1.0
constexpr auto _zero{_milli - _milli}; //subtract a number from itslf to get 0.0;floating point 0.0
constexpr auto _neg_one {_zero - _one}; //you get it; 	floating point -1.0

#ifndef TRUE	//alternative to keyword "true"
constexpr uint8_t TRUE (_zero < _one); 	//yes that's true ;-)
#endif //TRUE

#ifndef FALSE	//alternative to keyword "false"
constexpr uint8_t FALSE {!TRUE};
#endif //FALSE

constexpr auto _true 	{TRUE};		//this is useles :-)
constexpr auto _false	{FALSE};	//this aswell	;-)

constexpr std::complex<decltype(_one)> _i(_zero,_one);  //sqare root of (-1)
constexpr std::complex<decltype(_one)> _j(_zero,_one);  //sqare root of (-1)

constexpr uint8_t _HIGH{uint8_t(_one)};		//integer 1
constexpr uint8_t _LOW {uint8_t(_zero)};	//integer 0

constexpr auto _EARTH_G(9.80665);	//[(m/(s*s))/G]  Surface acceleration on Earth (average Value)
constexpr auto _GAUSS_P_TESLA(10.0 * _kilo);		//unit conversion for magnetic flux density
constexpr auto _TESLA_P_GAUSS(_one/_GAUSS_P_TESLA);	//unit conversion for magnetic flux density
constexpr auto _EARTH_RADIUS(6371000.785);             //meter  (average Value)

constexpr auto _ROM_PI_FUNC(void)->decltype(_neg_one){	//copute pi at compiletime (3.1415..............)
//acos(_neg_one);					//acos of -1.0 should be pi or -pi
return (acos(_neg_one)>_zero)?(acos(_neg_one)):(_neg_one*acos(_neg_one));	//return the positive value
}

constexpr auto _ROM_PI{_ROM_PI_FUNC()};	

constexpr auto _GRAD_P_RAD (180.0/_ROM_PI);	//conversion factor of angle units
constexpr auto _RAD_P_GRAD (_ROM_PI/180.0);	//conversion factor of angle units

constexpr auto _M_P_FEET(double(0.3048));	//speed unit conversion factor
constexpr auto _KNOT_P_MPS(double(1.943844));  	//speed unit conversion factor
constexpr auto _MPS_P_KNOT (_one/_KNOT_P_MPS);	//speed unit conversion factor

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


}	//namespace rom
#endif	//rom_globals_h
