#ifndef _IENTITY_
#define _IENTITY_

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IEntity
	{
		public:

			virtual ~IEntity(){}
			virtual void Delete()=0;

			virtual void OnStart()=0;
			virtual void OnUpdate()=0;
			virtual void OnCollisionCall(IEntity *otherEntity)=0; 
			virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo)=0;

			virtual bool IsCollidable()=0;
			virtual void SetCollidable(const bool &value)=0;

			virtual void PlaySound(const std::string &label, const bool &loop)=0;
			virtual void PlayAnimation(const std::string &label)=0;

	};
}


#endif

