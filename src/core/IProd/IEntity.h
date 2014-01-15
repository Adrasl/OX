#ifndef _IENTITY_
#define _IENTITY_

#include <string>
#include <core/Export.h>
#include <core/types.h>

namespace core
{
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
			/** \brief Sets PDU (Protocol Data unit) that refers to position, velocity and acceleration. **/
			virtual void SetPDU(const core::corePDU3D<double> &value)=0;
			virtual float GetTimeToLive()=0;
			virtual void GetPosition(float &x, float &y, float &z)=0;
			virtual void GetOrientation(float &x, float &y, float &z)=0;
			virtual void GetUp(float &x, float &y, float &z)=0;
			virtual void GetScale(float &value)=0;
			virtual void GetPsique(int &value)=0;
			/** \brief PDU (Protocol Data unit) that refers to position, velocity and acceleration. **/
			virtual corePDU3D<double> GetPDU()=0;

			virtual void SetPositionVelocityAcceleration(const float &px, const float &py, const float &pz,
														 const float &vx, const float &vy, const float &vz,
														 const float &ax, const float &ay, const float &az) = 0;
			virtual void GetPositionVelocityAcceleration(float &px, float &py, float &pz,
														 float &vx, float &vy, float &vz,
														 float &ax, float &ay, float &az) = 0;

			/** \brief Redefine in order to customize behaviour on Creation. **/
			virtual void OnStart()=0;
			/** \brief Redefine in order to customize behaviour, evaluated on every frame. **/
			virtual void OnUpdate()=0;
			/** \brief Redefine in order to customize behaviour on Destruction. **/
			virtual void OnDestroy()=0;
			/** \brief Redefine in order to customize behaviour on Collision. **/
			virtual void OnCollisionCall(IEntity *otherEntity)=0; 
			/** \brief Redefine in order to customize behaviour on Collision. **/
			virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo)=0;

			virtual bool IsCollidable()=0;
			virtual bool IsReadyToDie()=0;
			virtual void SetCollidable(const bool &value)=0;

	};
}


#endif

