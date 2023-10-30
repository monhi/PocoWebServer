// Please do not remove this comment.
//
// URL Encoding/Decodeing class, supports Unicode and ANSI (MFC TCHAR)
//
// Written by Daniel Cohen Gindi, (danielgindi (at) gmail.com)
//
// If you have any comments or questions, feel free to email me.
//
// You may use this class for any purpose, commercial or personal.

#include "stdafx.h"
#include "URLEncode.h"
#include <windows.h>
#include "general.h"
#include "time.h"

// Put the percent (%) sign first, so it won't overwrite the converted Hex'es
std::string		CURLEncode::m_lpszUnsafeString		=(" \"<>#{}|\\^~[]`");
int				CURLEncode::m_iUnsafeLen			=(int)strlen(m_lpszUnsafeString.c_str());
std::string		CURLEncode::m_lpszReservedString	=("$&+,./:;=?@-_!*()'");
int				CURLEncode::m_iReservedLen			=(int)strlen(m_lpszReservedString.c_str());
// Convert a byte into Hexadecimal CString


// Convert a Unicode or ANSI character into UTF8
short CURLEncode::toUTF8(TCHAR tc)
{
	wchar_t		wc[2];
	char		mb[3];

	// If application character set is ANSI,
	// we need to convert to Unicode first
	if (sizeof(TCHAR)==1)
	{
		mb[0]=(char)tc;
		mb[1]=0;
		MultiByteToWideChar(URLENCODE_DEFAULT_ANSI_CODEPAGE, 0, mb, 3, wc, 2);
	}

	if (sizeof(TCHAR)>1) wc[0]=tc;
	wc[1]=0;
	mb[0]=0;
	WideCharToMultiByte(CP_UTF8, 0, wc, 2, mb, 3, 0, 0);
	return MAKEWORD(mb[1], mb[0]);
}

// Convert a UTF8 character into Unicode or ANSI
TCHAR CURLEncode::fromUTF8(short w)
{
	WCHAR wc[2];
	CHAR mb[3];

	mb[0]=HIBYTE(w);
	mb[1]=LOBYTE(w);
	mb[2]=0;
	wc[0]=0;

	MultiByteToWideChar(CP_UTF8, 0, mb, 3, wc, 2);
	if (sizeof(TCHAR)==1)
	{
		mb[0]=0;
		WideCharToMultiByte(URLENCODE_DEFAULT_ANSI_CODEPAGE, 0, wc, 2, mb, 3, 0, 0);
		return mb[0];
	} else {
		return (TCHAR)wc[0];
	}
}

// strURL:			URL to encode.
// bEncodeReserved: Encode the reserved characters
//                  for example the ? character, which is used many times
//                  for arguments in URL.
//                  so if we are encoding just a string containing Keywords,
//                  we want to encode the reserved characters.
//                  but if we are encoding a simple URL, we wont.


#define isHex(c) (((c)>=_T('A') && (c)<=_T('F')) || \
				 ((c)>=_T('a') && (c)<=_T('f')) || \
				 ((c)>=_T('0') && (c)<=_T('9')))
#define hexToDec2(c) (((c)>=_T('A') && (c)<=_T('F')) ? c-_T('A')+10 : \
					(((c)>=_T('a') && (c)<=_T('f')) ? c-_T('a')+10 : c-_T('0')))

#define hexToDec(high, low) (hexToDec2(high)<<4 | hexToDec2(low))

// strURL: URL to decode.
/*
std::wstring CURLEncode::Decode(std::string strURL)
{
	int		i=strURL.find('%');
	TCHAR	tc1=0, tc2=0;
	u8		b=0;
	bool	bFound=false;
	int		k=0;

	std::wstring retu= L"";
	int		j=0;

	while (i>-1)
	{
		tc1=strURL[i+1];
		tc2=strURL[i+2];

		if (isHex(tc1) && isHex(tc2))
		{
			b=hexToDec(tc1, tc2);
			// first deal with 1-byte unprintable characters
			if (b<0x1F || b==0x7F) 
			{
				retu[j++]= b;
				i+=2;
			} 
			else 
			{
				// Then deal with 1-byte unsafe/reserved characters
				// We are reading for those static LPCTSTR strings,
				// so we have nice support for Unicode
				bFound=false;
				for (int ii=0; ii<m_iUnsafeLen && !bFound; ii++)
				{
					if (__toascii(m_lpszUnsafeString[ii])==b)
					{
						retu += m_lpszUnsafeString[ii];
						i+=2;
						bFound=TRUE;
					}
				}
				for (int ii=0; ii<m_iReservedLen && !bFound; ii++)
				{
					if (__toascii(m_lpszReservedString[ii])==b)
					{
						retu += m_lpszReservedString[ii];
						i+=2;
						bFound=TRUE;
					}
				}

				if (!bFound)
				{
					if (strURL[i+3]==('%'))
					{
						tc1=strURL[i+4];
						tc2=strURL[i+5];

						if (isHex(tc1) && isHex(tc2))
						{
							BYTE b2=hexToDec(tc1, tc2);
							retu = retu + fromUTF8(MAKEWORD(b2, b));
							i+= 5;
						}
					}
				}
			}
		}
		else
		{
			k++;
		}
		i=strURL.find(('%'), i+1);
	}
	return retu;
}
*/

std::wstring CURLEncode::Decode(std::string strURL)
{
	size_t      i;
	TCHAR		tc1=0,tc2=0;
	u8          b=0;
	bool		bFound=false;
	int         k=0;

	std::wstring retu = L"";
	int         j = 0;
	for (i = 0; i < strURL.size();)
	{
		if (strURL[i] == '%')
		{
			tc1 = strURL[i + 1];
			tc2 = strURL[i + 2];

			if (isHex(tc1) && isHex(tc2))
			{
				b = hexToDec(tc1, tc2);
				// first deal with 1-byte unprintable characters
				if (b < 0x1F || b == 0x7F)
				{
					retu += b;
					i += 2;
				}
				else
				{
					// Then deal with 1-byte unsafe/reserved characters
					// We are reading for those static LPCTSTR strings,
					// so we have nice support for Unicode
					bFound = false;
					for (int ii = 0; ii < m_iUnsafeLen && !bFound; ii++)
					{
						if (m_lpszUnsafeString[ii] == b)
						{
							retu += b;
							i += 3;
							bFound = TRUE;
						}
					}

					for (int ii = 0; ii < m_iReservedLen && !bFound; ii++)
					{
						if (m_lpszReservedString[ii] == b)
						{
							retu += b;
							i += 3;
							bFound = TRUE;
						}
					}

					if (!bFound)
					{
						if (strURL[i + 3] == ('%'))
						{
							tc1 = strURL[i + 4];
							tc2 = strURL[i + 5];

							if (isHex(tc1) && isHex(tc2))
							{
								BYTE b2 = hexToDec(tc1, tc2);
								retu = retu + fromUTF8(MAKEWORD(b2, b));
								i += 6;
							}
						}
					}
				}
			}
		}
		else
		{
			retu += strURL[i];
			i++;
		}
	}
	return retu;
}


time_t CURLEncode::GetTimeFromString(std::wstring param)
{
	time_t		t;
	size_t		i;
	char		c;
	wchar_t		d;
	wchar_t		zero = 1776;
	wchar_t		nine = 1785;
	std::string dest = "";

	for (i = 0; i < param.size(); i++)
	{
		d = param[i];
		if ((d >= zero) && (d <= nine))
		{
			c = (d - zero) + '0';
			dest += c;
		}
		else
		{
			dest += (d & 0x00FF);
		}
	}
	//std::cout << dest << std::endl;
	//dest = "1396/07/28+15:4";
	int year, month, day, hour, minute;
	sscanf(dest.c_str(), "%d/%d/%d+%d:%d", &year, &month, &day, &hour, &minute);
	// jalali to gregorian
	int g_y, g_m, g_d;
	jalali_to_gregorian(&g_y, &g_m, &g_d, year, month, day);
	////////////////////////////////////////////////////
	struct tm  timeinfo;
	/* prompt user for date */
	timeinfo.tm_year = g_y - 1900;
	timeinfo.tm_mon = g_m - 1;
	timeinfo.tm_mday = g_d;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minute;
	timeinfo.tm_sec = 0;
	timeinfo.tm_isdst = 0;
	t = mktime(&timeinfo);
	return t;
}
