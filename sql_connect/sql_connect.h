#ifndef __SQL_CONNECT__
#define __SQL_CONNECT__

#include <iostream>
#include <string>
#include <stdlib.h>
#include "mysql.h"


class sql_connecter{
	public:
		sql_connecter(const std::string &_host, const std::string &_user, const std::string &_passwd, const std::string &_db);
		//connect remote mysql
		bool begin_connect();
		bool insert_sql(const std::string &data);
bool select_sql(std::string &field_name,std::string &data,std::string &cityname);

		//bool updata_sql();
		//bool delete_sql();
		bool delete_table();
		bool creat_table();
		//close remote link();
		bool close_connect();
		~sql_connecter();
		void show_info();
	private:
		MYSQL_RES *res;
		MYSQL *mysql_base;
		std::string host;
		std::string user;
		std::string passwd;
		std::string db;
};

#endif


