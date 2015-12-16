#pragma once


// Common headers
#include <Windows.h>

#include <cstdint>

#include <iostream>
#include <string>
#include <sstream>

#include <boost\exception\all.hpp>


namespace Aid {


struct AidException : virtual boost::exception, virtual std::exception
{
};


}
