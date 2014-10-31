///////////////////////////////////////////////////////////////////////////////
//
// winsetres.cpp
//
// Copyright 2014 OpenDirective Ltd.
//
// Licensed under the New BSD license. You may not use this file except in
// compliance with this License.
//
// You may obtain a copy of the License at
// https://github.com/gpii/windows/blob/master/LICENSE.txt
//
// The research leading to these results has received funding from 
// the European Union's Seventh Framework Programme (FP7/2007-2013) 
// under grant agreement no. 289016.
//
// Spike to test changing resolution on Windows
//
///////////////////////////////////////////////////////////////////////////////

#include <SDKDDKVer.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

static SIZE getScreenResolution()
{
	int index = 0;
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
	{
		return { dm.dmPelsWidth, dm.dmPelsHeight };
	}

	return { 0, 0 };

}

static BOOL setScreenResolution(SIZE res)
{
	const LONG width = res.cx;
	const LONG height = res.cy;

	int index = 0;
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	while (0 != EnumDisplaySettings(NULL, index++, &dm))
	{
		if (dm.dmPelsWidth == width && dm.dmPelsHeight == height)
		{
			DEVMODE dmCurrent;
			ZeroMemory(&dmCurrent, sizeof(dmCurrent));
			dmCurrent.dmSize = sizeof(dmCurrent);
			if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCurrent))
			{
				// Note: ChangeDisplaySettings has no effect if new is same as current 
				dmCurrent.dmPelsWidth = width;
				dmCurrent.dmPelsHeight = height;
				long lRet = ChangeDisplaySettings(&dmCurrent, 0);
                if (DISP_CHANGE_SUCCESSFUL == lRet)
				{
					return TRUE;
				}
				if (DISP_CHANGE_RESTART == lRet)
				{
					printf("Reset required\r\n");
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	static wchar_t buf[100];

	SIZE res = getScreenResolution();
	swprintf_s(buf, _countof(buf), L"Current: %u %u\r\n", res.cx, res.cy);
	wprintf(buf);

	if (argc > 2)
	{
		SIZE newRes = { _wtoi(argv[1]), _wtoi(argv[2]) };
		BOOL r = setScreenResolution(newRes);
		printf((r) ? "Set OK\r\n" : "Set Failed\r\n");

	    res = getScreenResolution();
	    swprintf_s(buf, _countof(buf), L"New: %u %u\r\n", res.cx, res.cy);
	    wprintf(buf);
    }

	return 0;
}

