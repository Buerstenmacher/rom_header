#ifndef rom_matrix
#define rom_matrix

#include "rom_error.h"

namespace rom {

//Helper Functions:
int8_t parity_of_permutation(size_t inp) {return (inp%2)?-1:1;}

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
if (is_rectangular(in)==0) {rom::error("expected an rectangular 2d vector");}
std::vector<blt> one_row(in.size(),blt{0});
std::vector<std::vector<blt>> ret(in.back().size(),one_row); //size should be mirror of in
for (size_t a{0};a!=in.size();++a) {
	for (size_t b{0};b!=in.at(a).size();++b) {
		ret.at(b).at(a)=in.at(a).at(b); //mirror every value
		}
	}
return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//class Matrix should represent an two dimensional matrix
template<class flt=double>
class Matrix{
private:
std::vector<flt> m_d;  //the two dimensions should be flatened to an one dimensional std::vector
size_t m_rows;
size_t m_columns;

public:
Matrix delete_row_and_column(size_t r,size_t c) const{
std::vector<std::vector<flt>> ret;
ret.resize(0);  //empty vector
for (size_t row{0}; row!=rows();row++) {
        if (row!=r) {
                ret.push_back({});//insert a new row
                for (size_t col{0}; col!=columns();col++) {
                        if(col!=c) {
                                ret.back().push_back(at(row,col));//add elements to new row
                                }
                        }
                }
        }
return Matrix(ret);//convert 2d vector to Matrix-Object
}

Matrix(const Matrix& in) = default;

flt& at(size_t row_in,size_t column_in) {	//member access
return m_d.at(row_in*m_columns + column_in);	//in informatics indices begin at 0!!
}

flt at(size_t row_in, size_t column_in) const {	//const meber access
return m_d.at(row_in*m_columns + column_in);	//indices go from 0 to size-1  !!!!!
}

size_t rows(void) 	const {return m_rows;}
size_t columns(void) 	const {return m_columns;}
size_t size(void) 	const {return m_columns * m_rows;}

Matrix(void):m_d{0.0},m_rows{1},m_columns{1}  {}  	//default size is one, value is 0.0

Matrix(const std::vector<std::vector<flt>>& inp):Matrix{} {//construction from 2 dimensional vector of rows
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

operator std::vector<std::vector<flt>>() const  {
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

flt det(void) const {//calculate the determinant of matrix with Laplace's formula
if (rows()!=columns()) {error("Sorry cannot calculate determinant of a rectangular Matrix");}
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

Matrix transpose(void) const{
std::vector<std::vector<flt>> row_vec(*this);
return Matrix<flt>(row_column_mirror(row_vec));
}

};//class matrix


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

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


}//namespace rom


void rom_matrix_t(void){

std::cout<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the matrix library: "<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;


std::cout <<"Default Matrix"<< std::string(rom::Matrix<float>{}) << std::endl;
rom::Matrix<float>  a{{	{5.0,9.0,3.0,7.0,1.0,5.0, 7 , 1 },
			{6.0,9.0,3.0,7.0,0.0,5.0, 0 , 3 },
			{1.0,4.0,8.0,1.0,4.0,7.0, 4 , 6 },
			{6.0,3.0,9.0,6.0,2.0,7.0, 6 , 8 },
			{9.0,2.0,5.0,0.0,4.0,7.0, 0 , 9 },
			{1.0,4.0,8.0,2.0,5.0,9.0, 2 , 3 },
			{5.0,9.0,2.0,6.0,9.0,2.0, 5 , 4 },
			{9.0,2.0,6.0,2.0,5.0,9.0, 7 , 6 }	}};
std::cout <<"Matrix"<< std::string(a) << std::endl;
std::cout <<"Determinant "<< a.det() <<std::endl;
std::cout <<"Determinant of transpose is "<< a.transpose().det()<<  std::endl;

rom::Matrix<float>  b{{	{4.0,3.0},
			{7.0,5.0}	}};
std::cout <<"Matrix"<< std::string(b) << std::endl << "Determinant: " << b.det();
std::cout << std::endl;

rom::Matrix<float>  l{{	{1,0,3,8},
			{0,1,7,2}	}};
rom::Matrix<float>  r{{	{ 3, 8,36},
			{ 7, 2,34},
			{-1, 0,-4},
			{ 0,-1,-3}	}};
auto result = l*r;
std::cout <<"Matrix"<< std::string(l) <<" * "<< std::endl;
std::cout <<"Matrix"<< std::string(r) <<" = "<< std::endl;
std::cout <<"Matrix"<< std::string(result) <<"  "<< std::endl;

rom::Matrix<float>  l1{{	{2, 4,-3, 1},
				{3,-1, 2, 7}	}};
rom::Matrix<float>  r1{{	{10, 1, 11},
				{ 3, 7, 10},
				{-2, 6, 4},
				{ 8, 2, 10}	}};
auto result1 = l1*r1;
std::cout <<"Matrix"<< std::string(l1) <<" * "<< std::endl;
std::cout <<"Matrix"<< std::string(r1) <<" = "<< std::endl;
std::cout <<"Matrix"<< std::string(result1) <<"  "<< std::endl;
std::cout <<"It's transpose is: "<< std::string(result1.transpose()) <<"  "<< std::endl;
std::cout <<"It's transpose is: "<< std::string(result1.transpose()) <<"  "<< std::endl;
std::cout << uint16_t(l1==r1) <<"\t" << uint16_t(l1==l1.transpose().transpose()) << std::endl;


}


#endif

