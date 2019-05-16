#ifndef rom_matrix
#define rom_matrix

#include "rom_error.h"
#include "rom_stream.h"

namespace rom {

//Helper functions:
int8_t parity_of_permutation(size_t inp) {return (inp%2)?-1:1;}
int8_t parity_of_permutation(size_t a,size_t b) {return ((a+b)%2)?-1:1;}


//test if an 2d vector is square
template<class blt> //should work for every built in type
uint8_t is_square(const std::vector<std::vector<blt>>& in) {
for (auto& elem:in) {if (elem.size()!=in.size()) {return 0;}}
return 1;
}

//test if an 2d vector is rectangular
template<class blt> //should work for every built in type
uint8_t is_rectangular(const std::vector<std::vector<blt>>& in) {
for (auto& elem:in) {if (elem.size()!=in.back().size()) {return 0;}}
return 1;
}

template<class blt> //should work for every built in type
//this should create a vector of columns if input is a vector of rows and vise-versa
std::vector<std::vector<blt>> row_column_mirror(const std::vector<std::vector<blt>>& in) {
if (is_rectangular(in)==0) {rom::error("expected a rectangular 2d vector");}
std::vector<blt> one_row(in.size(),blt{0});			//temporaray storage
std::vector<std::vector<blt>> ret(in.back().size(),one_row); 	//size should be mirror of in
for (size_t a{0};a!=in.size();++a) {
	for (size_t b{0};b!=in.at(a).size();++b) {
		ret.at(b).at(a)=in.at(a).at(b); //mirror every value
		}
	}
return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//class Matrix should represent an two dimensional matrix
template<class flt=double>	//some type of floating point number
class Matrix{
private:
std::vector<flt> m_d;  //the two dimensions should be flatened to an one dimensional std::vector
size_t m_rows;
size_t m_columns;


Matrix minors(void) const {	//create a matrix of minors from the current object
if ( !is_sqare() )  {error("Sorry cannot create a non sqare Matrix of Minors ");}
auto ret = (*this); 			//copy current object
for (size_t r{0};r<ret.rows();++r) {  	//recreate a 2d vector of rows
	for (size_t c{0};ret.columns()!=c;c++) { 	//every column
		ret.at(r,c) = (*this).delete_row_and_column(r,c).det();
		}
	}
return ret;
}

Matrix cofactors(void) const {
auto ret = this->minors();
for (size_t r{0};r<ret.rows();++r) {  	//recreate a 2d vector of rows
	for (size_t c{0};ret.columns()!=c;c++) { 	//every column
		ret.at(r,c) *= parity_of_permutation(r,c);
		}
	}
return ret;
}

Matrix delete_row(size_t r) const {	//create a new Matrix without row number r
std::vector<std::vector<flt>> ret {};
for (size_t row{0}; row!=rows();row++) {if (row!=r) {ret.push_back(this->row(row));}}
return Matrix(ret);
}

Matrix delete_column(size_t c) const {	//create a new Matrix without column number c
auto ret = this->transpose();
ret = ret.delete_row(c);
return ret.transpose();
}

public:
Matrix delete_row_and_column(size_t r,size_t c) const{
return this->delete_column(c).delete_row(r);
}

Matrix(const Matrix& in) = default;	//default copy constructor

flt& at(size_t row_in,size_t column_in) {	//member access
return m_d.at(row_in*m_columns + column_in);	//in informatics indices begin at 0!!
}

flt at(size_t row_in, size_t column_in) const {	//const meber access
return m_d.at(row_in*m_columns + column_in);	//indices go from 0 to size-1  !!!!!
}

size_t rows(void) 	const {return m_rows;}
size_t columns(void) 	const {return m_columns;}
size_t size(void) 	const {return m_columns * m_rows;}

std::vector<flt> row(size_t r) const {
if (r >= rows()) {error("Trying to access row out of Matrix");}
std::vector<flt> ret{};
for (size_t c{0};c!=columns();++c) {ret.push_back(at(r,c));}
return ret;
}

std::vector<flt> col(size_t c) const {
if (c >= columns()) {error("Trying to access column out of Matrix");}
std::vector<flt> ret{};
for (size_t r{0};r!=rows();++r) {ret.push_back(at(r,c));}
return ret;
}

Matrix(void):m_d{0.0},m_rows{1},m_columns{1}  {}  	//default size is one, value is 0.0

Matrix(const std::vector<std::vector<flt>>& inp):Matrix{} {//construction Matrix from 2d vector of rows
if (is_rectangular(inp)==0) {rom::error("trying to construct a matrix from not rectangular input");}
m_rows = inp.size();		//input has to be a vector of rows
m_columns = inp.at(0).size();
m_d.resize(size());
for (size_t r{0};r!=rows();r++) { 		//every row
	for (size_t c{0};columns()!=c;c++) { 	//every column
		this->at(r,c)=inp.at(r).at(c);
		}
	}
}

operator std::vector<std::vector<flt>>() const  {	//construct 2d vector from Matrix
std::vector<std::vector<flt>> row_v;
for (size_t r{0};r<rows();++r){  //recreate a 2d vector of rows
	row_v.push_back(std::vector<flt>());
	for (size_t c{0};c<columns();++c){
		row_v.back().push_back(at(r,c));
		}
	}
return row_v;
}

Matrix(size_t row,size_t col,flt val=0.0):m_d{0.0},m_rows{row},m_columns{col} {//create a zero Matrix with right size
m_d.resize(size());
for (auto& ref:m_d) {ref=val;}
}

Matrix identity(void) const {	//create an identity-matrix with the size of the current object
if ( !is_sqare() )  {error("Sorry cannot create a non sqare identity-Matrix ");}
auto ret = (*this); 			//copy current object
for (size_t r{0};r<ret.rows();++r) {  	//recreate a 2d vector of rows
	for (size_t c{0};ret.columns()!=c;c++) { 	//every column
		ret.at(r,c) = (r==c)?(1.0):(0.0);
		}
	}
return ret;
}

flt det(void) const {//calculate the determinant of matrix with Laplace's formula
if (rows()!=columns()) {error("Sorry cannot calculate determinant of a non square Matrix");}
flt ret{0.0};
if (rows() < 1){error("Determinant is not defined if size of matrix is smaller than 1x1");}
else if (rows()==2) {ret = at(0,0)*at(1,1)-at(0,1)*at(1,0);}//perform simple Sarrus(2x2) rule
else if (rows()==1) {ret = at(0,0);}//trivial case
else /*(rows()> 2)*/ {
	for (size_t r{0};r!=rows();r++) {
		flt mul{delete_row_and_column(r,0).det()};//recursion is slow but correct
		mul *= at(r,0);
 		mul *= parity_of_permutation(r+0);
		ret += mul;
		}
	}
return ret;
}

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
for (size_t i{0};i<r.size();++i) {
	if (rom::_almost_equal(r.m_d.at(i),this->m_d.at(i)) == 0)   {return 0;}
	}
return 1;
}

uint8_t is_sqare(void) const {return (m_rows == m_columns)?1:0;}

uint8_t is_symmetric(void) const {
auto t = (*this).transpose();
return (t==(*this))?1:0;	//if the matrix is like its transpose it is symmetric
}

Matrix transpose(void) const{
std::vector<std::vector<flt>> row_vec(*this);
return Matrix<flt>(row_column_mirror(row_vec));
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
auto ret = *this;
n--;
for (size_t i{0};i<n;++i) {ret*=(*this);}
return ret;
}


Matrix inverse(void) const {
flt det = this->det();
auto ret = this->cofactors().transpose() * (flt(1.0)/det);
return ret;
}

};//class matrix

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
auto ret = l;
for (size_t r{0};r<ret.rows();r++) {for (size_t c{0};c<ret.columns();c++) {ret.at(r,c) *= mul;}}
return ret;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


}//namespace rom


void rom_matrix_t(void){

std::cout<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the matrix library: "<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;


std::cout <<"Default Matrix"<< rom::Matrix<float>{} << std::endl;
rom::Matrix<double> a{{		{5.0,9.0,3.0,7.0,1.0,5.0,7.0,1.0},
				{6.0,9.0,3.0,7.0,0.3,5.0,0.3,3.0},
				{1.0,4.0,8.0,1.0,4.0,7.0,4.0,6.0},
				{6.0,3.0,9.0,6.0,2.0,7.0,6.0,8.0},
				{9.0,2.0,5.0,0.1,4.0,7.0,0.1,9.0},
				{1.0,4.0,8.0,2.0,5.0,9.0,2.0,3.0},
				{5.0,9.0,2.0,6.0,9.0,2.0,5.0,4.0},
				{9.0,2.0,6.0,2.0,5.0,9.0,7.0,6.0}	}};
std::cout <<"Matrix"<< a << std::endl;//the right answear is: 213546
std::cout <<"Determinant "<< a.det() <<std::endl;
std::cout <<"Determinant of the transpose is "<< a.transpose().det()<<  std::endl;

rom::Matrix<float>  b{{	{4.0,3.0},
			{7.0,5.0}	}};
std::cout <<"Matrix"<< b << std::endl << "Determinant: " << b.det();
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

std::cout << std::endl;
std::cout << std::endl;
std::cout << std::endl;

rom::Matrix<float>  f{{	{ 3, 0, 2},
			{ 2, 0, -2},
			{ 0, 1, 1}	}};
std::cout <<"Matrix"<< f << std::endl;
std::cout <<"Inverse = " <<f.inverse()<< std::endl;
std::cout <<"Multiplication returns  = " <<(f.inverse()*f)<< std::endl;

std::cout << std::endl;
std::cout << std::endl;
std::cout << std::endl;

std::cout <<"Matrix"<< a << std::endl;
auto a_inv = a.inverse();
std::cout <<"Inverse = " <<a_inv<< std::endl;
std::cout <<"Multiplication returns  = " <<(a_inv*a)<< std::endl;
std::cout <<"double multiplication returns  = " <<a*(a_inv*a)<< std::endl;

}


#endif

