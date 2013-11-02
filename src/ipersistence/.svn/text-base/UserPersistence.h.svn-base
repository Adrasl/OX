#ifndef _USERPERSISTENCE_
#define _USERPERSISTENCE_

#include <core/IPersistence/IUserPersistence.h>
#include <ipersistence/types.h>
#include <ipersistence/mainpersistence.h>
#include <dba/dba.h>
#include <string>
#include <boost/thread.hpp>

namespace core
{
	namespace ipersistence
	{
		class MainPersistence;

		class UserPersistence : public core::IUserPersistence, dba::Storeable 
		{
			DECLARE_STORE_TABLE();

			public:

				UserPersistence(const std::string &name_ = "", const std::string &password_ = "", const int &permissions_ = PERM_OWNER_ALL); 
				virtual ~UserPersistence();
				virtual bool Load(const int &id);
				virtual bool Load(const std::string &name);
				virtual void Load() {};
				virtual void Save();
				virtual void Delete();

				virtual void SetName(const std::string &name_)			{ name = name_; Changed();					}
				virtual void SetPassword(const std::string &password_)	{ password = password_; Changed();			}
				virtual void SetPermissions(const int &permissions_)	{ permissions = permissions_; Changed();	}
				virtual void SetPosition(const float &x, const float &y, const float &z)    { position_x = x; position_y = y; position_z = z; }
				virtual void SetOrientation(const float &x, const float &y, const float &z) { orientation_x = x; orientation_y = y; orientation_z = z; }
				virtual void SetUp(const float &x, const float &y, const float &z)		    { up_x = x; up_y = y; up_z = z; }
				virtual void SetScale(const float &value)									{ scale = value; }
				virtual void SetPsique(const int &value)									{ psique = value; }
				virtual void SetModel(const std::string &value)								{ model = value; Changed();	}
				//virtual void AddWorld(const core::ipersistence::WorldPersistence &world) 
				//														{ worlds.push_back(world);			}
				virtual std::string GetName()							{ return name;						}
				virtual std::string GetPassword()						{ return password;					}
				virtual int			GetPermissions()					{ return permissions;				}
				virtual void GetPosition(float &x, float &y, float &z)		    { x = position_x; y = position_y; z = position_z; }
				virtual void GetOrientation(float &x, float &y, float &z)		{ x = orientation_x; y = orientation_y; z = orientation_z; }
				virtual void GetUp(float &x, float &y, float &z)				{ x = up_x; y = up_y; z = up_z; }
				virtual void GetScale(float &value)						        { value = scale; }
				virtual void GetPsique(int &value)						        { value = psique;}
				virtual void GetId(int &value)									{ value = this->getId();}
				virtual std::string GetModel()									{ return model;						}
				static dba::SQL *GetSchema()									{ return &schema;					}


			private:

				std::string name, model;
				std::string password;
				int   permissions, psique;
				float position_x, position_y, position_z, 
					  orientation_x, orientation_y, orientation_z,
					  up_x, up_y, up_z,
					  scale;
				//std::list<core::ipersistence::WorldPersistence> worlds;
				//std::string image;
				//std::string empathy;
				//std::string permissions;

			private:

				dba::SQLArchive *ar;
				dba::SQL *counter_create;
				static dba::SQL schema;
				static boost::mutex m_mutex;

				void Changed();
				void ProcessException(const dba::SQLException& pEx);
				void ProcessException(const dba::Exception& pEx);
		};
	}
}

#endif

