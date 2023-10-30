#include "ClientHandlerFactory.h"
#include "ClientHandler.h"
#include "WebSocketRequestHandler.h"


CClientHandlerFactory::CClientHandlerFactory(const std::string& format):_format(format)
{

}

CClientHandlerFactory::~CClientHandlerFactory()
{

}

HTTPRequestHandler* CClientHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
	std::string urI = request.getURI();
	if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
	{
		/*
		Poco::Net::NameValueCollection cookies;
		request.getCookies(cookies);
		Poco::Net::NameValueCollection::ConstIterator it = cookies.find("Session");
		if (it != cookies.end())
		{
			std::string SessionName = "";
			std::string SessionPath = "";
			SessionName = it->second;
			if (SessionName == "Login")
			{
				return new WebSocketRequestHandler();
			}
		}
		return new CClientHandler(_format);
		*/
		return new WebSocketRequestHandler();
	}
	else
	{
		return new CClientHandler(_format);
	}
}