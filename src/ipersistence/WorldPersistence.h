#ifndef _WORLDPERSISTENCE_
#define _WORLDPERSISTENCE_

#include <core/IPersistence/IWorldPersistence.h>
//#include <core/IPersistence/IEntityPersistence.h>
#include <ipersistence/types.h>
#include <ipersistence/mainpersistence.h>
//#include <ipersistence/EntityPersistence.h>
#include <dba/dba.h>
#include <string>
#include <boost/thread.hpp>

namespace core
{
	namespace ipersistence
	{
		class MainPersistence;
		class EntityPersistence;

		class WorldPersistence : public dba::Storeable, core::IWorldPersistence
		{
			DECLARE_STORE_TABLE();

			public:

				WorldPersistence(const std::string &name_ = "", const std::string &owner_ = "", const int &permissions_ = 127); 
				virtual ~WorldPersistence();
				virtual bool Load(const int &id);
				virtual bool Load(const std::string &name);
				virtual void Load() {};
				virtual void Save();
				virtual void Delete();

				virtual void SetName(const std::string &name_)					{ name = name_; Changed();					}
				virtual void SetOwner(const std::string &user_owner)			{ owner = user_owner; Changed();					}
				virtual void SetPermissions(const int &permissions_)			{ permissions = permissions_; Changed();	}
				virtual void AddEntity(core::IEntityPersistence &entity);
				virtual void RemoveEntity(core::IEntityPersistence &entity);
				virtual std::string GetName()									{ return name;								}
				virtual std::string GetOwner()									{ return owner;								}
				virtual int			GetPermissions()							{ return permissions;						}
				virtual core::IEntityPersistence* GetEntity(const int &i);
				virtual int GetNumEntities()									{ return entities.size();					}
				
				static dba::SQL *GetSchema()									{ return &schema;							}


			private:

				std::string name, owner;
				int permissions;
				std::list<EntityPersistence> entities;

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

