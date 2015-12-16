#pragma once


namespace Aid {


class Exceptions
{
public:
	void operator()();

private:
	static const DWORD kExceptionCode = 0xBEEF;
	static const ULONG_PTR kArguments[];

	static void RaiseException();
};


}
