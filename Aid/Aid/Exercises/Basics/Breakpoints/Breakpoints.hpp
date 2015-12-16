#pragma once


namespace Aid {


class Breakpoints
{
public:
	void operator()();

private:
	static const std::uint8_t kBreakpointOpcode = 0xCC;

	static bool IsBreakpointSet(const std::uint8_t * address);

	static void DummyFunction();

	static void DummyFunction2();
};


}
