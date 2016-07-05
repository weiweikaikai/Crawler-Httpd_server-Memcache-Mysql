#include "sql_connect.h"

sql_connecter::sql_connecter(const std::string &_host,\
		const std::string &_user,\
		const std::string &_passwd,\
		const std::string &_db)
{
	this->mysql_base = mysql_init(NULL);
	this->res = NULL;
	this->host = _host;
	this->user = _user;
	this->passwd = _passwd;
	this->db   = _db;
}

bool sql_connecter::begin_connect()
{
	if( mysql_real_connect(mysql_base, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 3306, NULL, 0) == NULL){
	//std::cerr<<"connect error"<<std::endl;
		return false;
	}else{
		//std::cout<<"connect done..."<<std::endl;
	}
	return true;
}

bool sql_connecter::close_connect()
{
	mysql_close(mysql_base);
	//std::cout<<"connect close..."<<std::endl;
    return true;
}

bool sql_connecter::select_sql(std::string &field_name,std::string &data,std::string &cityname)
{
	std::string sql = "SELECT info from tianqi where cityname='";
    sql+=cityname;
    sql+="'";
    //std::cout<<sql<<std::endl;
	if(mysql_query(mysql_base, sql.c_str()) == 0)
	{
	   // std::cout<<"find query success!"<<std::endl;
	}
	else
	{
	   // std::cerr<<"find query failed!"<<std::endl;
	}
	res = mysql_store_result(mysql_base);
	int row_num = mysql_num_rows(res);
	int field_num = mysql_num_fields(res);
     //std::cout<<row_num<<field_num<<std::endl;
	MYSQL_FIELD *fd = NULL;
	fd = mysql_fetch_field(res); 
	field_name = fd->name;

	MYSQL_ROW row= mysql_fetch_row(res);
        if(row)
        {
        // std::cout<<row[0]<<std::endl;
           data = row[0];
	        return true;
        }
        return false;
}

bool sql_connecter::insert_sql(const std::string &data)
{
	std::string sql = "insert tianqi (cityname,info) values ";
	sql += "(";
	sql +=data;
	sql +=");";
  //std::cout<<sql.c_str()<<std::endl;
	if(mysql_query(mysql_base, sql.c_str()) == 0){
	//std::cout<<"insert  success!"<<std::endl;
		return true;
	}else{
//	std::cerr<<"insert failed!"<<std::endl;
		return false;
	}
}

bool sql_connecter:: creat_table()
{
	std::string sql = "create table tianqi (id int primary key auto_increment,cityname varchar(30) NOT NULL,info varchar(2000) NOT NULL)charset utf8";
	if(mysql_query(mysql_base, sql.c_str()) == 0){
	//	std::cout<<"create query success!"<<std::endl;
		return true;
	}else{
	//	std::cerr<<"create query failed!"<<std::endl;
		return false;
	}
   
}
bool  sql_connecter::delete_table()
{
	std::string sql = "drop table tianqi";
    //sql+=table;
	if(mysql_query(mysql_base, sql.c_str()) == 0){
	//	std::cout<<" delete query success!"<<std::endl;
		return true;
	}else{
	//	std::cerr<<"detete query failed!"<<std::endl;
		return false;
	}

}
sql_connecter::~sql_connecter()
{
	close_connect();
	if(res != NULL){
		free(res);
	}
}

void sql_connecter::show_info()
{
	std::cout<<mysql_get_client_info()<<std::endl;
}
