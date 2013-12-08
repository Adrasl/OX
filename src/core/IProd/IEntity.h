#ifndef _IENTITY_
#define _IENTITY_

#include <string>
#include <core/Export.h>
//#include <core/IApplication.h>
//#include <core/IApplicationConfiguration.h>
#include <core/types.h>

namespace core
{
	//class IApplication;

	class _COREEXPORT_ IEntity
	{
		public:

			virtual ~IEntity(){}
			virtual void DeletePersistence() = 0;

			virtual void SetPosition(const float &x, const float &y, const float &z)=0;
			virtual void SetOrientation(const float &x, const float &y, const float &z)=0;
			virtual void SetUp(const float &x, const float &y, const float &z)=0;
			virtual void SetScale(const float &value)=0;
			virtual void SetPsique(const int &value)=0;
			virtual void SetTimeToLive(const float &value)=0;
			virtual float GetTimeToLive()=0;
			virtual void GetPosition(float &x, float &y, float &z)=0;
			virtual void GetOrientation(float &x, float &y, float &z)=0;
			virtual void GetUp(float &x, float &y, float &z)=0;
			virtual void GetScale(float &value)=0;
			virtual void GetPsique(int &value)=0;

			virtual void OnStart()=0;
			virtual void OnUpdate()=0;
			virtual void OnDestroy()=0;
			virtual void OnCollisionCall(IEntity *otherEntity)=0; 
			virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo)=0;

			virtual bool IsCollidable()=0;
			virtual bool IsReadyToDie()=0;
			virtual void SetCollidable(const bool &value)=0;

			//virtual void PlaySound(const std::string &label, const bool &loop)=0;
			//virtual void PlayAnimation(const std::string &label)=0;

	};
}


#endif

