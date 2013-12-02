#ifndef _IENTITYPERSISTENCE_
#define _IENTITYPERSISTENCE_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ IEntityPersistence
	{
		public:
			
			IEntityPersistence(){};
			~IEntityPersistence(){};
			virtual void Load(const int &id) = 0;
			virtual void Load(const std::string &name) = 0;
			virtual void Load()   = 0;
			virtual void Save()   = 0;
			virtual void Delete() = 0;

			virtual void SetName(const std::string &name_) = 0;
			virtual void SetModelData(const std::string &value)	= 0;
			virtual void SetSoundDataCreate(const std::string &value) = 0;
			virtual void SetSoundDataDestroy(const std::string &value) = 0;
			virtual void SetSoundDataTouch(const std::string &value) = 0;
			virtual void SetSoundDataIdle(const std::string &value)	= 0;
			virtual std::string GetModelData() = 0;
			virtual std::string GetSoundDataCreate() = 0;
			virtual std::string GetSoundDataDestroy() = 0;
			virtual std::string GetSoundDataTouch() = 0;
			virtual std::string GetSoundDataIdle() = 0;

			virtual std::string GetName() = 0;

			virtual void SetPosition(const float &x, const float &y, const float &z) = 0;
			virtual void SetOrientation(const float &x, const float &y, const float &z) = 0;
			virtual void SetUp(const float &x, const float &y, const float &z) = 0;
			virtual void SetScale(const float &value) = 0;
			virtual void SetPsique(const int &value) = 0;
			virtual void GetPosition(float &x, float &y, float &z) = 0;
			virtual void GetOrientation(float &x, float &y, float &z) = 0;
			virtual void GetUp(float &x, float &y, float &z) = 0;
			virtual void GetScale(float &value) = 0;
			virtual void GetPsique(int &value) = 0;

	};
}

#endif

