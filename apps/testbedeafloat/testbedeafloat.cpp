// testbedeafloat.cpp : main project file.
#include <dba/dba.h>
#include <dba/double_filter.h>

using namespace System;

class Entity : public dba::Storeable
{
	DECLARE_STORE_TABLE();

	public:

		Entity(const std::string &name_, const float &value) : var(value), name(name_) {}
		~Entity() {}

	private:

		float var;
		std::string name;
};

//::MAPPING:: Class name, parent class name and relation name (SQL table in case of SQL database)
BEGIN_STORE_TABLE(Entity, dba::Storeable, "entity_table")
	BIND_STR(Entity::name, dba::String, "name" )
	BIND_FLT(Entity::var, dba::Float, "var"	)
END_STORE_TABLE()

//SQL schema
dba::SQL Entity_schema(
"CREATE TABLE entity_table ("
"  id INT PRIMARY KEY,"
"  name VARCHAR,"
"  var FLOAT"
")");

//begin of SQL schema 
dba::SQL counter_create(
"CREATE TABLE debea_object_count ("
"  id INT"
")");


int main(array<System::String ^> ^args)
{
	try 
	{
		Console::WriteLine(L"Hello World");

		dba::SQLArchive ar;

		ar.setIdFetcher(new dba::GenericFetcher()); //for assigning id numbers. Using generic one that needs special SQL table debea_object_count. 
		unlink("voxDB"); 		                    //backend creates new empty database if there is no file
		ar.open("dbapgsql-static", "dbname=voxDB user=vox password=vox"); //load database plugin & open existing database //ar.initPlugin("dbapgsql-static");

		//create needed tables sending simple SQL
		ar.getOStream().sendUpdate(counter_create);
		ar.getOStream().sendUpdate(Entity_schema);

		//first id //
		ar.getOStream().sendUpdate(dba::SQL("INSERT INTO debea_object_count VALUES (:d)") << 1);

		float n = 12.34f;
		Entity *my_entity = new Entity("dummy", n);
		my_entity;
		ar.getOStream().put(my_entity);
	}
	catch (const dba::SQLException& pEx)	
	{ 
		std::cout << "SQL Error: " << pEx.what() << std::endl;
		std::cout << "While executing: " << std::endl << pEx.getQuery() << std::endl;
	}
	catch (const dba::Exception& pEx)		
	{ 
		std::cout << "Error: " << pEx.what() << std::endl;
	}


    return 0;
}
