#ifndef _IWORLDPERSISTENCE_
#define _IWORLDPERSISTENCE_

#include <core/IPersistence/IEntityPersistence.h>
#include <string>
#include <list>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ IWorldPersistence
	{
		public:

			virtual ~IWorldPersistence(){}
			virtual bool Load(const int &id) = 0;
			virtual bool Load(const std::string &name) = 0;
			virtual void Load()   = 0;
			virtual void Save()   = 0;
			virtual void Delete() = 0;

			virtual void SetName(const std::string &name_) = 0;
			virtual void SetOwner(const std::string &user_owner) = 0;			
			virtual void SetPermissions(const int &permissions_) = 0;
			virtual void AddEntity(core::IEntityPersistence *entity) = 0;
			virtual void RemoveEntity(core::IEntityPersistence *entity) = 0;
			
			virtual std::string GetName() = 0;
			virtual std::string GetOwner() = 0;
			virtual int			GetNumEntities() = 0;			
			virtual int			GetPermissions() = 0;
			virtual void		GetId(int &value)= 0;
			virtual core::IEntityPersistence* GetEntity(const int &i) = 0;
			

	};
}

#endif

