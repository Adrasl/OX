#ifndef _IUSERPERSISTENCE_
#define _IUSERPERSISTENCE_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ IUserPersistence
	{
		public:

			virtual ~IUserPersistence(){}
			virtual bool Load(const int &id) = 0;
			virtual bool Load(const std::string &name) = 0;
			virtual void Load()	  = 0;
			virtual void Save()   = 0;
			virtual void Delete() = 0;

			virtual void SetName(const std::string &name_) = 0;
			virtual void SetPassword(const std::string &password_) = 0;
			/** \brief (Permissions not available yet). **/
			virtual void SetPermissions(const int &permissions_) = 0;
			virtual void SetPosition(const float &x, const float &y, const float &z) = 0;
			virtual void SetOrientation(const float &x, const float &y, const float &z) = 0;
			virtual void SetUp(const float &x, const float &y, const float &z) = 0;
			virtual void SetScale(const float &value) = 0;
			virtual void SetPsique(const int &value) = 0;
			virtual void SetModel(const std::string &data) = 0;
			
			virtual std::string GetName() = 0;
			virtual std::string GetPassword() = 0;
			/** \brief (Permissions not available yet). **/
			virtual int			GetPermissions() = 0;
			virtual void GetPosition(float &x, float &y, float &z) = 0;
			virtual void GetOrientation(float &x, float &y, float &z) = 0;
			virtual void GetUp(float &x, float &y, float &z) = 0;
			virtual void GetScale(float &value) = 0;
			virtual void GetPsique(int &value) = 0;
			virtual void GetId(int &value) = 0;
			virtual std::string GetModel() = 0;

	};
}

#endif

