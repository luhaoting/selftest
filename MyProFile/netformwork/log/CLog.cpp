/*
 * log.cpp
 *
 *  Created on: 2014Äê7ÔÂ12ÈÕ
 *      Author: luhaoting
 */

#include "CLog.h"
#include <time.h>
#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <direct.h>

namespace Log {

CLog::CLog()
{
	// TODO Auto-generated constructor stub
	_mkdir("./log");
}

CLog& CLog::instance()
{
	static CLog s_log;
	return s_log;
}

void CLog::setLogFileName(std::string name)
{
	m_FileName = name;
}

std::string CLog::getLogFileName()
{
	return m_FileName;
}

void CLog::log(std::string strLog)
{
	if(strLog.empty())
	{
		return;
	}
    struct tm *t;
    time_t tt;
    time(&tt);
    t=localtime(&tt);
    char szDirName[256];

    sprintf(szDirName, "./log/%s%4d-%02d-%02d.LOG", getLogFileName().c_str(), \
    		t->tm_year+1900, t->tm_mon+1, t->tm_mday);

    FILE *fp = fopen(szDirName,"a+");
    if(NULL == fp)
    {
    	return;
    }
    fprintf(fp,"%02d:%02d:%02d-- %s\n", t->tm_hour, t->tm_min, t->tm_sec, strLog.c_str());
    fclose(fp);

    struct _stat info;
    _stat(szDirName, &info);
    int nSize = info.st_size;

    if(nSize > MAX_LEN)
    {	char szNewDirName[256];
    	sprintf(szNewDirName, "./log/%s%4d-%02d-%02d.%02d%02d%02dLOG", getLogFileName().c_str(), \
    	    		t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    	std::rename(szDirName ,szNewDirName);
    }
}

CLog::~CLog()
{
	// TODO Auto-generated destructor stub
}

void DebugLog(const char* log)
{
#ifdef _DEBUG
	CLog::instance().setLogFileName("debug");
	CLog::instance().log(log);
#endif
}

void NetLog(const char* log)
{
	CLog::instance().setLogFileName("net");
	CLog::instance().log(log);
}

void DataLog(const char* log)
{
	CLog::instance().setLogFileName("data");
	CLog::instance().log(log);
}

} /* namespace log */
