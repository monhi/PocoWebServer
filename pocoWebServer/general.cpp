#include "general.h"
#include <stdarg.h>
#include <stdio.h>
//#include <string>


int color_printf(int color, char* string, ...)
{
	va_list arg;
	int done;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	va_start(arg, string);
	done = vfprintf(stdout, string, arg);
	va_end(arg);
	return done;
}



int GetProgramVersionInfo(int &major, int &minor, int &build, int &revision)
{
	TCHAR filename[512];	
	GetModuleFileName(NULL, filename, 512);
	DWORD   verBufferSize;
	char    verBuffer[2048];
	//  Get the size of the version info block in the file
	verBufferSize = GetFileVersionInfoSize(filename, NULL);
	if (verBufferSize > 0 && verBufferSize <= sizeof(verBuffer))
	{
		//  get the version block from the file
		if (TRUE == GetFileVersionInfo(filename, NULL, verBufferSize, verBuffer))
		{
			UINT length;
			VS_FIXEDFILEINFO *verInfo = NULL;
			//  Query the version information for neutral language
			if (TRUE == VerQueryValueA(verBuffer, ("\\"), reinterpret_cast<LPVOID*>(&verInfo), &length))
			{
				//  Pull the version values.
				major = HIWORD(verInfo->dwProductVersionMS);
				minor = LOWORD(verInfo->dwProductVersionMS);
				build = HIWORD(verInfo->dwProductVersionLS);
				revision = LOWORD(verInfo->dwProductVersionLS);
				return SUCCESS;
			}
		}
	}
	major		= PROGRAM_MAJOR_VERSION;
	minor		= PROGRAM_MINOR_VERSION;
	build		= 0;
	revision	= 0;
	return FAILURE;
}



int g_days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int j_days_in_month[12] = { 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29 };
const char *j_month_name[13] = { "",
"Farvardin", "Ordibehesht", "Khordad",
"Tir", "Mordad", "Shahrivar",
"Mehr", "Aban", "Azar",
"Dey", "Bahman", "Esfand" };

void gregorian_to_jalali(int *j_y, int *j_m, int *j_d, int  g_y, int  g_m, int  g_d)
{
	int gy, gm, gd;
	int jy, jm, jd;
	long g_day_no, j_day_no;
	int j_np;

	int i;

	gy = g_y - 1600;
	gm = g_m - 1;
	gd = g_d - 1;

	g_day_no = 365 * gy + (gy + 3) / 4 - (gy + 99) / 100 + (gy + 399) / 400;
	for (i = 0; i < gm; ++i)
		g_day_no += g_days_in_month[i];
	if (gm > 1 && ((gy % 4 == 0 && gy % 100 != 0) || (gy % 400 == 0)))
		/* leap and after Feb */
		++g_day_no;
	g_day_no += gd;

	j_day_no = g_day_no - 79;

	j_np = j_day_no / 12053;
	j_day_no %= 12053;

	jy = 979 + 33 * j_np + 4 * (j_day_no / 1461);
	j_day_no %= 1461;

	if (j_day_no >= 366) {
		jy += (j_day_no - 1) / 365;
		j_day_no = (j_day_no - 1) % 365;
	}

	for (i = 0; i < 11 && j_day_no >= j_days_in_month[i]; ++i) {
		j_day_no -= j_days_in_month[i];
	}
	jm = i + 1;
	jd = j_day_no + 1;
	*j_y = jy;
	*j_m = jm;
	*j_d = jd;
}

void jalali_to_gregorian(int *g_y, int *g_m, int *g_d, int  j_y, int  j_m, int  j_d)
{
	int gy, gm, gd;
	int jy, jm, jd;
	long g_day_no, j_day_no;
	int leap;

	int i;

	jy = j_y - 979;
	jm = j_m - 1;
	jd = j_d - 1;

	j_day_no = 365 * jy + (jy / 33) * 8 + (jy % 33 + 3) / 4;
	for (i = 0; i < jm; ++i)
		j_day_no += j_days_in_month[i];

	j_day_no += jd;

	g_day_no = j_day_no + 79;

	gy = 1600 + 400 * (g_day_no / 146097); /* 146097 = 365*400 + 400/4 - 400/100 + 400/400 */
	g_day_no = g_day_no % 146097;

	leap = 1;
	if (g_day_no >= 36525) /* 36525 = 365*100 + 100/4 */
	{
		g_day_no--;
		gy += 100 * (g_day_no / 36524); /* 36524 = 365*100 + 100/4 - 100/100 */
		g_day_no = g_day_no % 36524;

		if (g_day_no >= 365)
			g_day_no++;
		else
			leap = 0;
	}

	gy += 4 * (g_day_no / 1461); /* 1461 = 365*4 + 4/4 */
	g_day_no %= 1461;

	if (g_day_no >= 366) {
		leap = 0;

		g_day_no--;
		gy += g_day_no / 365;
		g_day_no = g_day_no % 365;
	}

	for (i = 0; g_day_no >= g_days_in_month[i] + (i == 1 && leap); i++)
		g_day_no -= g_days_in_month[i] + (i == 1 && leap);
	gm = i + 1;
	gd = g_day_no + 1;

	*g_y = gy;
	*g_m = gm;
	*g_d = gd;
}


/*
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

std::string ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}
*/