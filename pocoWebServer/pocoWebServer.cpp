// pocoWebServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HTTPClientServer.h"
#include "RegReader.h"
//#include "Db.h"
#include "winreg.h"
#include "TR4Connection.h"
#include "general.h"
#include <windows.h>


void ShowHelp()
{
	int maj, min, bui, rev;
	GetProgramVersionInfo(maj, min, bui, rev);
	color_printf(GREEN, " (TR4) Web Server program  version(%02d.%02d.%02d.%02d)\n", maj, min, bui, rev);
	color_printf(GREEN, " DSPCottage Co,\n");
	color_printf(GREEN, " All rights reserved.\n");
	color_printf(GREEN, " Use  TR4Service -i to install as a service.\n");
	color_printf(GREEN, " Use  TR4Service -u to remove the service.\n");
	color_printf(GREEN, " Use without switches to run as a command prompt program.\n");
}


int _tmain(int argc, TCHAR** argv)
{

	HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, 0, NAMED_MUTEX_CONSTANT_TEXT);

	if (!hMutex)
	{
		// Mutex doesn’t exist. This is
		// the first instance so create
		// the mutex.
		hMutex = CreateMutexA(0, 0, NAMED_MUTEX_CONSTANT_TEXT);
	}
	else
	{
		// The mutex exists so this is the
		// the second instance so return.
		color_printf(RED, " Another Instant of program is running.\n");
		color_printf(RED, " Quiting in 3 seconds...\n");		
		Sleep(3000);
		return FALSE;
	}

	CConfigReader* m_pRegReader = nullptr;
	CTR4Connection* m_pTr4 = CTR4Connection::GetInstance();


	CHTTPClientServer app;
	return app.run(argc, argv);
	delete m_pRegReader;
	delete m_pTr4;
	return Application::EXIT_OK;
}