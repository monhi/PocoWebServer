#include "ClientHandler.h"
#include <string>
//#include "db.h"
#include "general.h"
#include "base64.h"
#include <iostream>
#include "IniHandler.h"
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include "URLEncode.h"
//#include "Db.h"


CClientHandler::CClientHandler(std::string format)
{
	std::string  m_temp;
	char		 name[MAX_PATH];
	GetModuleFileNameA(NULL, name, MAX_PATH);
	m_temp = name;

	std::size_t found = m_temp.find_last_of("/\\");
	m_path = m_temp.substr(0, found + 1);
	m_path.append("CRS2");
	//m_db = new CDb();
}

CClientHandler::~CClientHandler()
{
	/*
	if (m_db)
	{
		delete m_db;
		m_db = nullptr;
	}
	*/
}

void CClientHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	app.logger().information("Request from " + request.clientAddress().toString() + " "+ request.getMethod() + " "+request.getURI());

	std::string method	= request.getMethod();
	std::string uri		= request.getURI();
	if (method == HTTPRequest::HTTP_GET)
	{
		if (uri == "/DefaultError.htm")
		{
			ProcessDefaultError(request,response);
		}
		else if (uri == "/" || uri == "/Default.htm")
		{
			ProcessDefault(request, response);
		}
		else if (uri == "/Default.htm?Logout=1")
		{
			ProcessLogout(request, response);
		}
		else
		{
			ProcessOtherReqs(request,response);
		}
	}
	else if (method == HTTPRequest::HTTP_POST)
	{
		if ( uri == "/Default.htm" )
		{
			ProcessLogin(request, response);
		}
		else if (uri == "/CallList.htm")
		{
			//ProcessCallList(request, response);
		}
	}
}

void CClientHandler::SendDefaultPage(HTTPServerResponse& response)
{
	std::string		fileName = m_path;
	fileName += "\\default.htm";
	response.sendFile(fileName, "text/html");
}

void CClientHandler::ProcessLogin(HTTPServerRequest& request, HTTPServerResponse& response)
{
	std::istream &i = request.stream();
	int len = (int)request.getContentLength();
	char* buffer = new char[len + 1];
	memset(buffer, 0, len);
	i.read(buffer, len);
	buffer[len] = 0;
	// Parse buffer to handle data.
	char *p = strtok(buffer, "&");
	std::string UserNameToken = "txtUserName=";
	std::string UserName;
	bool		UserNameFound = false;
	std::string PasswordToken = "txtPassword=";
	std::string Password;
	bool		PasswordFound = false;

	std::string RememberToken = "chkRememberMe=";
	std::string Remember;
	bool		RememberFound = false;

	std::size_t found;
	std::string temp;
	while (p)
	{
		printf("Token: %s\n", p);
		temp = p;

		found = temp.find(UserNameToken);
		if (found != std::string::npos)
		{
			temp.erase(found, UserNameToken.length());
			UserName = temp;
			UserNameFound = true;
		}

		found = temp.find(PasswordToken);
		if (found != std::string::npos)
		{
			temp.erase(found, PasswordToken.length());
			Password = temp;
			PasswordFound = true;
		}

		found = temp.find(RememberToken);
		if (found != std::string::npos)
		{
			temp.erase(found, RememberToken.length());
			Remember = temp;
			RememberFound = true;
		}
		p = strtok(NULL, "&");
	}
	// we are supposed to send file to the server.
	std::string dbPassword;
	CIniHandler *m_pIni;
	m_pIni = new CIniHandler(SETTING_FILE_NAME);

	if (m_pIni->CheckIfUserExists(UserName, Password) == SUCCESS)
	{
		Poco::Net::HTTPCookie cookie0("Session", "Login");
		cookie0.setPath("/");
		cookie0.setMaxAge(-1); // 1 hour
		response.addCookie(cookie0);

		Poco::Net::HTTPCookie cookie1("Path", "CallList");
		cookie1.setPath("/");
		cookie1.setMaxAge(-1); // 1 hour
		response.addCookie(cookie1);
		response.redirect("Channel.htm");
	}
	else
	{
		response.redirect("DefaultError.htm");
	}
	delete m_pIni;
	/////////////////////////////
	delete[]buffer;
}


void CClientHandler::ProcessDefaultError(HTTPServerRequest& request, HTTPServerResponse& response)
{
	std::string path = m_path;
	std::string req = request.getURI();
	path += req;
	std::replace(path.begin(), path.end(), '/', '\\');
	response.sendFile(path, "text/html");
}

void CClientHandler::ProcessDefault(HTTPServerRequest& request, HTTPServerResponse& response)
{
	std::string SessionName = "";
	std::string SessionPath = "";
	Poco::Net::NameValueCollection cookies;
	request.getCookies(cookies);
	Poco::Net::NameValueCollection::ConstIterator it = cookies.find("Session");
	if (it != cookies.end())
	{
		SessionName = it->second;
		if (SessionName == "Login")
		{
			response.redirect("Channel.htm");
		}
		else
		{
			SendDefaultPage(response);
		}
	}
	else
	{
		SendDefaultPage(response);
	}
}

void CClientHandler::ProcessOtherReqs(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	std::string req = request.getURI();
	std::string path = m_path;
	std::string stemp = req;
	path += req;
	//int a = req.find(".css");
	if (req.find(".css") != std::string::npos)
	{
		//response.setChunkedTransferEncoding(true);
		//response.setContentType("text/css");
		std::replace(path.begin(), path.end(), '/', '\\');
		response.sendFile(path, "text/css");
	}
	else if (req.find(".js") != std::string::npos)
	{
		std::replace(path.begin(), path.end(), '/', '\\');
		response.sendFile(path, "application/javascript");
	}
	else if (req.find(".png") != std::string::npos)
	{
		std::replace(path.begin(), path.end(), '/', '\\');
		response.sendFile(path, "image/png");
	}
	else if (req.find(".woff2") != std::string::npos)
	{
		std::replace(path.begin(), path.end(), '/', '\\');
		response.sendFile(path, "font/woff2");
	}
	else if (req.find(".ico") != std::string::npos)
	{
		std::replace(path.begin(), path.end(), '/', '\\');
		response.sendFile(path, "image/x-icon");
	}
	else if (req.find(".ogg") != std::string::npos)
	{
		try
		{
			std::replace(stemp.begin(), stemp.end(), '$', '\\');
			std::replace(stemp.begin(), stemp.end(), '@', ':');
			if (stemp[0] == '/')
			{
				stemp.erase(0, 1);
			}
			response.setChunkedTransferEncoding(true);
			response.setKeepAlive(true);
			response.sendFile(stemp, "audio/ogg");
		}
		catch (Poco::Exception& e)
		{
			std::string alpha = e.displayText();
			alpha += " ";
		}
	}
	else if (
		(req.find(".htm") != std::string::npos)
		||
		(req.find(".html") != std::string::npos)
		)
	{
		std::string SessionName = "";
		std::string SessionPath = "";
		Poco::Net::NameValueCollection cookies;
		request.getCookies(cookies);

		Poco::Net::NameValueCollection::ConstIterator it = cookies.find("Session");
		if (it != cookies.end())
		{
			SessionName = it->second;
			if (SessionName == "Login")
			{
				std::replace(path.begin(), path.end(), '/', '\\');
				response.sendFile(path, "text/html");
			}
			else
			{
				response.redirect("Default.htm");
			}
		}
		else
		{
			response.redirect("Default.htm");
		}
	}
	else if (req.find("/ws") != std::string::npos)
	{
		response.redirect("Default.htm");
	}

}

void CClientHandler::SendChannelHTML(HTTPServerRequest& request, HTTPServerResponse& response)
{
	FILE *f0;
	//FILE* f1;

	std::string path = m_path;
	std::string path0 = path;
	//std::string path1 = path;
	path0 += "\\Channel.htm";
	//path1 += "\\Channel.h1";
	response.setChunkedTransferEncoding(false);
	response.setContentType("text/html");
	std::ostream& ostr = response.send();

	f0 = fopen(path0.c_str(), "rb");
	if (f0)
	{
		fseek(f0, 0, SEEK_END);
		long size = ftell(f0);
		char* buffer = new char[size + 1];
		fseek(f0, 0, SEEK_SET);
		fread(buffer, 1, size, f0);
		buffer[size] = 0;
		ostr << buffer;
		fclose(f0);
		delete[]buffer;
	}
}

void CClientHandler::ProcessLogout(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Poco::Net::HTTPCookie cookie0("Session", "Login");
	cookie0.setPath("/");
	cookie0.setMaxAge(0); // 1 hour
	response.addCookie(cookie0);

	Poco::Net::HTTPCookie cookie1("Path", "CallList");
	cookie1.setPath("/");
	cookie1.setMaxAge(0); // 1 hour
	response.addCookie(cookie1);
	response.redirect("Default.htm");
}
