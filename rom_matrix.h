#ifndef rom_matrix
#define rom_matrix

#include "rom_error.h"

namespace rom {

int8_t parity_of_permutation(size_t inp) {return (inp%2)?-1:1;}

////////////////////////////////////////////////////////////////////////////////////////////////////
//class Vector should represent an two dimensional matrix
template<class flt=double>
class Matrix{
private:
std::vector<flt> m_d;  //the two dimensions should be flatened to an one dimensional std::vector
size_t m_rows;
size_t m_columns;

Matrix delete_row_and_column(size_t r,size_t c) const{
std::vector<std::vector<flt>> ret;
ret.resize(0);  //empty vector
for (size_t row{0}; row!=rows();row++) {
        if (row!=r) {
                ret.push_back({});
                for (size_t col{0}; col!=columns();col++) {
                        if(col!=c) {
                                ret.back().push_back(at(row,col));
                                }
                        }
                }
        }
return Matrix(ret);
}

public:
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

Matrix(const std::vector<std::vector<flt>>& inp):Matrix{} {//construction from 2 dimensional vector
m_rows = inp.size();
m_columns = inp.at(0).size();//ToDo: check if every row has the same number of columns
m_d.resize(size());
for (size_t r{0};r!=rows();r++) { 		//every row
	for (size_t c{0};columns()!=c;c++) { 	//value in column
		this->at(r,c)=inp.at(r).at(c);
		}
	}
}

flt det(void) const {//calculate the determinant of matrix with Laplace's formula
if (rows()!=columns()) {error("Sorry cannot calculate determinant of a rectangular Matrix");}
if (rows() < 2){error("Determinant is not defined if size of matrix is smaller than 2x2");}
flt ret{0.0};
if (rows()==2) {ret = at(0,0)*at(1,1)-at(0,1)*at(1,0);}//perform simple Sarrus(2x2) rule
else 	{
	for (size_t r{0};r!=rows();r++) {
		flt mul{delete_row_and_column(r,0).det()};//recursion is slow but correct
		mul *= at(r,0);
 		mul *= parity_of_permutation(r+0);
		ret += mul;
		}
	}
return ret;
}


operator std::string() const {//make it easy to output internal data of vector class
std::ostringstream o;
for (size_t r{0};r<rows();++r) {
	o << std::endl << "{ ";
	for (size_t c{0};c<columns();c++) {o << " (" << m_d.at(c+r*columns())<<")";}
	o  << " }";
	}
return o.str();
}


};//class matrix

}//namespace rom


void rom_matrix_t(void){

std::cout<<std::endl;
std::cout <<"//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout <<"Testing the vector library: "<<std::endl;
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
std::cout <<"Determinant "<< a.det() << std::endl;

std::cout <<"Matrix"<< std::string(a=a.delete_row_and_column(1,1)) << std::endl;
std::cout <<"Determinant "<< a.det() << std::endl;

std::cout <<"Matrix"<< std::string(a=a.delete_row_and_column(1,1)) << std::endl;
std::cout <<"Determinant "<< a.det() << std::endl;

rom::Matrix<float>  b{{{4.0,3.0},{7.0,5.0}}};
std::cout <<"Matrix"<< std::string(b) << std::endl << "Determinant: " << b.det();



}


#endif

