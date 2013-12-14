#include <ipersistence/UserPersistence.h>
#include <sstream>
#include <dba/double_filter.h>

#include <debugger.h> 

using namespace core::ipersistence;

//::MAPPING:: Class name, parent class name and relation name (SQL table in case of SQL database)
BEGIN_STORE_TABLE(UserPersistence, dba::Storeable, "user_table")
	BIND_STR(UserPersistence::name,				dba::String,		"name"			)
	BIND_STR(UserPersistence::password,			dba::String,		"password"		)
	BIND_STR(UserPersistence::model,			dba::String,		"model"		)
	BIND_INT(UserPersistence::psique,			dba::Int,			"psique"		)
	BIND_INT(UserPersistence::permissions,		dba::Int,			"permissions"	)
	BIND_FLT(UserPersistence::position_x,		dba::Float,			"position_x"	)
	BIND_FLT(UserPersistence::position_y,		dba::Float,			"position_y"	)
	BIND_FLT(UserPersistence::position_z,		dba::Float,			"position_z"	)
	BIND_FLT(UserPersistence::orientation_x,	dba::Float,			"orientation_x"	)
	BIND_FLT(UserPersistence::orientation_y,	dba::Float,			"orientation_y"	)
	BIND_FLT(UserPersistence::orientation_z,	dba::Float,			"orientation_z"	)
	BIND_FLT(UserPersistence::up_x,				dba::Float,			"up_x"			)
	BIND_FLT(UserPersistence::up_y,				dba::Float,			"up_y"			)
	BIND_FLT(UserPersistence::up_z,				dba::Float,			"up_z"			)
	BIND_FLT(UserPersistence::scale,			dba::Float,			"scale"			)
	//BIND_COL(UserPersistence::worlds, dba::stdList<WorldPersistence>, "fk_user")
END_STORE_TABLE()
//SQL schema
dba::SQL UserPersistence::schema(
"CREATE TABLE user_table ("
"  id INT PRIMARY KEY,"
"  name VARCHAR,"
"  password VARCHAR,"
"  model VARCHAR,"
"  psique INT,"
"  permissions INT,"
"  position_x FLOAT,"
"  position_y FLOAT,"
"  position_z FLOAT,"
"  orientation_x FLOAT,"
"  orientation_y FLOAT,"
"  orientation_z FLOAT,"
"  up_x FLOAT,"
"  up_y FLOAT,"
"  up_z FLOAT,"
"  scale FLOAT"
")");

boost::mutex UserPersistence::m_mutex;

UserPersistence::UserPersistence(const std::string &name_, const std::string &password_, const int &permissions_) 
				: name(name_), password(password_), model(""), permissions(permissions_), psique(1),
				  position_x(0.0f), position_y(0.0f), position_z(0.0f), 
				  orientation_x(0.0f), orientation_y(0.0f), orientation_z(0.0f),
				  up_x(0.0f), up_y(0.0f), up_z(1.0f),
				  scale(1.0f)
{
	try 
	{
		ar = MainPersistence::GetArchive();
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

UserPersistence::~UserPersistence()
{
}

void UserPersistence::Changed()
{
	try 
	{
		boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(CHANGED));	//ar->getOStream().put(this); //To save on changes
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

bool UserPersistence::Load(const int &id)
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		UserPersistence new_object;
		dba::SQLIStream istream = ar->getIStream();
		istream.setWhereId(id);
		//istream.open(new_object);
		bool success = istream.get(&new_object);
		this->operator =(new_object);
		return success;
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
	return false;
}

bool UserPersistence::Load(const std::string &name)
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		UserPersistence new_object;
		dba::SQLIStream istream = ar->getIStream();
		std::stringstream wop;
		wop << "name = " << "'" << name << "'" ;
		istream.setWherePart( dba::SQL(wop.str()) ) ;
		istream.open(new_object);
		bool success = istream.getNext(&new_object);
		this->operator =(new_object);
		return success;
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
	return false;
}

void UserPersistence::Save()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		//dba::Storeable::stState state = this->getState();
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void UserPersistence::Delete()
{
	try 
	{
		boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(DELETED));
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void UserPersistence::ProcessException(const dba::SQLException& pEx)
{	boost::mutex::scoped_lock lock(m_mutex);
	std::string error;
	std::stringstream wop;
	wop << "SQL Error: " << pEx.what() << std::endl;
	wop << "While executing: " << std::endl << pEx.getQuery() << std::endl;
	error = wop.str();
}

void UserPersistence::ProcessException(const dba::Exception& pEx)
{	boost::mutex::scoped_lock lock(m_mutex);
	std::string error;
	std::stringstream wop;
	wop << "Error: " << pEx.what() << std::endl;
	error = wop.str();
}