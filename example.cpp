//recommendet usage:
//you should be on an raspberry pi running the latest version of raspbian

//you should directly connect your raspberry pi to a "Sense Hat"

//connect to your network and make sure you have internet access

//install g++ "sudo apt-get install build-essential"


//run command "gpio -v" to check if wiringPi is pre-installed

//if not  -->
//install wiringpi: see "http://wiringpi.com/download-and-install/"

//put all files of this repository into one directory and compile it

//Compiling:
//g++ example.cpp -o example -std=c++11 -Wno-psabi -Weffc++ -Wall -pedantic -pthread -lwiringPi -Os

//if compiling goes well you should see an executeable file named example, use command "ls -l" to see
//whats in your directory

//run it by typing command ./example

#include "rom_fft.h"
#include "rom_wiringpin.h"
#include "rom_matrix.h"

int main (void) {
rom_fft_t();
rom_wiringpin_t();
rom_matrix_t();
rom_spacetime_t();


}
