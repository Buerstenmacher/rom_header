//this header file is the c++ reincarnatin of the "MathTheBeautiful" channel on youtube
// "https://www.youtube.com/channel/UCr22xikWUK2yUW4YxOKXclQ"
//part 1 linear algebra
//part 2 linear algebra

#ifndef rom_matrix
#define rom_matrix

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "rom_globals.h"
#include "rom_time.h"
#include "rom_error.h"
#include "rom_stream.h"

namespace rom {

//Helper functions:
int8_t parity_of_permutation(size_t inp) {return (inp%2)?-1:1;}
int8_t parity_of_permutation(size_t a,size_t b) {return ((a+b)%2)?-1:1;}

//test if an 2d vector is square
template<class blt> //should work for every built in type
uint8_t is_square(const std::vector<std::vector<blt>>& in) {
for (auto& elem:in) {if (elem.size()!=in.size()) {return 0;/*false*/}}
return 1;//true
}

//test if an 2d vector is rectangular
template<class blt> //should work for every built in type
uint8_t is_rectangular(const std::vector<std::vector<blt>>& in) {
for (auto& elem:in) {if (elem.size()!=in.back().size()) {return 0;/*false*/}}
return 1;//true
}

//switch 2 values of variables with the same type
template<class assignable>	//every type that has operator = defined
void switch_values(assignable& a, assignable& b) {  //parameter will be NOT - const!!!
static assignable tmp{};
tmp=a;
a=b;
b=tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//class Matrix should represent an two dimensional matrix
template<class flt=double>	//some type of floating point number
class Matrix{
private:
std::vector<flt> m_d;  //the two dimensions should be flatened to an one dimensional std::vector
size_t m_rows;
size_t m_columns;

Matrix delete_row(size_t r) const {	//create a new Matrix without row number r
std::vector<std::vector<flt>> ret {};
for (size_t row{0}; row!=rows();row++) {if (row!=r) {ret.push_back(this->row(row));}}
return Matrix(ret);
}

Matrix delete_column(size_t c) const {	//create a new Matrix without column number c
auto ret{this->transpose()};		//todo: this function is verry slow do to multiple times copying the entire Matrix
ret = ret.delete_row(c);
return ret.transpose();
}

constexpr flt zero_max(void) const {	//the largest value we will accept as zero
return std::abs(1024.0*std::numeric_limits<flt>::min());
}

uint8_t row_is_zero(size_t r) const {			//check if an entire row consists of zero values
auto row{row(r)};
for (auto& value:row) {if (std::abs(value) >= zero_max()) {return 0;}}
return 1;
}

uint8_t col_is_zero(size_t r) const {			//check if an entire column consists of zero values
auto colu{col(r)};
for (auto& value:colu) {if (std::abs(value) >= zero_max()) {return 0;}}
return 1;
}

uint8_t col_is_zero_except_first_row(size_t r) const {	//check if an entire column consists of zero values ingnore first element
auto colu{col(r)};
auto it{colu.begin()};
for (it++/*ignore colu.at(0)*/;it!=colu.end();it++) {if (std::abs(*it) >= zero_max()) {return 0;}}
return 1;
}

Matrix switch_rows(size_t a, size_t b) const {		//return a matrix with rows a and b switched
auto ret{Matrix(*this)};				//copy current object;
if (a==b) {return ret;}
if ((a>= rows()) || (b>=rows()))	{::rom::error("trying to switch rows out of matrix");}
for (size_t c{0};c!=columns();++c) 	{switch_values(ret.at(a,c),ret.at(b,c));}
return ret;
}

Matrix multply_row_by_number(size_t row, flt number) const {	//create a matrix with one number multiplied by number
auto ret{Matrix(*this)};	//copy current object;
if (row>= rows())			{::rom::error("trying to multiply row out of matrix");}
for (size_t c{0};c!=columns();++c) 	{ret.at(row,c)=ret.at(row,c)*number;}
return ret;
}

ptrdiff_t row_with_largest_value_in_first_column(void) const {	//first column is col(0)
auto first_column{col(0)};
for (auto& r:first_column)	{r = std::abs(r);}
auto iter{std::max_element(first_column.begin(),first_column.end())};
return std::distance(first_column.begin(),iter);
}

size_t row_with_first_nonzero_value_in_first_column_after_first_element(void) const {	//first column is col(0)
auto first_column{col(0)};
for (size_t r{1};r!=rows();r++)	{if (std::abs(first_column.at(r))>=zero_max()) {return r;}}
return 0;
}

Matrix add_multiple_of_row_a_to_row_b(flt mul,size_t a, size_t b) const{
auto ret{Matrix(*this)};	//copy current object;
if (a==b) 				{::rom::error("adding a multiple of one row to itself is not a valid operation");}
if ((a>= rows()) || (b>=rows()))	{::rom::error("trying to add up rows out of matrix");}
for (size_t c{0};c!=columns();++c) 	{ret.at(b,c) += ret.at(a,c)*mul;}
return ret;
}

Matrix eliminate_element_in_first_column_of_row_a_by_first_row(size_t a) const {
auto at_a{at(a,0)};
auto at_0{at(0,0)};
auto ret = add_multiple_of_row_a_to_row_b(flt(-1.0*at_a/at_0),0,a);
return ret;
}

public:
Matrix(const Matrix& in) = default;	//default copy constructor
Matrix(void) = delete;  		//no default constructor

flt& at(size_t row_in,size_t column_in) {	//member access
return m_d.at(row_in*m_columns + column_in);	//in informatics indices begin at 0!!
}

flt at(size_t row_in, size_t column_in) const {	//const meber access
return m_d.at(row_in*m_columns + column_in);	//indices go from 0 to size-1  !!!!!
}

size_t rows(void) 	const {return m_rows;}			//number of rows
size_t columns(void) 	const {return m_columns;}		//number of columns
size_t size(void) 	const {return m_columns * m_rows;}	//number of elements in matrix

std::vector<flt> row(size_t r) const {			//get a copy of row number r
if (r >= rows()) {error("Trying to access row outside of matrix");}
std::vector<flt> ret{};
for (size_t c{0};c!=columns();++c) {ret.push_back(at(r,c));}
return std::move(ret);
}

std::vector<flt> col(size_t c) const { 			//get a copy of column number c
if (c >= columns()) {error("Trying to access column outside of matrix");}
std::vector<flt> ret{};
for (size_t r{0};r!=rows();++r) {ret.push_back(at(r,c));}
return std::move(ret);
}

Matrix delete_row_and_column(size_t r,size_t c) const{	//create a new matrix without row r and column c
return this->delete_column(c).delete_row(r);
}

flt determinant(void) const {//calculate the determinant of matrix with gaussian elimination and recursion
if (!is_sqare()) 	{error("Sorry cannot calculate determinant of a non square matrix");}
if (rows() < 1)		{error("Determinant is not defined if size of matrix is smaller than 1x1");}
//std::cout <<"********** ********** ********** ********** ********** ********** ********** ********** ********** **********"<<std::endl;
//std::cout << "Determinant of: "<<(*this)<<" = "<<std::endl;
if (rows() == 1)	{//if size of matrix = 1x1 determinant is its single value
//	std::cout << at(0,0)<<std::endl;
	return at(0,0);
	}
if (col_is_zero(0))	{//if the entire first column is zero the determinant of the entire matrix is zero
//	std::cout << flt{0.0} <<std::endl;
	return flt{0.0};
	}
if (col_is_zero_except_first_row(0)) {//if there is only one value left in column
	auto copy = delete_row_and_column(0,0);
//	std::cout << at(0,0)<<" * "   <<std::endl;
	return (at(0,0) * copy.determinant());
	}
if (row_with_largest_value_in_first_column()!=0)  {//put the largest value in first row
	auto lrow{row_with_largest_value_in_first_column()};
	auto copy = switch_rows(0,lrow);
//	std::cout << "-1.0 * "  <<std::endl;
	return ((-1.0) * copy.determinant());
	}
if (row_with_first_nonzero_value_in_first_column_after_first_element()!=0)  {
	auto ro{row_with_first_nonzero_value_in_first_column_after_first_element()};
	auto copy = eliminate_element_in_first_column_of_row_a_by_first_row(ro);
//	std::cout <<" = "<<std::endl;
	return copy.determinant();
	}
error("failed to calculate determinant of matrix");
return NAN;
}

Matrix minors(void) const {	//create a matrix of minors from the current object
if ( !is_sqare() )  {error("Sorry cannot create a non sqare Matrix of Minors ");}
auto ret{*this}; 			//copy current object to get a matrix with the right size
for (size_t r{0};r<ret.rows();++r) {  			//every row
	for (size_t c{0};ret.columns()!=c;c++) { 	//every column
		ret.at(r,c) = (*this).delete_row_and_column(r,c).determinant(); //store the coresponding determinant value
		}
	}
return std::move(ret);
}

Matrix cofactors(void) const {
auto ret{this->minors()};
for (size_t r{0};r<ret.rows();++r) {  	//recreate a 2d vector of rows
	for (size_t c{0};ret.columns()!=c;c++) { 	//every column
		ret.at(r,c) *= parity_of_permutation(r,c);	//switch the sign if permutation is (even? odd?)
		}
	}
return std::move(ret);
}

//constructor: you have to provide the size of rows and columns
Matrix(size_t rows,size_t columns, flt values=0.0):m_d((rows*columns),values),m_rows{rows},m_columns{columns} {}

Matrix(const std::vector<std::vector<flt>>& inp):Matrix(inp.size(),inp.front().size()) {//construction Matrix from 2d vector of rows
if (is_rectangular(inp)==0) {rom::error("trying to construct a matrix from not rectangular input");}
m_rows = inp.size();		//input has to be a vector of rows
m_columns = inp.at(0).size();
m_d.resize(size());		//reserve the memory for all values
for (size_t r{0};r!=rows();r++) { 		//every row
	for (size_t c{0};columns()!=c;c++) { 	//every column
		this->at(r,c)=inp.at(r).at(c);	//set all values
		}
	}
}//that's it

operator std::vector<std::vector<flt>>() const  {	//construct 2d vector from Matrix
std::vector<std::vector<flt>> row_v{};
for (size_t r{0};r<rows();++r){  //recreate a 2d vector of rows
	row_v.push_back(std::vector<flt>());
	for (size_t c{0};c<columns();++c){
		row_v.back().push_back(at(r,c));
		}
	}
return std::move(row_v);
}

Matrix identity(void) const {	//create an identity-matrix with the size of the current object
if ( !is_sqare() )  {error("Sorry cannot create a non sqare identity-Matrix ");}
auto ret {*this}; 			//copy current object
for (size_t r{0};r<ret.rows();++r) {  	//recreate a 2d vector of rows
	for (size_t c{0};ret.columns()!=c;++c) { 	//every column
		ret.at(r,c) = (r==c)?(1.0):(0.0);
		}
	}
return std::move(ret);
}

/*//do not use this function for matrices that are bigger than 12x12 elements	//complexity:  O(n!)
flt determinant_old(void) const {//calculate the determinant of matrix with Laplace's formula
if (!is_sqare()) {error("Sorry cannot calculate determinant of a non square Matrix");}
flt ret{0.0};
if (rows() < 1){error("Determinant is not defined if size of matrix is smaller than 1x1");}
else if (rows()==2) {ret = at(0,0)*at(1,1)-at(0,1)*at(1,0);}//perform simple Sarrus(2x2) rule
else if (rows()==1) {ret = at(0,0);}//trivial case; determinant is it's only value of 1x1 matrix
else /~(rows()> 2) ~/ {	//we use the first column for recursive calculation
	for (size_t r{0};r!=rows();r++) {
		flt mul{delete_row_and_column(r,0).determinant()};//recursion is slow but correct
		mul *= at(r,0);
 		mul *= parity_of_permutation(r+0);
		ret += mul;
		}
	}
return ret;
}*/

operator std::string() const {//make it easy to output internal data of Matrix class
std::ostringstream o;
for (size_t r{0};r<rows();++r) {
	o << std::endl << "{";
	for (size_t c{0};c<columns();c++) {o << " (" << m_d.at(c+r*columns())<<")";}
	o  << " }";
	}
return o.str();
}

uint8_t operator == (const Matrix& r) const {
if ((*this).size()!=r.size())	{return 0;}
for (size_t i{0};i<r.size();++i) {	if (rom::_almost_equal(r.m_d.at(i),this->m_d.at(i)) == 0)   {return 0;}}
return 1;
}

uint8_t is_sqare(void) const 	{return (m_rows == m_columns)?1:0;}

uint8_t is_symmetric(void) const{
auto t {transpose()};
return (t==(*this))?1:0;	//if the matrix is like its transpose it is symmetric
}

Matrix transpose(void) const{
auto ret{Matrix(columns(),rows())}; //create a Matrix with mirrored size
for (size_t r{0};r!=rows();++r)	{for (size_t c{0};c!=columns();++c)	{ret.at(c,r) = (*this).at(r,c);}}	//copy-mirror all values
return ret;
}

Matrix& operator*=(const Matrix& rhs) {	//matrix multiplication
(*this) = (*this)*rhs;
return (*this);
}

Matrix& operator*=(flt rhs) {		//scalar multiplication
(*this) = (*this)*rhs;
return (*this);
}

Matrix to_the_power_of(size_t n) const {
if(n<1) {rom::error("Matrix exponention requires exponent to be larger than 0");}
auto ret {*this};
n--;
for (size_t i{0};i<n;++i) {ret*=(*this);}
return ret;
}

Matrix inverse(void) const {return  this->cofactors().transpose() * (flt(1.0)/this->determinant());}

};//class matrix
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


template<class flt=double>
std::ostream& operator << (std::ostream& os, const Matrix<flt>& v) {
os << std::string(v);
return os;
}

template<class flt=double>
Matrix<flt> operator*(const Matrix<flt>& l,const Matrix<flt>& r) {//Matrix multiplication
if (l.columns()!=r.rows()) {::rom::error("Matrix multiplication is not defined");}
Matrix<flt> ret(l.rows(),r.columns(),flt(0.0));
for (size_t r_row{0};r_row<ret.rows()   ;r_row++){
	for (size_t r_col{0};r_col<ret.columns();r_col++){
		for (size_t item{0} ;item <r.rows()	;item++	){
			ret.at(r_row,r_col)+=l.at(r_row,item)*r.at(item,r_col);
			}
		}
	}
return ret;
}

template<class flt=double>
Matrix<flt> operator*(const Matrix<flt>& l,flt mul) {//Scalar multiplication
auto ret{l};
for (size_t r{0};r<ret.rows();r++) {for (size_t c{0};c<ret.columns();c++) {ret.at(r,c) *= mul;}}
return ret;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

}//namespace rom


void rom_matrix_t(void){
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the matrix library: "<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;


//std::cout <<"Default Matrix"<< rom::Matrix<float>{} << std::endl;
rom::Matrix<double> a{{		{5.0,9.0,3.0,7.0,1.0,5.0,7.0,1.0,3.0},
				{6.0,9.0,3.0,7.0,0.3,5.0,0.3,3.0,9.0},
				{1.0,4.0,8.0,1.0,4.0,7.0,4.0,6.0,5.0},
				{6.0,3.0,9.0,6.0,2.0,7.0,6.0,8.0,1.0},
				{9.0,2.0,5.0,0.1,4.0,7.0,0.1,9.0,6.0},
				{1.0,4.0,8.0,2.0,5.0,9.0,2.0,3.0,1.0},
				{5.0,9.0,2.0,6.0,9.0,2.0,5.0,4.0,5.5},
				{0.3,2.5,9.0,5.0,1.0,6.0,0.1,7.0,3.0},
				{9.0,2.0,6.0,2.0,5.0,9.0,7.0,6.0,0.3}	}};//Determinant: 3944525.700
std::cout <<"Matrix"<< a << std::endl;
std::cout << std::fixed << std::setprecision(3);
std::cout <<"Determinant "<< a.determinant() <<std::endl << std::endl;

/*std::cout <<"Determinant of the transpose is "<< a.transpose().determinant()<<  std::endl;
rom::Matrix<float>  b{{	{4.0,3.0},
			{7.0,5.0}	}};
std::cout <<"Matrix"<< b << std::endl << "Determinant: " << b.determinant();
std::cout << std::endl;

rom::Matrix<float>  l{{	{1,0,3,8},
			{0,1,7,2}	}};
rom::Matrix<float>  r{{	{ 3, 8,36},
			{ 7, 2,34},
			{-1, 0,-4},
			{ 0,-1,-3}	}};
auto result = l*r;
std::cout <<"Matrix"<< l <<" * "<< std::endl;
std::cout <<"Matrix"<< r <<" = "<< std::endl;
std::cout <<"Matrix"<< result <<"  "<< std::endl;
rom::Matrix<float>  e{{	{ 1, 0, 0, 0},
			{ 1, 1, 0, 0},
			{ 1, 1, 1, 0},
			{ 1, 1, 1, 1}	}};
uint16_t exp = 10;
std::cout <<"Matrix"<< e <<" ^ ";
std::cout <<"Integer "<< exp <<" = "<< std::endl;
std::cout <<"Matrix"<< e.to_the_power_of(exp) <<"  "<< std::endl;


rom::Matrix<float>  f{{	{ 3, 0, 2},
			{ 2, 0, -2},
			{ 0, 1, 1}	}};
std::cout <<"Matrix"<< f << std::endl;
std::cout <<"Inverse = " <<f.inverse()<< std::endl;
std::cout <<"Multiplication returns  = " <<(f.inverse()*f)<< std::endl;
*/

auto start{rom::mashinetime()};
auto a_inv = a.inverse();
auto a_inv_a = a_inv*a;
auto a_a_inv_a = a*(a_inv*a);
auto end{rom::mashinetime()};

std::cout <<"Matrix a:"<< a << std::endl<<std::endl;
std::cout <<"Inverse of Matrix a (ai): " <<a_inv<< std::endl<<std::endl;
std::cout <<"Multiplication returns of marix a and its inverse (a*ai) should be the identyty Matrix= " <<a_inv_a<< std::endl<<std::endl;
std::cout <<"double multiplication (a*ai*a = a) returns  = " <<a_a_inv_a<< std::endl<<std::endl;
std::cout <<"This calculation took "<< (end-start) << " seconds. " << std::endl<<std::endl;



}


#endif

