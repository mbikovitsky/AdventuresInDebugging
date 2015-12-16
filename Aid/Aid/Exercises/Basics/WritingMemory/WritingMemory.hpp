#pragma once


namespace Aid
{
	class WritingMemory
	{
	public:
		void operator()();

	private:
		static const WORD kForegroundAttrsMask = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
		static const WORD kBackgroundAttrsMask = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;

		static WORD GetForegroundAttributes(const CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info);
		static WORD GetBackgroundAttributes(const CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info);

		static void SetColorAttributes(WORD foreground, WORD background, CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info);
	};
}
