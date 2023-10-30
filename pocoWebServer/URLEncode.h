// Please do not remove this comment.
//
// URL Encoding/Decodeing class, supports Unicode and ANSI (MFC TCHAR)
//
// Written by Daniel Cohen Gindi, (danielgindi (at) gmail.com)
//
// If you have any comments or questions, feel free to email me.
//
// You may use this class for any purpose, commercial or personal.

#pragma once

#include "stdafx.h"
#include <string>
#include <string>
//#include <atlstr.h>

// For ANSI, set default codepage to Hebrew.
// This is used for conversion into UTF-8 from ANSI (Through Unicode)
// You can change this to your codepage.
#define URLENCODE_DEFAULT_ANSI_CODEPAGE 1255

typedef unsigned char u8;

class CURLEncode
{
private:
	static std::string	m_lpszUnsafeString;
	static int			m_iUnsafeLen;
	static std::string	m_lpszReservedString;
	static int			m_iReservedLen;
	short				toUTF8(TCHAR tc);
	TCHAR				fromUTF8(short w);
public:
	std::wstring		Decode(std::string strURL);
	time_t				GetTimeFromString(std::wstring param);
};