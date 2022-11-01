
 // cConsole.h

#pragma once

#include <windows.h>
#include <stdlib.h>

class cConsole
{
	public:
		cConsole();
		cConsole(char *p_Title);
		~cConsole();

		HANDLE	m_Handle;

		Write(char *p_String);
		WriteDigit(int p_Digit, int p_Base);
		WriteDigit(float p_Digit);
		SetCursor(int p_XPos, int p_YPos);
		SetTitle(char *p_Title);
};