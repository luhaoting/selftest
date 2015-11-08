/*
 * log.h
 *
 *  Created on: 2014Äê7ÔÂ12ÈÕ
 *      Author: luhaoting
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include<fstream>
#include <cstring>


namespace Log {

class CLog
{
public:
	static const int MAX_LEN = 1024 * 1024 * 10;  //10M
	static CLog& instance();
	virtual ~CLog();

	void setLogFileName(std::string name);
	std::string getLogFileName();
	void log(std::string );

private:
	CLog();
	std::string m_FileName;

};
	void DebugLog(const char* log);
	void NetLog(const char* log);
	void DataLog(const char* log);
} /* namespace log */

#endif /* LOG_H_ */
