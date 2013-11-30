#include <ipersistence/WorldPersistence.h>
#include <sstream>

#include <debugger.h> 

using namespace core::ipersistence;

//::MAPPING:: Class name, parent class name and relation name (SQL table in case of SQL database)
BEGIN_STORE_TABLE(WorldPersistence, dba::Storeable, "world_table")
	BIND_STR	(WorldPersistence::name,		dba::String,						"name")
	BIND_STR	(WorldPersistence::owner,		dba::String,						"owner")
	BIND_INT	(WorldPersistence::permissions,	dba::Int,							"permissions")
	BIND_COL	(WorldPersistence::entities,	dba::stdList<EntityPersistence>,	"fk_world")
END_STORE_TABLE()
//SQL schema
dba::SQL WorldPersistence::schema(
"CREATE TABLE world_table ("
"  id INT PRIMARY KEY,"
"  name VARCHAR,"
"  owner VARCHAR,"
"  permissions INT"
//"  fk_user INT"
")");

boost::mutex WorldPersistence::m_mutex;

WorldPersistence::WorldPersistence(const std::string &name_, const std::string &owner_, const int &permissions_) : name(name_), owner(owner_), permissions(permissions_)
{
	try 
	{
		ar = MainPersistence::GetArchive();
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

WorldPersistence::~WorldPersistence()
{
}

void WorldPersistence::Changed()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(CHANGED));	//ar->getOStream().put(this); //To save on changes
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

bool WorldPersistence::Load(const int &id)
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		WorldPersistence new_object;
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

bool WorldPersistence::Load(const std::string &name)
{
	try 
	{	bool success = false;
		WorldPersistence new_object;
		{	boost::mutex::scoped_lock lock(m_mutex);
			dba::SQLIStream istream = ar->getIStream();
			std::stringstream wop;
			wop << "name = " << "'" << name << "'" ;
			istream.setWherePart( dba::SQL(wop.str()) ) ;
			istream.open(new_object);
			success = istream.getNext(&new_object);
		}
		if (success)
			return Load(new_object.getId());
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
	return false;
}

void WorldPersistence::Save()
{
	try 
	{	//dba::Storeable::stState state = this->getState();
		boost::mutex::scoped_lock lock(m_mutex);
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void WorldPersistence::Delete()
{
	try 
	{	boost::mutex::scoped_lock lock(m_mutex);
		this->setState(dba::Storeable::stState(DELETED));
		ar->getOStream().put(this);
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

void WorldPersistence::ProcessException(const dba::SQLException& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "SQL Error: " << pEx.what() << std::endl;
	wop << "While executing: " << std::endl << pEx.getQuery() << std::endl;
	error = wop.str();
}

void WorldPersistence::ProcessException(const dba::Exception& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "Error: " << pEx.what() << std::endl;
	error = wop.str();
}

void WorldPersistence::AddEntity(core::IEntityPersistence &entity)
{	
	entity.Save();
	{	boost::mutex::scoped_lock lock(m_mutex);
		//core::ipersistence::EntityPersistence new_entity(entity.GetName());
		entities.push_back((*(core::ipersistence::EntityPersistence*)&entity)); 
	}
	Changed();	
}

void WorldPersistence::RemoveEntity(core::IEntityPersistence &entity)
{	
	entity.Save();
	{	boost::mutex::scoped_lock lock(m_mutex);
		std::list<core::ipersistence::EntityPersistence>::iterator iter = entities.begin();
		core::ipersistence::EntityPersistence* entity_to_delete = (core::ipersistence::EntityPersistence*)&entity;
		unsigned int index = 0;
		bool found = false;
		for ( ; ( entities.size() > 0 ) && (index < entities.size()) && !found ; index++)
		{	core::ipersistence::EntityPersistence* iter_entity = (core::ipersistence::EntityPersistence*)&(*iter);
			found = ( iter_entity->getId() == entity_to_delete->getId() );
			if (!found) iter++;
		}
		if ( iter != entities.end() ) 
			entities.erase(iter); 
	}
	Changed();	
}

core::IEntityPersistence* WorldPersistence::GetEntity(const int &i)		
{	boost::mutex::scoped_lock lock(m_mutex);
	core::IEntityPersistence* result = NULL;
	std::list<core::ipersistence::EntityPersistence>::iterator iter = entities.begin();
	unsigned int index = 0;
	for ( ; (index != i) && (index < entities.size() ) ; index++)
		iter++;
	if (index == i)
		result = (core::IEntityPersistence*)&(*iter);
	return result;
}