#ifndef BITTRANSFORM_H
#define BITTRANSFORM_H

#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>

template <typename T>
class ByteTransform {
	public:
		//Convert T to an array of bytes
		static unsigned char * convert(T value);
		//Convert an array of bytes to T
		static T revert(unsigned char * array);
		//Give us the size of the array of bytes
		static int getSize();
};

#include "ByteTransform.cpp"
#endif
