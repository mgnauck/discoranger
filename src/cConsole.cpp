
 // cConsole.cpp

#include "cConsole.h"

cConsole::cConsole()
{
	BOOL r = AllocConsole();
	m_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

cConsole::~cConsole()
{
	BOOL r = FreeConsole();
}

cConsole::Write(char *p_String)
{
	unsigned long written;
	BOOL r = WriteConsole(m_Handle, p_String, strlen(p_String), &written, NULL);
}

cConsole::WriteDigit(int p_Digit, int p_Base)
{
	char p_String[512];
	itoa(p_Digit, p_String, p_Base);
	Write(p_String);
}

cConsole::WriteDigit(float p_Digit)
{
	char p_String[512];
	char *p_Temp;
	p_Temp = _gcvt(p_Digit, 4, p_String);
	Write(p_String);
}

cConsole::SetCursor(int p_XPos, int p_YPos)
{
	COORD p = {p_XPos, p_YPos};
	BOOL r = SetConsoleCursorPosition(m_Handle, p);
}

cConsole::SetTitle(char *p_Title)
{
	BOOL r = SetConsoleTitle(p_Title);
}