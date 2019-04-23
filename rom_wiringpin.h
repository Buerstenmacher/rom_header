#ifndef rom_wiringpin_h
#define rom_wiringpin_h

#include <array>
#include "rom_spacetime.h"
#include "rom_time.h"
#include "rom_error.h"
//#include "wiringPi++.h"	//use of wiringPi++;  c++ translation of wiringPi by Buerstenmacher
#include "wiringPi.h"		//use of wiringPi from Gordon Henderson


/*PINBELEGUNG RASPBERRY PI 2, August 2015

 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 | ALT0 | 1 |  3 || 4  |   |      | 5V      |     |     |
 |   3 |   9 |   SCL.1 | ALT0 | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |  OUT | 0 | 11 || 12 | 0 | OUT  | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | OUT  | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | ALT0 | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO | ALT0 | 0 | 21 || 22 | 0 | OUT  | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | ALT0 | 0 | 23 || 24 | 1 | ALT0 | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 1 | ALT0 | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 0 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+ */

namespace rom{

//-******************************************************************************
class wiringpin {// this class creates a rpresentation for one GPIO Pin on Raspberry
private:
int8_t wp;      //pinnumber for wiringPi;
uint8_t val;    //last output value: 0->low  1->high  3->input

static uint8_t wp_is_setup(uint8_t inp = 0) {
static uint8_t is_it{0};//this static variabe should tell us if wiringPiSetup had been called in the past
if (inp) {is_it = 1;}
return is_it;
}

public:
friend std::ostream& operator<<(std::ostream& os, const wiringpin& pin) {return os << "WiringPi: "<<uint16_t(pin.wp);}

wiringpin(int8_t nr):wp(nr),val{3} {
if (wp_is_setup() == 0) {
	wiringPiSetup();
	wp_is_setup(1);
	}
if ((wp>=0)  && (wp<=29))	{pinMode(wp,INPUT);}
else {rom::error("Pin number "+std::to_string(wp)+" is not defined in wirinPi-Library.\n");}
}

~wiringpin(void) {pinMode(wp,INPUT);}//high impendance input state for abandoned pins is the safest choice

inline void pullhi() {	//set digital output to high voltage
if (val == 3)           {pinMode(wp,OUTPUT);}
if (val != rom::_HIGH)	{digitalWrite(wp,rom::_HIGH);}
val =1;
}

inline void pulllo() {	//set digital output to low voltage
if (val == 3)           {pinMode(wp,OUTPUT);}
if (val != rom::_LOW)	{digitalWrite(wp,rom::_LOW);}
val =0;
}

inline void flow() {	//switch gpio to high impendance input
if (val != 3)     {pinMode(wp,INPUT);}
val =3;
}

inline void write(uint8_t bit) {	//write 0 or 1 to gpio output
{if 	(val == 3)	{pinMode(wp,OUTPUT);}}
{if 	(bit ==rom::_LOW)	{pulllo();}
else if (bit ==rom::_HIGH)	{pullhi();}
else			{rom::error("error in function rom::wiringpin.write()");}}
val =bit;
}

inline uint8_t read() {	//set gpio to input and read it's state
flow();
return digitalRead(wp);
}

/*
inline uint8_t flow_and_wait(double dt_sec=0.1) {//release gpio and wait for i2c pullup to pull line HIGH
flow();                                         //this may take some time if a i2c slave does clock-stretching on
double beg = rom_mashinetime() + dt_sec;        //a scl line //eine sekunde in der zukunft
do      {
        if (rom_mashinetime()>beg) {
                std::cout<<"Der GPIO mit Nummer "<<uint16_t(wp)<<" wiringpi wird seit "<<dt_sec<<" Sekunden von einem";
                std::cout<<" anderen Geraet auf Low gehalten oder von keinem Pullup Resistor auf HIGH gezogen\n";
                return 1; //error?  = true
                }
        } while (read() != ROM_HIGH);
return 0;  //Error? == false
}*/

};	//wiringpin
//-******************************************************************************

//-******************************************************************************
class pin:public wiringpin{     	//this class simplifies the use of wiringpin,
private:                        	//you do not need to know the wiringpi-number of a pin
static int8_t pintab(uint8_t inp) {	//you simply take the physical number of the pin itself
static const std::array<int8_t,41> pintable  {
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


//-******************************************************************************
//This Clas creates an software- I2c -Master on two gpio Pins of an rasperry Pi
//It depends on beeing on an original Raspberry Pi Computer and
//having wiringPi from Gordon Henderson properly installed (see Website "wiringpi.com")
//It does NOT depend on i2c interface beeing enabled on the "rasperry Pi Configuration"
//ToDo:  always mask the read/write bit, so it does not matter witch address is given to a function
class i2c_master {
friend class max1139eee;//A-D converter
friend class mcp4728;   //D-A converter
private:                //Suppors multiple Slaves & clock streching
wiringpin sda;          //does not suport multiple Masters
wiringpin scl;
uint64_t readcnt;       // byte counter
uint64_t writecnt;      // byte counter
const double bittime;   // (1.0/freqency)
rom::autodelay rdel;	//delayclass for short delays down to fractions of microseconds

static int16_t reinterpret_s16(uint16_t u16) {return int16_t(*reinterpret_cast<int16_t*>(&u16));}

inline uint8_t flow_and_wait(rom::wiringpin& pinin,double dt_sec=0.2) {
pinin.flow();					//release gpio and wait for i2c pullup resistor to pull line HIGH
double time = rom::mashinetime() + dt_sec;	//this may take some time if a i2c slave does clock-stretching on
do      {					//a scl line, so we will wait until dt_sec seconds have passed
        if (rom::mashinetime()>time) {
                std::cout<<"The GPIO with number "<<pinin<<" wiringpi is tied to low voltage for "<<dt_sec;
                std::cout<<" seconds. \n There might be some hardware problem on the i2c bus";
                return 1; //exit with error: line dos not go to high voltage as expected
                }
        } while (pinin.read() != rom::_HIGH);
return 0;  //exit normally
}

void clock_out_error(uint16_t times){
while (times--) {
        scl.pulllo();		rdel(bittime * 1.5);
        flow_and_wait(scl);	rdel(1.5 * bittime);
        }
send_stop();
}

void wait_for_free_bus(float maxtime=1.0) {
sda.flow();
scl.flow();
int16_t fail;
auto start{rom::mashinetime()};
do      {
        fail =0;
        for (uint8_t i =0; i<15;i++) {  //test both lines for low voltage 15 times
                rdel(bittime*0.2);
                if ((sda.read() == rom::_LOW) || (scl.read()==rom::_LOW)) { fail++;}
                }
        }while ((fail) && (is_future(start+maxtime)));	//try for maxtime
if (fail == 0)  {return;}		//exit normally if both lines stood low for short time
clock_out_error(32);			//try to remove errors of i2c slaves
start=rom::mashinetime();		//try again
do      {
        fail =0;
        for (uint8_t i =0; i<15;i++) {
                rdel(bittime*0.2);
                if ((sda.read() == rom::_LOW) || (scl.read()==rom::_LOW)) { fail++;}
                }
        }while ((fail) && (is_future(start+maxtime)));
if (fail == 0)  {return;}		//exit normaly if second chance has worked; end progrm if not; 
else    {rom::error("wait for free bus failed because i2c-bus is more than one second busy");}
}

void send_start(void) {
sda.flow();
flow_and_wait(scl);
wait_for_free_bus();    rdel(bittime*0.5);
sda.pulllo();           rdel(bittime*0.5);//pause halbes Bit
scl.pulllo();           rdel(bittime*0.5);// halbes bit Pause (tw(scll))
}

void send_bit(uint8_t bin) {
if (bin==1)     {sda.flow();}
if (bin==0)     {sda.pulllo();} rdel(0.5 * bittime);//tsu(sda)
flow_and_wait(scl);		rdel(0.5 * bittime);//tw(sclh)
scl.pulllo();                   rdel(0.5 * bittime);//th(sda)
}

uint8_t sakn(void) {
sda.flow();                             rdel(0.5 * bittime);
flow_and_wait(scl);			rdel(0.5 * bittime);
static uint8_t sdatmp;
sdatmp = sda.read();                    rdel(0.5 * bittime);
scl.pulllo();                           rdel(0.5 * bittime);
return  (sdatmp)?0:1;
}

void send_stop(void) {
sda.pulllo();                           rdel(0.5 * bittime);
flow_and_wait(scl);			rdel(0.5 * bittime);
sda.flow();                             rdel(1.0 * bittime);
}

void send_repeated_start(void) {
sda.flow();                             rdel(0.5 * bittime);
flow_and_wait(scl);			rdel(0.5 * bittime);
sda.pulllo();                           rdel(0.5 * bittime);
scl.pulllo();                           rdel(0.5 * bittime);
sda.flow();                             rdel(0.5 * bittime);
}

void send_mak(void) {//does not work; never try this again!
sda.pulllo();                           rdel(0.5 * bittime);
scl.flow();                             rdel(0.5 * bittime);
scl.pulllo();                           rdel(0.5 * bittime);
sda.flow();                             rdel(0.5 * bittime);
}

uint8_t read_byte(void) {
uint8_t bits;			//storage for single bits
uint8_t temp = 0;		//storage for return value
for (int8_t n=7;n>=0;n--) {	//little endian on i2c bus
        flow_and_wait(scl);		rdel(0.5 *bittime);
        bits = sda.read();
        bits <<= n;                     rdel(0.5 * bittime);
        scl.pulllo();                   rdel(0.5 * bittime);
        temp+=bits;
        }
return temp;
}

public:
void send_byte(uint8_t by)      {for (int8_t n=7;n >=0; n--)    {send_bit(rom::getbit(by,n));}}

i2c_master(const i2c_master& in) = delete;		//no copy!
i2c_master operator=(const i2c_master& in) = delete; 	//no copy!

i2c_master(uint8_t sdi=8,uint8_t sci=9,double freqin=100000):   sda(sdi),scl(sci),readcnt(0),
                                                                writecnt(0),bittime(1.0/freqin),rdel{}
{
rdel(0.0);
wait_for_free_bus();
sda.flow();	//obsolete
scl.flow();	//obsolete
}

rom::autodelay& borrow_delayfunctor(void) {	//objects that include an i2c master can get access to it's delay
return rdel;					//functor. This may save some time for initialisation
}

~i2c_master() {//destructor with or without message
//std::cout<<"An I2c-bus-Master get's destroyed wich transmitted " <<readcnt+writecnt<< " Bytes.";
//std::cout<<std::endl;
}

uint8_t check_slave_adress(uint8_t sadr) {
send_start();
send_byte(uint8_t(sadr));
if (!sakn())	{
	send_stop();
	return 0;
	}	//no response on this adress
else    {
	send_stop();
	return 1;
	}
}

uint8_t read_u8_reg(uint8_t reg,uint8_t sadr) { //read one Byte From register (reg) from slave(sadr)
send_start();
send_byte(sadr & ~0x01);	//lsb gets reset
if (!sakn()) {
        send_stop();
        rom::error("No reply to i2c command!");
        }
send_byte(reg);
if (!sakn()) {
        send_stop();
        rom::error("No reply to i2c register command!");
        }
send_repeated_start();
send_byte(sadr | 0x01);		//LSB gets set to get read adress of slave
if (!sakn()) {
        send_stop();
        rom::error("No reply to i2c read comman");
        }
uint8_t tmp = read_byte();
send_stop();
readcnt++;
return tmp;
}

uint16_t read_u16_reg(uint8_t reg,uint8_t sadr) {//reads 2 8Byte register and creates an unsigned 16 int
/*      Backup			//THE COMMENT DOES NOT WORK! READ 2 8BIT REGISTER instead
this->send_start();           	//auto increment of registers must be enabled at slave did not work allways :-(
this->send_byte(sadr);  	//if uncertan use read_u8_reg();
if (!sakn()) {        		//never try this again!!!!!!!!
        this->send_stop();
        rom_error("Auf ein i2c Anfrage hat kein slave geantwortet");
        }
this->send_byte(reg|0x80);
if (!sakn()) {
        this->send_stop();
        rom_error("Auf ein i2c register Anfrage hat kein slave geantwortet");
        }
this->send_repeated_start();
this->send_byte(sadr | 0x01);    // LSB wird gestzt
if (!sakn()) {
        this->send_stop();
        rom_error("Auf ein i2c lesr Anfrage hat kein slave geantwortet");
        }
tmpl = this->read_byte();
this->send_mak();
tmph = this->read_byte();
this->send_stop();      */
uint8_t tmpl = read_u8_reg(reg,sadr);	//this works fine
uint8_t tmph = read_u8_reg(reg+1,sadr);	//but is slower
return uint16_t(tmpl + tmph*0x100);
}

int16_t read_s16_reg(uint8_t reg, uint8_t sadr) {return reinterpret_s16(read_u16_reg(reg,sadr));}

void write_register(uint8_t reg, uint8_t sad,uint8_t data) {
send_start();
send_byte(sad & ~0x01);	//lsb gets reset
if (!sakn())    {rom::error("No reply to i2c command!");}
send_byte(reg);
if (!sakn())    {rom::error("No reply to i2c register command!");}
send_byte(data);
if (!sakn())    {rom::error("No reply to i2c write command!");}
send_stop();
rdel(16.0*bittime);	//longer delay, i2c slave should get two bytes time bevor we check what he has done
if (read_u8_reg(reg,sad) != data) {  //reading register to check if slave had written everything properly
        std::cout<<"Sad: "<<uint16_t(sad)<<" | Reg: "<<uint16_t(reg)<<" | Data: "<<uint16_t(data);
	std::cout<<" read_u8_reg(reg,sad) " <<uint16_t(read_u8_reg(reg,sad))<<std::endl;
        rom::error("Write command on i2cbus failed");
        }
writecnt++;
}
}; //-******************************************************************************

//this class alows you to comunicate with an humidity ad temperature sensor on "raspberry pi sense hat"
class hts221 {//-***Capacitive digital sensor for relative humidity and temperat$
private:
i2c_master& master;
//Table 15. Register address map
//Name          Type            Register address (hex)  Default (hex)
static const uint8_t reg_WHO_AM_I =     0x0F;           //BC
static const uint8_t reg_AV_CONF =      0x10;           //1B
//      Reserved                        11-1C           Do not modify
static const uint8_t reg_CTRL_REG1 =    0x20;           //0
static const uint8_t reg_CTRL_REG2 =    0x21;           //0
static const uint8_t reg_CTRL_REG3 =    0x22;           //0
//      Reserved                        23-26           Do not modify
static const uint8_t reg_STATUS_REG =   0x27;           //0
static const uint8_t reg_HUMIDITY_OUT_L=0x28;           //Output
static const uint8_t reg_HUMIDITY_OUT_H=0x29;           //Output
static const uint8_t reg_TEMP_OUT_L =   0x2A;           //Output
static const uint8_t reg_TEMP_OUT_H =   0x2B;           //Output
//      Reserved                        2C-3F           Do not modify
static const uint8_t sad_write =        0xBE;
//static const uint8_t sad_read =       unknown;
static const uint8_t H0_rH_x2 =         0x30;
static const uint8_t H1_rH_x2 =         0x31;
static const uint8_t T0_degC_x8 =       0x32;
static const uint8_t T1_degC_x8 =       0x33;
static const uint8_t T1_T0_msb =        0x35;
static const uint8_t H0_T0_OUT =        0x36;
static const uint8_t H1_T0_OUT =        0x3A;
static const uint8_t T0_OUT =   0x3C;
static const uint8_t T1_OUT =   0x3E;

////humidity calibration data and temperature calibration data
double h0_rh(void)      {return (master.read_u8_reg(H0_rH_x2,sad_write)/2.0);}
double h1_rh(void)      {return (master.read_u8_reg(H1_rH_x2,sad_write)/2.0);}
int16_t h0_t0_out(void) {return master.read_s16_reg(H0_T0_OUT,sad_write);}
int16_t h1_t0_out(void) {return master.read_s16_reg(H1_T0_OUT,sad_write);}
int16_t h_out(void)     {return master.read_s16_reg(reg_HUMIDITY_OUT_L,sad_write);}
int16_t t_out(void)     {return master.read_s16_reg(reg_TEMP_OUT_L,sad_write);}

double t0_cels(void) {
uint8_t msb = master.read_u8_reg(T1_T0_msb,sad_write);
uint8_t low = master.read_u8_reg(T0_degC_x8,sad_write);
msb &= 0x03;
return (double(low) + double(msb)*256.0)/ 8.0;
}

double t1_cels(void) {
uint8_t msb = master.read_u8_reg(T1_T0_msb,sad_write);
uint8_t low = master.read_u8_reg(T1_degC_x8,sad_write);
msb &= 0x0C;
msb >>= 2;
return (double(low) + double(msb)*256.0)/8.0;
}

int16_t t0_out(void)    {return master.read_s16_reg(T0_OUT,sad_write);}
int16_t t1_out(void)    {return master.read_s16_reg(T1_OUT,sad_write);}

public:
hts221(const hts221& in) = delete;		//no copy!
hts221 operator=(const hts221& in) = delete; 	//no copy!

//raspberry pi; i2c bus speed of 100khz is the most safe option (you can try 400 khz as well)
hts221(i2c_master& in):master(in) {
if (master.check_slave_adress(sad_write)==0){
	rom::error("Sensor hts221 on i2c-bus does nnot respond!");
	}
master.write_register(reg_CTRL_REG1,sad_write,(master.read_u8_reg(reg_CTRL_REG1,sad_write)|0x81));//Sensor einschalten und updaterate auf 1HZ einstellen
master.write_register(reg_AV_CONF,sad_write,(master.read_u8_reg(reg_AV_CONF,sad_write) & 0xC0));//Alle Bits null setzten ausser die zwei reservierten
master.write_register(reg_AV_CONF,sad_write,(master.read_u8_reg(reg_AV_CONF,sad_write) | 0x24));//fuer Temperatur werden 32 werte gemittelt fuer Feuchtigkeit 64 werte
std::vector<int16_t> trash_can;
for (uint16_t i =0;i<100;i++) { 	//put first 100 Values in trash
        trash_can.push_back(h_out());   //That's required because this sensor delivers some wrong
        trash_can.push_back(t_out());   //readings after startup
        master.borrow_delayfunctor()(0.005);
        }
trash_can.resize(0);            	//empty trashcan obsolete
}

double humidity(void) {	//this function will return the relative humidity witch your sensor measures
static double k=0.0;	//humidity measurments with sense-hat are not accurate because the sense-hat gets
static double n=0.0;	//some heat from the rasperry pi;
if (k==0.0) {k = (h1_rh()-h0_rh()) / (h1_t0_out()-h0_t0_out());}  //slope
if (n==0.0) {n = h0_rh()-(h0_t0_out()*k);                      }  //offset
return (k*h_out())+n;                                             //"y = k*x + n"
}

double temp(void) {	//this function will return the temperature of your hts221 sensor
static double k=0.0;	//temerature measurements on the sens-hat are not accurate as well
static double n=0.0;	//in my case the error is aproxymately 3 degrees centigrate but i use
if (k==0.0) {k = (t1_cels()-t0_cels()) / (t1_out()-t0_out());}	//an rasperry-pi-zero-w witch heats the least
if (n==0.0) {n = t0_cels() - (t0_out()*k);                   }	//of all raspberry models
return (k*t_out())+n;
}
};//-******************************************************************************

//-******************************************************************************
//this class alows you to comunicate with an air-preasure and temperature sensor on "raspberry pi sense hat"
class lps25h { //pressure sensor on i2c bus
private:
i2c_master& master;
static constexpr uint8_t sad_read[2] {0xB9,0xBB};
static constexpr uint8_t sad_write[2] {0xB8,0xBA};
//register adress map
static const uint8_t REF_P_XL   = 0x08;
static const uint8_t REF_P_L    = 0x09;
static const uint8_t REF_P_H    = 0x0A;
static const uint8_t WHO_AM_I   = 0x0F;
static const uint8_t RES_CONF   = 0x10;
static const uint8_t CTRL_REG1  = 0x20;
static const uint8_t CTRL_REG2  = 0x21;
static const uint8_t CTRL_REG3  = 0x22;
static const uint8_t CTRL_REG4  = 0x23;
static const uint8_t INT_CFG    = 0x24;
static const uint8_t INT_SOURCE = 0x25;
static const uint8_t STATUS_REG = 0x27;
static const uint8_t PRESS_OUT_XL       = 0x28;
static const uint8_t PRESS_OUT_L        = 0x29;
static const uint8_t PRESS_OUT_H        = 0x2A;
static const uint8_t TEMP_OUT_L = 0x2B;
static const uint8_t TEMP_OUT_H = 0x2C;
static const uint8_t FIFO_CTRL  = 0x2E;
static const uint8_t FIFO_STATUS        = 0x2F;
static const uint8_t THS_P_L            = 0x30;
static const uint8_t THS_P_H            = 0x31;
static const uint8_t RPDS_L             = 0x39;
static const uint8_t RPDS_H             = 0x3A;

public:

double pressure(void) {         //[mbar]
uint32_t tmp = 0;
tmp = master.read_s16_reg(PRESS_OUT_L,sad_write[0])<<8; //this works wen pressure is positive
tmp &= 0xFFFFFF00;
tmp |= master.read_u8_reg(PRESS_OUT_XL,sad_write[0]);
return double(tmp/4096.0);
}

double altitude(void) {         //m above see level
double tmp = pressure();        //using stange formula to calculate altitude
tmp /= 1013.25;                 //from air pressure
tmp = pow(tmp,0.190284);
tmp = 1.0 -tmp;
tmp *= 145366.45;
return tmp *= rom::_M_P_FEET;
}

double temp(void) {
double tmp=0;
tmp = double(master.read_s16_reg(TEMP_OUT_L,sad_write[0]));     //the value in Rgister is likely negative
tmp /= 480.0;                                                   //cpp should convert it to double
return tmp += 42.5;
}
lps25h(const lps25h& in) = delete;		//no copy!
lps25h operator=(const lps25h& in) = delete; 	//no copy!

lps25h(i2c_master& ma_in):master(ma_in) {
if (master.check_slave_adress(sad_write[0])==0)	{rom::error("Sensor lps25h on i2c-bus does not respond!");}
if (master.read_u8_reg(WHO_AM_I,sad_write[0]) != 0xBD)  {rom::error("There is a major problem with an preasure sensor on i2c-bus");}
master.write_register(RES_CONF,sad_write[0],(master.read_u8_reg(RES_CONF,sad_write[0])|0x0F));// 512 internal averages for pressure 64 averages for temperature
master.write_register(CTRL_REG1,sad_write[0],rom::ob(1,0,1,0,0,0,0,0));// 7HZ update
master.write_register(CTRL_REG2,sad_write[0],rom::ob(0,0,0,0,0,0,0,0));// No FIFo
std::vector<double> trash_can;
for (uint16_t i =0;i<100;i++) { 		//put first 100 Values in trash
        trash_can.push_back(temp());    	//That's required because this sensor delivers some wrong
        trash_can.push_back(pressure());        //readings after startup
        master.borrow_delayfunctor()(0.001);
        }
trash_can.resize(0);            //empty trashcan
}
};//-******************************************************************************


//-******************************************************************************
class max1139eee { //10 Bit A-D converter, 12 Channel, i2c bus
private:
i2c_master& master;

static constexpr uint8_t sad_write = 106;//slave adress
//setup_byte:
//internal Vref(2.048volt);  external clock(faster); unipolar; no reset
static constexpr uint8_t setup_byte = rom::cob(1,1,0,1,1,0,1,0);
uint8_t config_byte;
static constexpr double Vref = 2.048;
uint8_t sad_read(void) {return (sad_write | 0x01);}
uint8_t last_channel;

public:
max1139eee(const max1139eee& in) = delete;	//no copy!
max1139eee operator=(const max1139eee& in) = delete; 	//no copy!

//you can connect your Max1139 to an gpio, but
//pins for sda and scl must have an pull up resistor to 3.3V
max1139eee(i2c_master& mi):master(mi),config_byte{rom::ob(0,1,1,0,0,0,0,1)},last_channel{255}  {
//config_byte:
//bit 7 0==config byte
//bit 5 and 6:convert only selected channel
//bit 1-4 Cannel
//bit 0 1==single ended
master.send_start();
master.send_byte(sad_write);
if (!master.sakn())    {rom::error("i2c slave with adress "+std::to_string(sad_write)+
					" does not reply.");}
master.send_byte(setup_byte);
if (!master.sakn())    {rom::error("No reply to i2c write command to ad converter!");}
master.send_stop();
}

double read(uint8_t channel) {
if (channel != last_channel) {
        last_channel = channel;
        channel = (channel & 0x0F) << 1;  //take only 4 bits
        config_byte = rom::ob(0,1,1,0,0,0,0,1) | (channel);
        master.send_start();
        master.send_byte(sad_write);
        if (!master.sakn())    {rom::error("error on max1139-i2c bus");}
        master.send_byte(config_byte);
        if (!master.sakn())    {rom::error("error on max1139-i2c bus");}
        master.send_stop();
        }
master.send_start();
master.send_byte(sad_read());
if (!master.sakn())    {rom::error("error on max1139-i2c bus");}
uint8_t high{master.read_byte()};
master.send_mak();
uint8_t low{master.read_byte()};
master.send_stop();
uint16_t result =  ((uint16_t(0x03 & high) << 8) | low);
return  (Vref * result / 1024.0);
}
}; //-******************************************************************************

//-******************************************************************************
class mcp4728 {    //12 bit D-A converter, 4channel, i2c bus, 5mA max outp-Current
private:
i2c_master& master;
static constexpr uint8_t sad_write = rom::cob(1,1,0,0,0,0,0,0);
uint8_t sad_read(void) {return (sad_write | rom::cob(0,0,0,0,0,0,0,1));}//adress is programmable, see Datasheet
static constexpr uint8_t single_write = rom::cob(0,1,0,1,1,0,0,0);//command for single output write
static constexpr uint8_t udac = rom::cob(0,0,0,0,0,0,0,0);//udac bit is zero for spontaneous update
static constexpr uint8_t vref = rom::cob(1,0,0,0,0,0,0,0);//internal reference 2.048V
static constexpr uint8_t pd = rom::cob(0,0,0,0,0,0,0,0);//power dow selection "normal"
static constexpr uint8_t gx = rom::cob(0,0,0,0,0,0,0,0);//gain selection "1x"
static constexpr double Vref = 2.048;                   //internal reference voltage
static constexpr double values = 4096;                  //12 bit d-a converter

void write_int(uint16_t val, uint8_t chan){     //chan: 0=A; 1=B; 2=C; 3=D;
chan &= rom::ob(0,0,0,0,0,0,1,1);               //mask 2 bits for 4 channels
chan <<= 1;                                     //smallest bit should be reserved for udac
uint8_t command_2nd{uint8_t(single_write | chan | udac)};//put second byte together
uint8_t valh=((val & 0x0f00)>>8);               //4 bit //split input value into high and low byte
uint8_t vall=(val & 0x00ff);                    //8 bit //split input value into high and low byte
uint8_t command_3rd{uint8_t(vref | pd | gx | valh)};    //put 3rd byte together 
master.send_start();
master.send_byte(sad_write);    //first byte is slave adress
if (!master.sakn())    {rom::error("error on mcp4728-i2c bus");}
master.send_byte(command_2nd);
if (!master.sakn())    {rom::error("error on mcp4728-i2c bus");}
master.send_byte(command_3rd);
if (!master.sakn())    {rom::error("error on mcp4728-i2c bus");}
master.send_byte(vall);         //4th byte is lower 8 bits of value
if (!master.sakn())    {rom::error("error on mcp4728-i2c bus");}
master.send_stop();     //rom_sheepdelay(16.0*master.bittime);
}


public:
mcp4728(const mcp4728& in) = delete;
mcp4728 operator=(const mcp4728& in) = delete; 	//no copy!

mcp4728(i2c_master& mi):master(mi) {}

void write(float voltage,uint8_t chan) {
int32_t v_int = voltage*values/Vref;
v_int = (v_int<0)?0:v_int;              //no input voltage is allowed to be smaller than 0
v_int = (v_int>=4096)?4095:v_int;       //no input voltage is allowed to be larger than 4095
write_int(v_int,chan);
}

void write_all(float vltg) {for (uint8_t i{0};i<4;i++) {write(vltg,i);}}
};//-******************************************************************************

} //namespace rom

//usage example
void rom_wiringpin_t(void){

std::cout<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the io library: "<< std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;

rom::i2c_master sense_m(8,9,100000);	//create an i2c-master for comunication with sense hat
rom::i2c_master custom_m(24,25,400000);	//create an i2c_master for comuntication with other devices
				//on custom i2c bus

rom::hts221 humi{sense_m};	//
std::cout << "Temperature of humidity ensor is: \t" << humi.temp() <<std::endl;
std::cout << "Humidity is:                      \t" << humi.humidity() <<std::endl;

rom::lps25h press{sense_m};
std::cout << "Temperature of pressure sensor is:\t" << press.temp() <<std::endl;
std::cout << "Pressure is:                      \t" << press.pressure() <<std::endl;
std::cout << "Altitude is:                      \t" << press.altitude() <<std::endl;

rom::max1139eee ad{custom_m};
rom::mcp4728 	da{custom_m};
for (uint8_t i{0};i<3;++i){
	std::cout << "Voltage is:                       \t" << ad.read(0) << std::endl;
	}
da.write_all(1.8);

}

#endif
