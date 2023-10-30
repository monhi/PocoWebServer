#pragma once
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <queue>

struct record_t
{
	int		id;
	std::string path;
};

struct search_t
{
	int			id;
	int			dir; 
	int			answered;
	int			channel;
	int			duration;
	std::string dial; 
	std::string cid;
	std::string path;
	time_t		epoch;
};

class CDb
{
private:
	std::string				m_username;
	std::string				m_password;
	std::string				m_recordsTableName;
	std::string				m_usersTableName;
	std::queue<search_t>	m_searchResult;
	MYSQL					*m_pCon;
	bool					m_dbOpen;
public:
							CDb();
							~CDb();
	int						Connect();
	int						CheckDataBase();
	int						CheckTable();
	int						EstablishConnection();
	int						SearchRecords(int ch,time_t epochStart,time_t epochEnd,int dir,int answered,int startDuration,int endDuration);
	int						GetSearchResult(search_t &search);
	int						AddRecord(time_t epoch, int channel, int dir, int answered, int duration, char* dial, char* cid, char* path, char* description);
	int						DeleteRecordFile(const char* path);
	int						RemoveRecord(int id,const char* path);
	int						GetOutOfDateRecords(int channel, int retentionDays);
	int						Close();
	int						test();
};

