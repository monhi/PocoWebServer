#include "stdafx.h"
#include "IniHandler.h"
#include "general.h"
#include <string>
#include <windows.h>
#include "sha1.h"
#include <string>
#include <atlconv.h>




static int get_edit_callback(void *data, int argc, char **argv, char **azColName)
{
	CIniHandler* m_pConfig = (CIniHandler*)data;
	m_pConfig->SetUserEditData(argc,argv, azColName);
	return 0;
}

static int add_users_callback(void *data, int argc, char **argv, char **azColName)
{
	CIniHandler* m_pConfig = (CIniHandler*)data;
	m_pConfig->AddUserCounter();
	return 0;
}

static int select_users_callback(void *data, int argc, char **argv, char **azColName)
{
	CIniHandler* m_pConfig = (CIniHandler*)data;
	m_pConfig->AddUsertoList(argc,argv,azColName);
	return 0;
}

static int select_callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	CIniHandler* m_pConfig = (CIniHandler*)data;
	if (argc)
	{
		m_pConfig->SetUserExists(true);
	}
	else
	{
		m_pConfig->SetUserExists(false);
	}

	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

static int key_Check_Callback(void *data, int argc, char **argv, char **azColName)
{
	CIniHandler* m_pConfig = (CIniHandler*)data;
	if (argc > 2)
	{
		m_pConfig->SetkeyValue(argv[0], argv[1]);
	}
	return SUCCESS;
}

static int Table_Check_Callback(void *data, int argc, char **argv, char **azColName)
{
	CIniHandler* m_pConfig = (CIniHandler*)data;
	m_pConfig->SetTableExists(true);
	return SUCCESS;
}

CIniHandler::CIniHandler(std::string fileName)
{
	m_fileName = fileName;
	OpenDB();
	if (m_status == SUCCESS)
	{
		CheckAccountTable();
		CheckServerTable();
		for (int i = 0; i < MAX_CHANNEL_NUMBER; i++)
		{
			CheckChannelTables(i);
		}
	}
}

CIniHandler::~CIniHandler()
{
	if (m_status == SUCCESS)
	{
		sqlite3_close(db);
	}
}


int CIniHandler::GetValue(char* section, char* key, char*& value, char* defaultV)
{
	char  sql[MAX_PATH];
	sprintf(sql, "SELECT * FROM %s WHERE key ='%s';", section,key);
	rc = sqlite3_exec(db, sql, key_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		strcpy(value, defaultV);
		return FAILURE;
	}

	if (keymap.find(key) != keymap.end())
	{
		strcpy(value, keymap[key].c_str());
	}
	else
	{
		strcpy(value, defaultV);
	}
	return SUCCESS;
}

int CIniHandler::SetValue(char* section, char* key, char* value)
{
	char  sql[MAX_PATH];
	sprintf(sql, "insert or replace into %s (KEY , VALUE , USERID ) values('%s','%s','-1');", section, key,value);
	rc = sqlite3_exec(db, sql, key_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return SUCCESS;
}

int CIniHandler::Save()
{
	return SUCCESS;
}

void CIniHandler::OpenDB()
{
	char  buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	m_configFileName = std::string(buffer).substr(0, pos + 1);
	m_configFileName += m_fileName.c_str();
	rc = sqlite3_open(m_configFileName.c_str(), &db);
	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		m_status = FAILURE;
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
		m_status = SUCCESS;
	}
}

void CIniHandler::CheckChannelTables(int i)
{
	char  sql[MAX_PATH];
	SetTableExists(false);
	sprintf(sql, "SELECT * FROM sqlite_master WHERE name ='CH%d' and type='table';", i);
	rc = sqlite3_exec(db, sql, Table_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
		if (GetTableExists() == true)
		{
			// do nothing.
		}
		else
		{
			// create table.
			sprintf(sql, "CREATE TABLE CH%d(KEY TEXT NOT NULL PRIMARY KEY,VALUE TEXT NOT NULL, USERID INTEGER);", i);
			rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			else
			{
				fprintf(stderr, "SQL table creation successful.\n");
				// Add default keys and values:
				sprintf(sql, "INSERT INTO CH%d(KEY,VALUE, USERID)	VALUES('Name', 'channel%d', -1);",i,i);
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}

				sprintf(sql, "INSERT INTO CH%d(KEY,VALUE, USERID)	VALUES('Status', '0', -1);", i);
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}

				sprintf(sql, "INSERT INTO CH%d(KEY,VALUE, USERID)	VALUES('NoiseGain', '20', -1);", i);
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
			}
		}
	}
}

void CIniHandler::CheckServerTable()
{
	char  sql[MAX_PATH];
	SetTableExists(false);
	sprintf(sql, "SELECT * FROM sqlite_master WHERE name ='SERVER' and type='table';");
	rc = sqlite3_exec(db, sql, Table_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
		if (GetTableExists() == true)
		{
			// do nothing.
		}
		else
		{
			// create table.
			sprintf(sql, "CREATE TABLE SERVER(KEY TEXT NOT NULL PRIMARY KEY,VALUE TEXT NOT NULL, USERID INTEGER);");
			rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			else
			{
				fprintf(stderr, "SQL table creation successful.\n");
				// Add default keys and values:
				// add default drive
				sprintf(sql, "INSERT INTO SERVER(KEY,VALUE, USERID)	VALUES('drive', '4', -1);");
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				// add default retention days
				sprintf(sql, "INSERT INTO SERVER(KEY,VALUE, USERID)	VALUES('RetentionDays', '7', -1);");
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				// Add default retention days
				sprintf(sql, "INSERT INTO SERVER(KEY,VALUE, USERID)	VALUES('DBUserName', 'root', -1);");
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				// Add default DB user name
				sprintf(sql, "INSERT INTO SERVER(KEY,VALUE, USERID)	VALUES('DBPassword', 'cm9vdA==', -1);");
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				// Add default WebServer Port
				sprintf(sql, "INSERT INTO SERVER(KEY,VALUE, USERID)	VALUES('DBWebServerPort', '80', -1);");
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
			}
		}
	}
}

void CIniHandler::CheckAccountTable()
{
	char  sql[MAX_PATH];
	SetTableExists(false);
	sprintf(sql, "SELECT * FROM sqlite_master WHERE name ='ACCOUNT' and type='table';");
	rc = sqlite3_exec(db, sql, Table_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
		if (GetTableExists() == true)
		{
			// do nothing.
		}
		else
		{
			// create table.
			sprintf(sql, "CREATE TABLE ACCOUNT(USERID INTEGER PRIMARY KEY AUTOINCREMENT, FIRSTNAME TEXT NOT NULL ,LASTNAME TEXT NOT NULL, PHONE TEXT NOT NULL, EMAIL TEXT NOT NULL, ADDRESS TEXT NOT NULL,USERNAME TEXT NOT NULL, PASSWORD TEXT NOT NULL,ACCESS INTEGER);");
			rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			else
			{
				fprintf(stderr, "SQL table creation successful.\n");
				// Add default keys and values:
				std::string input = "admin.admin";
				SHA1 checksum;
				checksum.update(input);
				std::string hash = checksum.final();
				//cout << "The SHA-1 of \"" << input << "\" is: " << hash << endl;
				sprintf(sql, "INSERT INTO ACCOUNT(FIRSTNAME,LASTNAME,PHONE,EMAIL,ADDRESS,USERNAME,PASSWORD,ACCESS)VALUES('admin','admin','00000000000','a@b.com','Earth Planet','admin','%s',0xffff);",hash.c_str());
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				/*
				std::string input1 = "madmin.madmin";
				SHA1 checksum1;
				checksum1.update(input1);
				std::string hash1 = checksum1.final();
				sprintf(sql, "INSERT INTO ACCOUNT(FIRSTNAME,LASTNAME,PHONE,EMAIL,ADDRESS,USERNAME,PASSWORD,ACCESS)VALUES('madmin','madmin','00000000000','a@b.com','Earth Planet','admin','%s',0xffff);", hash1.c_str());
				rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				*/
			}
		}
	}
}

int CIniHandler::CheckIfUserExists(std::string userName, std::string password)
{
	//USES_CONVERSION;
	char sql[512];
	//CString result = userName + "." + password;
	std::string input = userName + "." + password; // T2A(result);
	SHA1 checksum;
	checksum.update(input);
	std::string hash = checksum.final();
	SetUserExists(false);
	sprintf(sql, "SELECT * FROM ACCOUNT WHERE USERNAME = '%s' and PASSWORD = '%s';", userName.c_str(), hash.c_str());
	rc = sqlite3_exec(db, sql, select_callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	if (GetUserExists())
	{
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}
}

void CIniHandler::AddUsertoList(int argc, char ** argv, char ** azColName)
{
	int userIdx			=	-1;
	int firstNameIdx	=	-1;
	int lastNameIdx		=	-1;
	int phoneIdx		=	-1;
	int emailIdx		=	-1;
	int addressIdx		=	-1;
	int userNameIdx		=	-1;
	int	userMapIdx;
	USES_CONVERSION;
	std::string stemp;
	if (argc)
	{
		for (int i = 0; i < argc; i++)
		{
			stemp = azColName[i];
			if (stemp.compare("USERID") == 0)
			{
				userIdx = i;
			}
			else if (stemp.compare("FIRSTNAME") == 0)
			{
				firstNameIdx = i;
			}
			else if (stemp.compare("LASTNAME") == 0)
			{
				lastNameIdx = i;
			}
			else if (stemp.compare("PHONE") == 0)
			{
				phoneIdx = i;
			}
			else if (stemp.compare("EMAIL") == 0)
			{
				emailIdx = i;
			}
			else if (stemp.compare("ADDRESS") == 0)
			{
				addressIdx = i;
			}
			else if (stemp.compare("USERNAME") == 0)
			{
				userNameIdx = i;
			}
		}
		////////////////////////////////////////////////////////////
		if ( userIdx >= 0 )
		{
			stemp		= argv[userIdx];

			userMapIdx	= atoi(stemp.c_str());

			m_userIdMap[userMapIdx] = userMapIdx;

			if (firstNameIdx >= 0)
			{
				m_firstNameMap[userMapIdx]	= A2T(argv[firstNameIdx]);
			}
			if (lastNameIdx >= 0)
			{
				m_lastNameMap[userMapIdx]	= A2T(argv[lastNameIdx]);
			}
			if (phoneIdx >= 0)
			{
				m_phoneMap[userMapIdx] = argv[phoneIdx];
			}
			if (emailIdx >= 0)
			{
				m_emailMap[userMapIdx] = argv[emailIdx];
			}
			if (addressIdx)
			{
				m_addressMap[userMapIdx] = A2T(argv[addressIdx]);
			}
			if (userNameIdx)
			{
				m_userNameMap[userMapIdx] = argv[userNameIdx];
			}
		}
	}
}

void CIniHandler::ClearMaps()
{
	m_userIdMap.clear();
	m_firstNameMap.clear();
	m_lastNameMap.clear();
	m_phoneMap.clear();
	m_emailMap.clear();
	m_addressMap.clear();
	m_userNameMap.clear();
}

int CIniHandler::GetUserNumber()
{
	return m_userIdMap.size();
}

int CIniHandler::CountUsers()
{
	USES_CONVERSION;
	char   sql[512];
	m_userCounter = 0;
	sprintf(sql, "SELECT * FROM ACCOUNT;");
	rc = sqlite3_exec(db, sql, add_users_callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return m_userCounter;
}

int CIniHandler::DeleteUser(int idx)
{
	char sql[512];
	//check if user count is more than 2. 
	if (CountUsers() > 1)
	{
		sprintf(sql, "DELETE FROM ACCOUNT WHERE USERID = '%d';", idx);
		rc = sqlite3_exec(db, sql, select_callback, (void*)this, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			return FAILURE;
		}
		return SUCCESS;
	}
	else
	{
		return USER_DELETE_NUMBER_ERROR;
	}
}

void CIniHandler::AddUserCounter()
{
	m_userCounter++;
}

int CIniHandler::GetUser(int idx)
{
	USES_CONVERSION;
	char	sql[512];
	ClearMaps();
	sprintf(sql, "SELECT * FROM ACCOUNT where USERID='%d';",idx);
	rc = sqlite3_exec(db, sql, get_edit_callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return SUCCESS;
}

void CIniHandler::SetUserEditData(int argc, char ** argv, char ** azColName)
{
	USES_CONVERSION;
	int userIdx			= -1;
	int firstNameIdx	= -1;
	int lastNameIdx		= -1;
	int phoneIdx		= -1;
	int emailIdx		= -1;
	int addressIdx		= -1;
	int userNameIdx		= -1;
	int passwordIdx		= -1;

	m_editUserId	= -1;
	m_editFirstName	= L"";
	m_editLastName	= L"";
	m_editPhone		= "";
	m_editEmail		= "";
	m_editAddress	= L"";
	m_editUserName	= "";
	m_editPassword  = "";

	std::string stemp;
	if (argc)
	{
		for (int i = 0; i < argc; i++)
		{
			stemp = azColName[i];
			if (stemp.compare("USERID") == 0)
			{
				userIdx = i;
			}
			else if (stemp.compare("FIRSTNAME") == 0)
			{
				firstNameIdx = i;
			}
			else if (stemp.compare("LASTNAME") == 0)
			{
				lastNameIdx = i;
			}
			else if (stemp.compare("PHONE") == 0)
			{
				phoneIdx = i;
			}
			else if (stemp.compare("EMAIL") == 0)
			{
				emailIdx = i;
			}
			else if (stemp.compare("ADDRESS") == 0)
			{
				addressIdx = i;
			}
			else if (stemp.compare("USERNAME") == 0)
			{
				userNameIdx = i;
			}
			else if (stemp.compare("PASSWORD") == 0)
			{
				passwordIdx = i;
			}
		}
		////////////////////////////////////////////////////////////
		if (userIdx>=0)
		{
			stemp = argv[userIdx];

			m_editUserId = atoi(stemp.c_str());

			if (firstNameIdx>=0)
			{
				m_editFirstName = A2T(argv[firstNameIdx]);
			}
			if (lastNameIdx>=0)
			{
				m_editLastName = A2T(argv[lastNameIdx]);
			}
			if (phoneIdx>=0)
			{
				m_editPhone = argv[phoneIdx];
			}
			if (emailIdx>=0)
			{
				m_editEmail = argv[emailIdx];
			}
			if (addressIdx>=0)
			{
				m_editAddress = A2T(argv[addressIdx]);
			}
			if (userNameIdx>=0)
			{
				m_editUserName = argv[userNameIdx];
			}
			if (passwordIdx>=0)
			{
				m_editPassword = argv[passwordIdx];
			}
		}
	}
}

int CIniHandler::UpdateUser(std::string USERID, std::wstring FIRSTNAME, std::wstring LASTNAME, std::string PHONE, std::string EMAIL, std::wstring ADDRESS, std::string USERNAME, std::string PASSWORD, int ACCESS)
{
	char  sql[MAX_PATH*16];
	USES_CONVERSION;
	sprintf(sql, "insert or replace into ACCOUNT (USERID , FIRSTNAME, LASTNAME, PHONE, EMAIL, ADDRESS, USERNAME, PASSWORD , ACCESS ) values('%s','%s','%s','%s','%s','%s','%s','%s',%d);", USERID.c_str(), T2A(FIRSTNAME.c_str()), T2A(LASTNAME.c_str()), PHONE.c_str(), EMAIL.c_str(), T2A(ADDRESS.c_str()), USERNAME.c_str(), PASSWORD.c_str(), ACCESS);
	rc = sqlite3_exec(db, sql, key_Check_Callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return SUCCESS;
}

int CIniHandler::AddUser(std::wstring firstName, std::wstring lastName, std::string phone, std::string email, std::wstring address, std::string userName, std::string finalPassword, int access)
{
	char  sql[MAX_PATH*16];
	USES_CONVERSION;
	sprintf(
			sql, 
			"INSERT INTO ACCOUNT(FIRSTNAME,LASTNAME,PHONE,EMAIL,ADDRESS,USERNAME,PASSWORD,ACCESS)VALUES('%s','%s','%s','%s','%s','%s','%s',%d);", 
			T2A(firstName.c_str()), 
			T2A(lastName.c_str()), 
			phone.c_str(), 
			email.c_str(), 
			T2A(address.c_str()), 
			userName.c_str(), 
			finalPassword.c_str(), 
			access
		   );

	rc = sqlite3_exec(db, sql, NULL, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return SUCCESS;
}

int CIniHandler::SearchUsers()
{
	USES_CONVERSION;
	char	sql[512];
	ClearMaps();
	sprintf(sql, "SELECT * FROM ACCOUNT;");
	rc = sqlite3_exec(db, sql, select_users_callback, (void*)this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return FAILURE;
	}
	return SUCCESS;
}