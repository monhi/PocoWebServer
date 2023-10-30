#include "Db.h"
#include "RegReader.h"
#include <time.h>
#include <vector>
#include <Shlwapi.h>
#include "base64.h"

CDb::CDb()
{
	m_pCon = nullptr;
	m_recordsTableName = "Records";
	m_usersTableName = "Users";
	m_dbOpen = false;
}

CDb::~CDb()
{
	Close();
}

int CDb::Close()
{
	if (m_dbOpen)
	{
		mysql_close(m_pCon);
		m_dbOpen = false;
		m_pCon = nullptr;
	}	
	return SUCCESS;
}

int CDb::Connect()
{
	CConfigReader* m_pRegReader = CConfigReader::GetInstance();
	m_username = m_pRegReader->m_db_username;
	m_password = m_pRegReader->m_db_password;
	m_pCon = mysql_init(NULL);
	if (mysql_real_connect(m_pCon, "localhost", m_username.c_str(), m_password.c_str(), NULL, 0, NULL, 0) == NULL)
	{
		color_printf(RED, "MySQL error : %s \n", mysql_error(m_pCon));
		return FAILURE;
	}
	m_dbOpen = true;
	return SUCCESS;
}

int CDb::CheckDataBase()
{
	char *sql = "CREATE DATABASE IF NOT EXISTS TR4DB";
	char *sql2 = "USE TR4DB";
	if (mysql_query(m_pCon, sql))
	{
		color_printf(RED, "CheckDataBase error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	
	if (mysql_query(m_pCon, sql2))
	{
		color_printf(RED, "CheckDataBase error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	// Select database as active database.
	return SUCCESS;
}

int CDb::CheckTable()
{
	my_ulonglong tableExists;
	MYSQL_RES *result;
	std::ostringstream sql;
	sql << "SHOW TABLES LIKE '" << m_recordsTableName.c_str() << "'" ;

	if (mysql_query(m_pCon, sql.str().c_str()))
	{
		color_printf(RED, "CheckTable error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	result = mysql_store_result(m_pCon);

	tableExists = mysql_num_rows(result) ;
	if (tableExists)
	{
		// return SUCCESS;
		// Do nothing.
	}
	else
	{
		sql.str(std::string());
		sql << "CREATE TABLE " << m_recordsTableName.c_str() << "(Id INT PRIMARY KEY AUTO_INCREMENT,\
																dir TINYINT not null,\
																answered TINYINT not null,\
																channel TINYINT not null,\
																duration SMALLINT not null,\
																path nvarchar(512) not null,\
																epoch int(11) not null,\
																dial varchar(64),\
																cid varchar(64),\
																description nvarchar(255))";
		if (mysql_query(m_pCon, sql.str().c_str()))
		{
			color_printf(RED, "CheckTable error : %s\n", mysql_error(m_pCon));
			return FAILURE;
		}
	}

	/*
	// Creating user table
	sql.str(std::string());
	sql << "SHOW TABLES LIKE '" << m_usersTableName.c_str() << "'";

	if (mysql_query(m_pCon, sql.str().c_str()))
	{
		color_printf(RED, "CheckTable error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	result = mysql_store_result(m_pCon);

	tableExists = mysql_num_rows(result);
	if (tableExists)
	{
		return SUCCESS;
	}
	sql.str(std::string());
	sql << "CREATE TABLE " << m_usersTableName.c_str() << "(Id INT PRIMARY KEY AUTO_INCREMENT,user nvarchar(64) not null,password nvarchar(64) not null,permissions INT not null,rights INT not null)";

	if (mysql_query(m_pCon, sql.str().c_str()))
	{
		color_printf(RED, "CheckTable error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	// Add admin admin user name and password to the table.
	sql.str(std::string());
	std::string alpha = "admin";
	std::string beta = base64_encode((const unsigned char*)alpha.c_str(), alpha.length());
	sql << "INSERT INTO " << m_usersTableName.c_str() << " (user,password, permissions,rights) VALUES('admin','"<<beta<<"',65535,65535)";

	if (mysql_query(m_pCon,sql.str().c_str()))
	{
		color_printf(RED, "AddRecord error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	*/
	// Create a table in database;
	return SUCCESS;
}

int CDb::EstablishConnection()
{
	if ( Connect() == FAILURE )
		return FAILURE;
	if (CheckDataBase() == FAILURE)
		return FAILURE;
	if (CheckTable() == FAILURE)
		return FAILURE;
	return SUCCESS;
}

/*
int CDb::GetUserNameAndPassword(std::string userName, std::string& password)
{
	bool got = false;
	std::ostringstream sql;
	std::string temp;
	if (CheckDataBase() == FAILURE)
	{
		return FAILURE;
	}
	if (CheckDataBase() == FAILURE)
	{
		return FAILURE;
	}

	//m_records.clear();
	sql << "SELECT password FROM " << m_usersTableName.c_str() << " where user = \"" << userName<< "\";";
	temp = sql.str();
	if (mysql_query(m_pCon, temp.c_str()))
	{
		color_printf(RED, "user name SELECT error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	// get result and show it.
	MYSQL_RES *result = mysql_store_result(m_pCon);

	if (result == NULL)
	{
		color_printf(RED, "GetOutOfDateRecords MYSQL_RES error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}

	int num_fields = mysql_num_fields(result);

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(result)))
	{
		password = (row[0]);
		got = true;

	}
	mysql_free_result(result);
	if (got)
	{
		return SUCCESS;
	}
	return FAILURE;
}
*/

int CDb::test()
{
	if (EstablishConnection() == SUCCESS)
	{
		AddRecord(time(NULL)-(48*3600), 0, true, true, 1200, "321546988774", "", "c:\\testUnit\\221.wav", "for test only");
		GetOutOfDateRecords(0, 1);
		Close();
		return SUCCESS;
	}
	return FAILURE;
}



int	CDb::AddRecord(time_t epoch,int channel, int dir, int answered, int duration, char* dial, char* cid, char* path, char* description)
{
	std::ostringstream sql;
	std::string temp;
	std::string stemp = "";
	for ( size_t i = 0; i < strlen(path); i++ )
	{
		if (path[i] != '\\')
		{
			stemp += path[i];
		}
		else
		{
			stemp +=path[i];
			stemp +=path[i];
		}
	}

	sql << "INSERT INTO " << m_recordsTableName.c_str() << "(channel,dir , duration ,answered ,dial ,cid , epoch ,path, description ) VALUES("\
		<< channel << "," << dir << "," << duration << "," << answered << ",'" << dial << "','" << cid << "','" << epoch << "','" << stemp << "','" << description << "')";
	temp = sql.str();

	if (mysql_query(m_pCon, temp.c_str()))
	{
		color_printf(RED, "AddRecord error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	return SUCCESS;
}


int CDb::DeleteRecordFile(const char* path)
{
	BOOL		ret;
	std::string stemp = path;
	std::size_t found = stemp.find_last_of("/\\");
	stemp = stemp.substr(0, found + 1);
	int retVal;

	if (!DeleteFileA(path))
	{
		int res = GetLastError();
		if ((res == ERROR_PATH_NOT_FOUND) || (res == ERROR_INVALID_NAME) || (res == ERROR_FILE_NOT_FOUND))
		{
			// Try to delete path here also.
			retVal = SUCCESS;
			// do nothing.

		}
		else
		{
			retVal = FAILURE;
		}
	}
	else
	{
		retVal = SUCCESS;
	}

	ret = PathIsDirectoryEmptyA(stemp.c_str());
	if (ret)
	{
		RemoveDirectoryA(stemp.c_str());		
	}
	return retVal;
}

int CDb::RemoveRecord(int id,const char* path)
{
	std::ostringstream	sql;
	std::string			temp;
	if (DeleteRecordFile(path) == SUCCESS)
	{
		// delete record from database.
		sql << "DELETE FROM " << m_recordsTableName.c_str() << " where id = " << id << ";";
		temp = sql.str();
		if (mysql_query(m_pCon, temp.c_str()))
		{
			color_printf(RED, "RemoveRecord DELETE error : %s\n", mysql_error(m_pCon));
			return FAILURE;
		}
		return SUCCESS;
	}	
	return FAILURE;
}

int	CDb::SearchRecords(int ch, time_t epochStart, time_t epochEnd, int dir, int answered, int startDuration, int endDuration)
{
	std::ostringstream	sql;
	std::string			temp;
	char				temp0[128];
	char				temp1[128];
	char				temp2[128];
	char				temp3[128];
	char				temp4[128];
	int					record_cntr = 0;

	if (Connect() == FAILURE)
	{
		return FAILURE;
	}

	if (CheckDataBase() == FAILURE)
	{
		return FAILURE;
	}

	//sql << "SELECT id,dir,answered,channel,duration,dial,cid,path,epoch FROM " << m_recordsTableName.c_str() << " where"; channel = " << channel << " AND epoch < " << t << "; ";
	if (ch != -1)
	{
		sprintf(temp0, "channel = %d AND", ch); 
	}
	else
	{
		sprintf(temp0, " ", ch);
	}

	sprintf(temp1, " epoch < %lld AND epoch > %lld ", (long long)epochEnd, (long long)epochStart);

	if (dir != -1)
	{
		sprintf(temp2, " AND dir = %d ", dir);
	}
	else
	{
		sprintf(temp2, " ");
	}

	if (answered != -1)
	{
		sprintf(temp3, " AND answered = %d  ", answered);
	}
	else
	{
		sprintf(temp3, " ", answered);
	}

	if (startDuration > 0 || endDuration > 0)
	{
		if ( endDuration <= 0 )
		{
			endDuration = 1200;
		}
		sprintf(temp4, " AND duration > %d AND duration < %d ", startDuration,endDuration);
	}
	else
	{
		sprintf(temp4, " ");
	}

	sql << "SELECT id,dir,answered,channel,duration,dial,cid,path,epoch FROM " << m_recordsTableName.c_str() << " where " << temp0 << temp1 << temp2 << temp3 << temp4 <<" order by epoch desc;";

	temp = sql.str();
	if (mysql_query(m_pCon, temp.c_str()))
	{
		color_printf(RED, "GetOutOfDateRecords SELECT error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	// get result and show it.
	MYSQL_RES *result = mysql_store_result(m_pCon);

	if (result == NULL)
	{
		color_printf(RED, "GetOutOfDateRecords MYSQL_RES error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}

	int num_fields = mysql_num_fields(result);

	MYSQL_ROW row;
	search_t  search;
	m_searchResult.empty();
	record_cntr = 0;
	while (row = mysql_fetch_row(result))
	{
		//rec.id = atoi(row[0]);
		//rec.path = row[1];
		search.id			= atoi(row[0]);
		search.dir			= atoi(row[1]);
		search.answered		= atoi(row[2]);
		search.channel		= atoi(row[3]);
		search.duration		= atoi(row[4]);
		search.dial			= row[5];
		search.cid			= row[6];
		search.path			= row[7];
		search.epoch		= atoi(row[8]);
		if (
			(search.dial == "")
			&&
			(search.cid == "")
			&&
			(search.duration < 10)
		   )
		{
			// do nothing.
		}
		else
		{
			m_searchResult.push(search);
			record_cntr++;
		}
	}
	mysql_free_result(result);
	Close();
	return record_cntr;
}

int CDb::GetSearchResult(search_t &search)
{
	if (m_searchResult.size()>0)
	{
		search = m_searchResult.front();
		m_searchResult.pop();
		return SUCCESS;
	}
	return FAILURE;
}


int CDb::GetOutOfDateRecords(int channel,int retentionDays)
{
	record_t rec;
	std::ostringstream sql;
	std::string temp;
	time_t t = time(NULL);
	t -= (retentionDays * 3600 * 24);
	if ( Connect() == FAILURE )
	{
		return FAILURE;
	}

	if ( CheckDataBase() == FAILURE )
	{
		return FAILURE;
	}
	//m_records.clear();
	sql << "SELECT id,path FROM " << m_recordsTableName.c_str() << " where channel = " << channel << " AND epoch < "<< t << ";";
	temp = sql.str();
	if (mysql_query(m_pCon, temp.c_str()))
	{
		color_printf(RED, "GetOutOfDateRecords SELECT error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}
	// get result and show it.
	MYSQL_RES *result = mysql_store_result(m_pCon);

	if (result == NULL)
	{
		color_printf(RED, "GetOutOfDateRecords MYSQL_RES error : %s\n", mysql_error(m_pCon));
		return FAILURE;
	}

	int num_fields = mysql_num_fields(result);

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(result)))
	{
		rec.id = atoi(row[0]);
		rec.path = row[1];
		//m_records.push_back(rec);
		/*
		for (int i = 0; i < num_fields; i++)
		{
			printf("%s ", row[i] ? row[i] : "NULL");
		}
		printf("\n");
		*/
		RemoveRecord(rec.id, rec.path.c_str());
	}
	mysql_free_result(result);
	Close();
	return SUCCESS;
}

