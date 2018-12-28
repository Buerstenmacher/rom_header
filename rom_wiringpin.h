#ifndef rom_wiringpin_h
#define rom_wiringpin_h

#include "rom_time.h"
#include "rom_error.h"
#include "wiringPi.h"

namespace rom{

uint64_t _WIRINGPI_IS_SETUP{0};	//ToDo make it a static membervariable

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class wiringpin {// this class creates a rpresentation for one GPIO Pin on Raspberry
private:
int8_t wp;      //pinnumber for wiringPi;
uint8_t val;    //last output value: 0->low  1->high  3->input
rom::sheepdelay delay;  //function object for timing

public:
friend std::ostream& operator<<(std::ostream& os, const wiringpin& pin) {return os << "WirinPi: "<<uint16_t(pin.wp);}

wiringpin(int8_t nr):wp(nr),delay{},val{3} {
if (_WIRINGPI_IS_SETUP++ == 0)	{wiringPiSetup();}
if ((wp>=0)  && (wp<=29))	{pinMode(wp,INPUT);}
else {rom::error("Pin number "+std::to_string(wp)+" is not defined in wirinPi-Library.\n");}
}

~wiringpin(void) {pinMode(wp,INPUT);}//high impendance input state for abandoned pins is the safest choice

inline void pullhi() {if (val != rom::_HIGH)	{digitalWrite(wp,rom::_HIGH);}}

inline void pulllo() {if (val != rom::_LOW)	{digitalWrite(wp,rom::_LOW);}}

inline void write(uint8_t bit) {
{if 	(val == 3)	{pinMode(wp,OUTPUT);}}
{if 	(bit ==_LOW)	{pulllo();}
else if (bit ==_HIGH)	{pullhi();}
else			{rom::error("");}}
val =bit;
}

inline void flow() {
if (val != 3)	{pinMode(wp,INPUT);}
val =3;
}

inline uint8_t read() {
flow();
return digitalRead(wp);
}

inline uint8_t flow_and_wait(double dt_sec=0.1) {//release gpio and wait for i2c pullup to pull line HIGH
flow();                                         //this may take some time if a i2c slave does clock-stretching on
double beg = rom::mashinetime() + dt_sec;        //a scl line //eine sekunde in der zukunft
do      {
        if (rom::mashinetime()>beg) {
                std::cout<<"Der GPIO mit Nummer "<<uint16_t(wp)<<" wiringpi wird seit "<<dt_sec<<" Sekunden von einem";
                std::cout<<" anderen Geraet auf Low gehalten oder von keinem Pullup Resistor auf HIGH gezogen\n";
                return 1; //error?  = true
                }
        } while (read() != rom::_HIGH);
return 0;  //Error? == false
}
};	//wiringpin
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

}	//namespace rom
#endif
