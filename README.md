Repository	
rom_header:	Collection of C++11   

	File
	rom_error.h:	basic error handling

		Function
		-->rom::error(std::string)	
		use it to quit your program before it does something bad (division by zero,...)
		It will send a message to standard output and tell your os that an error occurred

		Function
		-->rom::ok(void)
		use it to quit your program without error

	File
	rom_fft.h:	a colection of functions and classes that can make Fourier transformations

		Function-Template
		<it_i,it_o> input-iterator, output-iterator
		-->rom::copy_range_checked(it_i,it_i,it_o,it_o)
		It will check the size of two iterator-Ranges.
		It will quit your program if the size does not match.
		If the sizes match, it will behave like std::cpoy
		Copies the elements in the range [it_i,it_i) into the range [it_o,it_o)

		Functor-class-template			
		<RamIt> random-access-iterator to std::complex<double> 
		The most general form of the Fourier transformation takes a complex input 
		and returns a complex result
		-->rom::dft{} 
		constructor without input
		-->rom::dft.operator()(RamIt,RamIt)
		will perform a discrete-fourier-transformation on the Input-Range	
		-->rom::dft.reverse(RamIt,RamIT)
		will perform an inverse discrete-fourier-transformation on the Input-Range					


		Functor-class-template
		<RamIt> random-access-iterator to std::complex<double> 
		-->rom::ffte{}  //the best fft function this close to the east pole  ;-)
		Does the same as rom::dft{} but is much faster.
		It uses O(n*log(n)) complexity for all sizes of Input except large prime numbers

		.
		.
		.



.
.
.
.
.
.
.
.
.
.
.
.
.
.
