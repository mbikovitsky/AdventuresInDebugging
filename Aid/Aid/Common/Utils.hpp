#pragma once

#include "WinApiHelpers\WinApiHelpers.hpp"


namespace Aid { namespace Utils {


void ClearScreen();


typedef void (*ChildFunction)();

WinApiHelpers::PlainHandle ForkToAddress(ChildFunction childFunction);

ptrdiff_t PointerToRva(const void * pointer);
const void * RvaToPointer(ptrdiff_t rva);


} }
