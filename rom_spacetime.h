#ifndef rom_spacetime
#define rom_spacetime

#include <vector>
#include <array>
#include "rom_error.h"

namespace rom {

////////////////////////////////////////////////////////////////////////////////////////////////////
//class Vector should represent an euclidean vector in n-dimensional space
template<typename flt=double,uint16_t dim = 3>   //dim is the number of dimensions
class Vector {
private:
std::array<flt,dim> m_xyz;

public:
Vector(const std::array<flt,dim>& in):m_xyz{in} {}//construct from std::array<flt>

flt abs() const {		//get the magnitude of vector
flt ret{0.0};
for (auto& d:m_xyz) {ret += d*d;}
return sqrt(ret);
}

flt& at(uint16_t dim_in) {	//get reference to the n'th dimension
if (dim_in >= m_xyz.size()) {
	rom::error("The program is trying to get the "+std::to_string(dim_in)+"'th element of an "+
		std::to_string(m_xyz.size())+ " dimensional euclidean vector");
	}
return m_xyz.at(dim_in);
}

flt get(uint16_t dim_in) const {	//get copy of the  n'th dimension
if (dim_in >= m_xyz.size()) {
	rom::error("The program is trying to get the "+std::to_string(dim_in)+"'th element of an "+
		std::to_string(m_xyz.size())+ " dimensional euclidean vector");
	}
return m_xyz.at(dim_in);
}

Vector(const Vector& in):m_xyz{in.m_xyz} {}	//copy

Vector& operator=(const Vector& in) {	//copy construct
m_xyz = in.m_xyz;
return *this;
}

Vector(void):m_xyz{} {for (auto& d:m_xyz) {d = flt(0.0);}}	//defaut construct to 0.0 length

Vector operator*(flt in)const {	//multiply by scalar
Vector ret{*this};
for (auto& d:ret.m_xyz) {d *= in;}
return ret;
}

Vector operator/(flt in)const {	//division by scalar
if (in==0.0) {rom::error("Division by zero in Vector class");}
return (*this) * (flt(1.0) / in);
}

Vector operator+(const Vector& in)const {
Vector ret{};
for (uint16_t i{0};i<m_xyz.size();++i) {ret.at(i) = in.get(i)+(*this).get(i);}
return ret;
}

Vector operator-(const Vector& in)const {return (*this) + in*flt(-1.0);}

Vector& operator*=(flt in) 		{return (*this)=(*this)*in;}
Vector& operator/=(flt in) 		{return (*this)=(*this)/in;}
Vector& operator+=(const Vector& in) 	{return (*this)=(*this)+in;}
Vector& operator-=(const Vector& in) 	{return (*this)=(*this)-in;}

uint8_t operator==(const Vector& in) const{		//try to avoid comparing rom::Vector and floating-point
Vector large = (this->abs()>in.abs())?(*this):(in);	//variables for equality; but if you need it here is the
return ((large.abs()) > ((in-(*this).abs())/_max_acceptable_error<flt>()))?1:0;	//operator
}

uint8_t operator!=(const Vector& in) const {return !(*this == in);}

Vector unitvector() const           {	//construct a Vector with length of 1.0 and
auto absi = this->abs();		//same direction
if (0.0 == absi)        {rom::error("Error at creating an unitvector.");}
return ((*this)/absi);
}

static Vector e_x(uint16_t d_in) { 	//return the unit vector of dimension 0 to (n-1)
Vector ret{};				//default constructor should return us an all zero vector;
ret.at(d_in) = flt(1.0);
return ret;
}

//---------ToDo find a Formula for n dimensional spaces----------------
//https://de.wikipedia.org/wiki/Kreuzprodukt
//this should calculate the crossproduct of n=3 dimensional vectors
//the cross product needs (n-1) vectors (where n is the dimensionality) as input,
static Vector crossp(const std::array<typename rom::Vector<flt,dim>,dim-1>& arr_in) {
static_assert(dim == 3,"Sorry, we are unable to calculate the crossprodukt if the dimension is not 3.");
auto a = arr_in.at(1);	//temporary variables save us some typing
auto in =arr_in.at(0);
return Vector(	std::array<flt,3>{	(in.at(1)*a.at(2) - in.at(2)*a.at(1)),
					(in.at(2)*a.at(0) - in.at(0)*a.at(2)),
					(in.at(0)*a.at(1) - in.at(1)*a.at(0))	}	);
}

static Vector crossp(const rom::Vector<flt,dim>& a,const rom::Vector<flt,dim>& b) {//overlord
return crossp(std::array<flt,2>{a,b});
}

static flt scalarp(const Vector& a,const Vector& in) {//scalarproduct for two n dimensional vectors
flt res{0.0};
for (uint16_t i{0};i<dim;++i) {res += in.get(i)*a.get(i);}
return res;
}

static flt alpha(const Vector& a,const Vector& b) {//calculate the angle between two vectors 
auto cosalpha = scalarp(a.unitvector(),b.unitvector());
return acos(cosalpha);
}

operator std::string() const {//make it easy to output internal data of vector class
std::ostringstream o;
o << std::to_string(dim) << " dimensional vector{";
for (auto &ii : m_xyz){o << " (" << ii<<")";}
o << " }";
return o.str();
}

};//class Vector

}//namespace rom


////////////////////////////////////////////////
//--------EXAMPLE USE OF rom_spacetime.h -------
////////////////////////////////////////////////

void rom_spacetime_t(void){

std::cout<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the vector library: "<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;

rom::Vector<double, 3> vec1{std::array<double,3>{1.0,2.0,3.0}};
rom::Vector<double, 3> vec2{std::array<double,3>{0.0,7.5,0.0}};
rom::Vector<double, 3> vec3= vec1+vec2;
std::cout << std::string(vec1) << "  +  "<<std::string(vec2 ) <<" = "<<std::endl;
std::cout << std::string(vec3)<<std::endl;

rom::Vector<double, 3> vec5{std::array<double,3>{1.0,0.0,0.0}};
rom::Vector<double, 3> vec4{std::array<double,3>{0.0,1.0,0.0}};
auto result = rom::Vector<double,3>::crossp(std::array<rom::Vector<double,3>,2>{vec4,vec5});

std::cout <<"Crossproduct of: "<< std::string(vec4)<< "   " << std::endl;
std::cout <<"and              "<< std::string(vec5)<< " = " << std::endl;
std::cout << std::string(result)<<std::endl;

std::cout << rom::Vector<double,3>::alpha(vec5,vec4) << " rad" << std::endl;
std::cout << rom::Vector<double,3>::alpha(vec4,result) << " rad" << std::endl;
std::cout << rom::Vector<double,3>::alpha(result,vec5) << " rad" << std::endl;


}

#endif
