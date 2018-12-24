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

constexpr auto _one {_milli / _milli};	//divide a number by itself to get 1.0
constexpr auto _zero{_milli - _milli};	//subtract a number from itslf to get 0.0

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




}	//namespace rom
#endif	//rom_globals_h
