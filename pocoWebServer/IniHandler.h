#pragma once
#include <string>
#include "sqlite3.h"
#include <map>
#include <string>

class CIniHandler
{
private:

public:
	CIniHandler(std::string fileName);
	int			GetValue(char* section, char* key, char*& value, char* defaultV);
	int			Save();
	int			SetValue(char* section, char* key, char* value);

	~CIniHandler();
private:
	int			m_status;
	sqlite3		*db;
	char		*zErrMsg;
	int			rc;
	int			m_userCounter;
public:
	void		OpenDB();
	void		CheckServerTable();
	void		CheckAccountTable();
	int			SearchUsers();
	void		CheckChannelTables(int i);
	bool		GetTableExists(){ return m_tableExists; }
	void		SetTableExists(bool val){ m_tableExists = val; }
	bool		GetUserExists(){ return m_userExists; }
	void		SetUserExists(bool val){ m_userExists = val; }
	void		SetkeyValue(std::string key, std::string value){ keymap[key]=value; }
	int			CheckIfUserExists(std::string userName, std::string password);
	void		AddUsertoList(int argc, char ** argv, char ** azColName);
	void		ClearMaps();
	int			GetUserNumber();
	int			CountUsers();
	int			DeleteUser(int idx);
	void		AddUserCounter();
	int			GetUser(int idx);
	void		SetUserEditData(int argc, char ** argv, char ** azColName);
	int			UpdateUser(std::string USERID, std::wstring FIRSTNAME, std::wstring LASTNAME, std::string PHONE, std::string EMAIL, std::wstring ADDRESS, std::string USERNAME, std::string PASSWORD, int ACCESS);
	int			AddUser(std::wstring firstName, std::wstring lastName, std::string phone, std::string email, std::wstring address, std::string userName, std::string finalPassword, int access);
private:
	std::string m_fileName;
	std::string m_configFileName;
	bool		m_tableExists;
	bool		m_userExists;
	std::map<std::string, std::string>	keymap;
public:
	std::map<int, int>						m_userIdMap;
	std::map<int, std::wstring>				m_firstNameMap;
	std::map<int, std::wstring>				m_lastNameMap;
	std::map<int, std::string>				m_phoneMap;
	std::map<int, std::string>				m_emailMap;
	std::map<int, std::wstring>				m_addressMap;
	std::map<int, std::string>				m_userNameMap;
public: // public data for edit
	int										m_editUserId;
	std::wstring							m_editFirstName;
	std::wstring							m_editLastName;
	std::string								m_editPhone;
	std::string								m_editEmail;
	std::wstring							m_editAddress;
	std::string								m_editUserName;
	std::string								m_editPassword;
};

