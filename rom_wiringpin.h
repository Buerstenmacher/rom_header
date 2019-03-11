#ifndef rom_wiringpin_h
#define rom_wiringpin_h

#include <array>
#include "rom_time.h"
#include "rom_error.h"
//#include "wiringPi++.h"	//use of wiringPi++ c++ translation of wiringPi by Buerstenmacher aka (Roman Lechner)
#include "wiringPi.h"		//use of wiringPi from Gordon Henderson

namespace rom{

//-******************************************************************************
class wiringpin {// this class creates a rpresentation for one GPIO Pin on Raspberry
private:
int8_t wp;      //pinnumber for wiringPi;
uint8_t val;    //last output value: 0->low  1->high  3->input

static uint8_t wp_is_setup(uint8_t inp = 0) {
static uint8_t is_it{0};	//this static variabe should tell us if wiringPiSetup had been called in the past
if (inp) {is_it = 1;}
return is_it;
}

inline void pullhi() {if (val != rom::_HIGH)	{digitalWrite(wp,rom::_HIGH);}}

inline void pulllo() {if (val != rom::_LOW)	{digitalWrite(wp,rom::_LOW);}}

inline void flow() {
if (val != 3)	{pinMode(wp,INPUT);}
val = 3;
}

public:
friend std::ostream& operator<<(std::ostream& os, const wiringpin& pin) {return os << "WirinPi: "<<uint16_t(pin.wp);}

wiringpin(int8_t nr):wp(nr),val{3} {
if (wp_is_setup() == 0) {
	wiringPiSetup();
	wp_is_setup(1);
	}
if ((wp>=0)  && (wp<=29))	{pinMode(wp,INPUT);}
else {rom::error("Pin number "+std::to_string(wp)+" is not defined in wirinPi-Library.\n");}
}

~wiringpin(void) {pinMode(wp,INPUT);}//high impendance input state for abandoned pins is the safest choice

inline void write(uint8_t bit) {
{if 	(val == 3)	{pinMode(wp,OUTPUT);}}
{if 	(bit ==_LOW)	{pulllo();}
else if (bit ==_HIGH)	{pullhi();}
else			{rom::error("error in function rom::wiringpin.write()");}}
val =bit;
}

inline uint8_t read() {
flow();
return digitalRead(wp);
}

};	//wiringpin
//-******************************************************************************


//-******************************************************************************
class pin:public wiringpin{     //this class simplifies the use of wiringpin,
private:                        //you do not need to know the wiringpi-number of a pin

static int8_t pintab(uint8_t inp) {
static const std::array<int8_t,41> pintable  {    //you simply take the number of the pin itself
//wPi   //Physical //wPi //physical
-1,      //0
-1,     /*1*/
-1,     //2
8,      /*3*/   -1,     //4
9,      /*5*/   -1,     //6
7,      /*7*/   15,     //8
-1,     /*9*/   16,     //10
0,      /*11*/  1,      //12
2,      /*13*/  -1,     //14
3,      /*15*/  4,      //16
-1,     /*17*/  5,      //18
12,     /*19*/  -1,     //20
13,     /*21*/  6,      //22
14,     /*23*/  10,     //24
-1,     /*25*/  11,     //26
30,     /*27*/  31,     //28
21,     /*29*/  -1,     //30
22,     /*31*/  26,     //32
23,     /*33*/  -1,     //34
24,     /*35*/  27,     //36
25,     /*37*/  28,     //38
-1,     /*39*/  29,     //40
};
return pintable.at(inp);
}

public:
pin(uint8_t pinnr):wiringpin((pinnr<41)?pintab(pinnr):(-1)) {}
};//-******************************************************************************

}	//namespace rom


void rom_wiringpin_t(void){
rom::pin pin(40);
for (uint16_t i{0};i<10;++i) {
	std::cout << "1" <<std::endl;
	pin.write(1);
	delay (500) ;
	std::cout << "0" <<std::endl;
	pin.write(0);
	delay (500) ;
	}
std::cout << std::endl;
std::cout << static_cast<uint16_t>(pin.read()) << std::endl;
}

#endif
