#include <ipersistence/mainpersistence.h>
#include <ipersistence/UserPersistence.h>
#include <core/IApplicationConfiguration.h>
#include <ipersistence/vdouble_filter.h>

#include <debugger.h> 
#include <string>
#include <iostream>

using namespace core;
using namespace core::ipersistence;

dba::SQLArchive MainPersistence::ar;

//begin of SQL schema 
dba::SQL MainPersistence::counter_create(
"CREATE TABLE debea_object_count ("
"  id INT"
")");

MainPersistence::MainPersistence(IApplicationConfiguration *app_config_) : app(NULL), app_config(app_config_)
{
	try 
	{
		//ApplicationConfiguration *app_config = ApplicationConfiguration::GetInstance();
		if (app_config == NULL)
			return;

		std::string db_hostname = app_config->GetSDHost();
		std::string db_port = app_config->GetSDPort();
		std::string db_name = app_config->GetSDName();
		std::string db_user = app_config->GetSDUser();
		std::string db_passwd = app_config->GetSDPassword();

		std::stringstream wop;
		wop << "host=" << db_hostname << " port=" << db_port <<" dbname=" << db_name << " user=" << db_user << " password=" << db_passwd;
		ar.setIdFetcher(new dba::GenericFetcher()); //for assigning id numbers. Using generic one that needs special SQL table debea_object_count. 
		//unlink("voxDB"); 		                    //backend creates new empty database if there is no file
		//ar.open("dbapgsql-static", "dbname=voxDB user=vox password=vox"); //load database plugin & open existing database //ar.initPlugin("dbapgsql-static");
		unlink(db_name.c_str());
		ar.open("dbapgsql-static", wop.str().c_str());


		//dba float filter tests---------------------------------------
		dba::ConvSpec convs = ar.getConversionSpecs();
		convs.mDbCharset;
		convs.mDecimalPoint;
		convs.mDbCharset = dba::ConvSpec::charset();//dba::ConvSpec::UTF8;
		//convs.mDecimalPoint = ',';
		//ar.setConversionSpecs(convs);
		//std::string prueba = dba::Float::toString();
		float  my_float = 19123.9;
		double my_double = 17456.9;
		std::cout << "float: " << my_float << " // double: " << my_double << "\n";
		dba::Float dba_float(my_float);
		dba::Double dba_double(my_double);
		std::string s_float = dba_float.toString(convs);
		std::string s_double = dba_double.toString(convs);
		std::cout << "Decimal point '.' float: " << s_float  <<  " // double: " << s_double << "\n";
		convs.mDecimalPoint = ',';
		std::string s_float2 = dba_float.toString(convs);
		std::string s_double2 = dba_double.toString(convs);
		std::cout << "Decimal point ',' float: " << s_float2 <<  " // double " << s_double2 << "\n";
		//----------------------------------------------------------------
		dba::vFloat dba_v_float(my_float);
		dba::vDouble dba_v_double(my_double);
		convs.mDecimalPoint = '.';
		std::string s_v_float = dba_v_float.toString(convs);
		std::string s_v_double = dba_v_double.toString(convs);
		std::cout << "Decimal point '.' float: " << s_v_float  <<  " // double: " << s_v_double << "\n";
		convs.mDecimalPoint = ',';
		std::string s_v_float2 = dba_v_float.toString(convs);
		std::string s_v_double2 = dba_v_double.toString(convs);
		std::cout << "Decimal point ',' float: " << s_v_float2 <<  " // double " << s_v_double2 << "\n";
		//----------------------------------------------------------------


		//bindedVars(ar);
		//create needed tables sending simple SQL
		ar.getOStream().sendUpdate(counter_create);
		ar.getOStream().sendUpdate((*(UserPersistence::GetSchema())));
		ar.getOStream().sendUpdate((*(EntityPersistence::GetSchema())));
		ar.getOStream().sendUpdate((*(WorldPersistence::GetSchema())));
		//first id //
		ar.getOStream().sendUpdate(dba::SQL("INSERT INTO debea_object_count VALUES (:d)") << 1);

		//printf("Decimal point ,");
		core::ipersistence::EntityPersistence test("stuff");
		test.Save();

		//convs.mDecimalPoint = '.';
		//ar.setConversionSpecs(convs);
		//printf("Decimal point .");
		//core::ipersistence::EntityPersistence test2("stuff2");
		//test2.Save();

		//TESTS-----------------------------------------------------------------------
		core::ipersistence::UserPersistence user_1("caca1", "111");
		core::ipersistence::UserPersistence user_2("caca2", "111");
		core::ipersistence::UserPersistence user_3("caca3", "111");
		core::ipersistence::UserPersistence userLoadid;
		core::ipersistence::UserPersistence userLoadname;
		user_1.Save();
		user_2.Save();
		user_3.Save();
		user_1.SetName("test1");
		user_2.SetName("test2");
		user_1.Save();
		user_2.Save();
		userLoadid.Load(1);
		userLoadname.Load("test2");
		userLoadid.SetPassword("111");
		userLoadname.SetPassword("222");
		userLoadid.Save();
		userLoadname.Save();
		user_3.Delete();

		core::ipersistence::WorldPersistence mundo1("mimundo1", "test1");
		core::ipersistence::WorldPersistence mundo2("mimundo2", "test1");
		core::ipersistence::WorldPersistence mundo3("mimundo3", "test1");
		core::ipersistence::WorldPersistence mundoLoad;
		mundo1.Save();		
		mundo2.Save();
		mundo3.Save();
		mundo2.SetName("cambie2");
		mundo2.Save();
		mundoLoad.Load("cambie2");
		mundoLoad.SetName("cambie otra vez 2");
		mundoLoad.Save();

		//user_1.AddWorld(mundo3);
		user_1.Save();

		core::ipersistence::UserPersistence userLoadloco;
		userLoadloco.Load(1); //loading all children

		userLoadloco.Save();

		//-----------
		core::ipersistence::EntityPersistence ent_1("flower");
		core::ipersistence::EntityPersistence ent_2("starship");
		core::ipersistence::EntityPersistence ent_3("teapot");
		core::ipersistence::EntityPersistence ent_4("field");
		core::ipersistence::EntityPersistence ent_5("panda");
		ent_1.SetModelData("flower/flower");
		ent_2.SetModelData("starship/falcon");
		ent_3.SetModelData("teapot");
		ent_4.SetModelData("environment");
		ent_5.SetModelData("panda-model");
		ent_1.SetPosition(10,0,0);
		ent_1.SetScale(0.2f);
		ent_2.SetPosition(0,0,0);
		ent_3.SetPosition(0,20,0);
		ent_3.SetScale(2.5);
		ent_5.SetScale(2.5);
		ent_4.SetPosition(-8,42.05f,0);
		ent_4.SetScale(0.25f);
		ent_5.SetPosition(20,20,0);
		ent_1.Save();		
		ent_2.Save();
		ent_3.Save();
		ent_4.Save();
		ent_5.Save();
		mundo1.AddEntity( (*(core::IEntityPersistence*)&ent_1) );//duplicates entities :S
		mundo1.AddEntity( (*(core::IEntityPersistence*)&ent_2) );
		mundo1.AddEntity( (*(core::IEntityPersistence*)&ent_3) );
		mundo3.AddEntity( (*(core::IEntityPersistence*)&ent_1) );
		mundo3.AddEntity( (*(core::IEntityPersistence*)&ent_3) );
		mundo3.AddEntity( (*(core::IEntityPersistence*)&ent_4) );
		mundo3.AddEntity( (*(core::IEntityPersistence*)&ent_5) );
		mundo1.Save();
		mundo3.Save();
		mundo1.Load();
		core::ipersistence::WorldPersistence mundo3_test;
		mundo3_test.Load(mundo3.getId());
		//---------------------

		core::ipersistence::UserPersistence user_anonimo("anonimo", "anonimo");
		user_anonimo.Save();

		core::ipersistence::UserPersistence userLoadname_A;
		core::ipersistence::UserPersistence userLoadname_B;
		userLoadname_A.Load("Floripondio");
		userLoadname_B.Load("anonimo");

		core::ipersistence::WorldPersistence mundo_salvaje("mimundo1", "anonimo", 127);
		core::ipersistence::WorldPersistence mundo_salvaje2("mimundo2");
		mundo_salvaje.Save();
		mundo_salvaje2.SetOwner("anonimo");
		mundo_salvaje2.Save();

		//TESTS-----------------------------------------------------------------------

		//---- ON BOOT DEFAULT SCENE---
		core::ipersistence::UserPersistence default_user("default_user", "this_is_the_default_scene");
		//core::ipersistence::EntityPersistence flower("flower");
		//core::ipersistence::EntityPersistence teapot("teapot");
		//core::ipersistence::EntityPersistence field("field");
		//core::ipersistence::EntityPersistence panda("panda");
		//flower.SetModelData("flower/flower");
		//teapot.SetModelData("teapot");
		//field.SetModelData("environment");
		//panda.SetModelData("panda-model");
		core::ipersistence::WorldPersistence default_world("default_world", "default_user", 127);
		default_user.SetModel("teapot");
		default_user.SetScale(1.0);
		default_user.Save();
		//flower.SetPosition(10,0,0);
		//flower.SetScale(0.01);
		//teapot.SetPosition(0,20,0);
		//teapot.SetScale(1.5);
		//field.SetPosition(-8,42.05,0);
		//field.SetScale(0.25);
		//panda.SetPosition(20,20,0);
		//panda.SetScale(0.005);
		//flower.Save();
		//teapot.Save();
		//field.Save();
		//panda.Save();
		//default_world.AddEntity( (*(core::IEntityPersistence*)&flower) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&teapot) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&field ) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&panda ) );
		default_world.Save();
		//-----------------------------

	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

MainPersistence::~MainPersistence()
{
}

void MainPersistence::ProcessException(const dba::SQLException& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "SQL Error: " << pEx.what() << std::endl;
	wop << "While executing: " << std::endl << pEx.getQuery() << std::endl;
	error = wop.str();
}

void MainPersistence::ProcessException(const dba::Exception& pEx)
{
	std::string error;
	std::stringstream wop;
	wop << "Error: " << pEx.what() << std::endl;
	error = wop.str();
}

void MainPersistence::Delete()
{
}

void MainPersistence::GetUserData(const int &id, std::string &name, std::string &passwd)
{
	try 
	{
		//std::vector<std::string> &names;
		//std::vector<std::string> &passwords;
		std::string names_, passwords_;
		std::stringstream query;
		query << "SELECT name,password FROM user_table WHERE id = " << "'" << id << "'";
		std::auto_ptr<dba::DbResult> res(ar.getIStream().sendQuery(dba::SQL(query.str().c_str()).into(names_).into(passwords_)) );
		while(res->fetchRow()) 
		{ 
			//names.push_back(names_);
			//passwords.push_back(passwords_);
			names_;
			passwords_;
		}
		name	= names_;
		passwd	= passwords_;
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

//void MainPersistence::ListProjects()
//{
//	//for (int i = 0; i < 100; i++)
//	//{	std::stringstream wop;
//	//	wop << "Desde IPersistence con amor.." << i;
//	//	if (app != NULL) app->PostLogMessage(wop.str());
//	//}
//}

//void MainPersistence::LoadProject(unsigned int id)
//{}
//
//void MainPersistence::LoadProjectInfo(unsigned int id)
//{}

bool MainPersistence::UserExists(const std::string &name)
{
	try 
	{
		std::string result;
		std::stringstream query;
		query << "SELECT * FROM user_table WHERE name = " << "'" << name << "'";
		std::auto_ptr<dba::DbResult> res(ar.getIStream().sendQuery(dba::SQL(query.str().c_str()).into(result)) );
		while(res->fetchRow()) 
		{ 
			result;
			return true;
		}
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
	return false;
}

bool MainPersistence::WorldExists(const std::string &name)
{
	try 
	{
		std::string result;
		std::stringstream query;
		query << "SELECT * FROM world_table WHERE name = " << "'" << name << "'";
		std::auto_ptr<dba::DbResult> res(ar.getIStream().sendQuery(dba::SQL(query.str().c_str()).into(result)) );
		while(res->fetchRow()) 
		{ 
			result;
			return true;
		}
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
	return false;
}

void MainPersistence::GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions)
{
	try 
	{
		std::string names_;
		int permissions_;
		std::stringstream query;
		query << "SELECT name,permissions FROM world_table WHERE owner = " << "'" << user_name << "'";
		std::auto_ptr<dba::DbResult> res(ar.getIStream().sendQuery(dba::SQL(query.str().c_str()).into(names_).into(permissions_)) );
		while(res->fetchRow()) 
		{ 
			names.push_back(names_);
			permissions.push_back(permissions_);
		}
	} 
	catch (const dba::SQLException& pEx)	{ ProcessException(pEx); }
	catch (const dba::Exception& pEx)		{ ProcessException(pEx); }
}

bool MainPersistence::DeleteWorld(const std::string &name)
{
	core::ipersistence::WorldPersistence world;
	if (world.Load(name))
	{	world.Delete();
		return true;	}
	return false;
}

bool MainPersistence::SaveWorldPermissions(const std::string &name, const int permissions)
{
	core::ipersistence::WorldPersistence world;
	if (world.Load(name))
	{	world.SetPermissions(permissions);
		world.Save();
		return true;	}
	return false;
}

bool MainPersistence::DeleteUser(const std::string &name)
{
	core::ipersistence::UserPersistence user;
	if (user.Load(name))
	{
		std::vector<std::string> world_names;
		std::vector<int> world_perm;
		GetWorldList(name, world_names, world_perm);
		for (unsigned int i = 0; i < world_names.size(); i++)
			DeleteWorld(world_names[i]);
		user.Delete();
		return true;
	}
	return false;
}