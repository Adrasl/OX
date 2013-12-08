#include <ipersistence/EntityPersistence.h>
#include <ipersistence/vdouble_filter.h>
#include <sstream>

#include <dba/dba.h>
#include <dba/double_filter.h>

#include <debugger.h> 

using namespace core::ipersistence;

//::MAPPING:: Class name, parent class name and relation name (SQL table in case of SQL database)
BEGIN_STORE_TABLE(EntityPersistence, dba::Storeable, "entity_table")
	BIND_STR(EntityPersistence::name,					dba::String,		"name"			)
	BIND_FLT(EntityPersistence::position_x,				dba::vFloat,		"position_x"	)
	BIND_FLT(EntityPersistence::position_y,				dba::vFloat,		"position_y"	)
	BIND_FLT(EntityPersistence::position_z,				dba::vFloat,		"position_z"	)
	BIND_FLT(EntityPersistence::orientation_x,			dba::vFloat,		"orientation_x"	)
	BIND_FLT(EntityPersistence::orientation_y,			dba::vFloat,		"orientation_y"	)
	BIND_FLT(EntityPersistence::orientation_z,			dba::vFloat,		"orientation_z"	)
	BIND_FLT(EntityPersistence::up_x,					dba::vFloat,		"up_x"			)
	BIND_FLT(EntityPersistence::up_y,					dba::vFloat,		"up_y"			)
	BIND_FLT(EntityPersistence::up_z,					dba::vFloat,		"up_z"			)
	BIND_FLT(EntityPersistence::scale,					dba::vFloat,		"scale"			)
	BIND_STR(EntityPersistence::modeldata,				dba::String,	"modeldata"			)
	BIND_STR(EntityPersistence::sounddata_create,		dba::String,	"sounddata_create"	)
	BIND_STR(EntityPersistence::sounddata_destroy,		dba::String,	"sounddata_destroy"	)
	BIND_STR(EntityPersistence::sounddata_touch,		dba::String,	"sounddata_touch"	)
	BIND_STR(EntityPersistence::sounddata_idle,			dba::String,	"sounddata_idle"	)
	BIND_INT(EntityPersistence::psique,					dba::Int,			"psique"		)
	BIND_INT(EntityPersistence::collidable,				dba::Int,			"collidable"	)
	BIND_FLT(EntityPersistence::time_to_live,			dba::vFloat,		"timetolive"	)
END_STORE_TABLE()

//SQL schema
dba::SQL EntityPersistence::schema(
"CREATE TABLE entity_table ("
"  id INT PRIMARY KEY,"
"  name VARCHAR,"
"  position_x FLOAT,"
"  position_y FLOAT,"
"  position_z FLOAT,"
"  orientation_x FLOAT,"
"  orientation_y FLOAT,"
"  orientation_z FLOAT,"
"  up_x FLOAT,"
"  up_y FLOAT,"
"  up_z FLOAT,"
"  scale FLOAT,"
"  modeldata VARCHAR,"
"  sounddata_create VARCHAR,"
"  sounddata_destroy VARCHAR,"
"  sounddata_touch VARCHAR,"
"  sounddata_idle VARCHAR,"
"  psique INT,"
"  collidable INT,"
"  timetolive FLOAT,"
"  fk_world INT"
")");

boost::mutex EntityPersistence::m_mutex;

EntityPersistence::EntityPersistence(const std::string &name_) : name(name_), psique(0),
				  position_x(0.0f), position_y(0.0f), position_z(0.0f), 
				  orientation_x(0.0f), orientation_y(0.0f), orientation_z(0.0f),
				  up_x(0.0f), up_y(0.0f), up_z(1.0f),
				  scale(1.0f), collidable(0), time_to_live(0.0f),
                  modeldata(""), sounddata_create(""), sounddata_destroy(""), sounddata_touch(""), sounddata_idle("")
{
	try 
	{
		ar = MainPersistence::GetArchive();
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

EntityPersistence::~EntityPersistence()
{
}

void EntityPersistence::Changed()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(CHANGED));	//TO SAVE ON CHANGE :: ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void EntityPersistence::Load(const int &id)
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		EntityPersistence new_object;
		dba::SQLIStream istream = ar->getIStream();
		istream.setWhereId(id);
		istream.get(&new_object);
		this->operator =(new_object);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void EntityPersistence::Load(const std::string &name)
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		EntityPersistence new_object;
		dba::SQLIStream istream = ar->getIStream();
		std::stringstream wop;
		wop << "name = " << "'" << name << "'" ;
		istream.setWherePart( dba::SQL(wop.str()) ) ;
		istream.open(new_object);
		istream.getNext(&new_object);
		this->operator =(new_object);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void EntityPersistence::Save()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		dba::Storeable::stState state = this->getState();
		//ar->getOStream().put(this);
		//ar->getOStream().commit();
		//ar->getOutputStream()->bind("entity_table", "scale", StoreableFilter<&scale>, dba::Database::StoreType::FLOAT)
		//ar->getOutputStream()->bind("entity_table", "scale", dba::Float, dba::Database::StoreType::FLOAT)
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void EntityPersistence::Delete()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(DELETED));
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void EntityPersistence::ProcessException(const dba::SQLException& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "SQL Error: " << pEx.what() << std::endl;
	wop << "While executing: " << std::endl << pEx.getQuery() << std::endl;
	error = wop.str();
}

void EntityPersistence::ProcessException(const dba::Exception& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "Error: " << pEx.what() << std::endl;
	error = wop.str();
}