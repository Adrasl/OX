#ifndef _ENTITYPERSISTENCE_
#define _ENTITYPERSISTENCE_

#include <core/IPersistence/IEntityPersistence.h>
#include <ipersistence/types.h>
#include <ipersistence/mainpersistence.h>
#include <dba/dba.h>
#include <dba/double_filter.h>
#include <string>
#include <boost/thread.hpp>

namespace core
{
	namespace ipersistence
	{
		class MainPersistence;

		class EntityPersistence : core::IEntityPersistence, public dba::Storeable
		{
			DECLARE_STORE_TABLE();

			public:

				EntityPersistence( const std::string &name_ = ""); 
				virtual ~EntityPersistence();
				virtual void Load(const int &id);
				virtual void Load(const std::string &name);
				virtual void Load() {};
				virtual void Save();
				virtual void Delete();

				virtual void SetName(const std::string &name_)			{ name = name_; Changed();			}
				virtual void SetModelData(const std::string &value)		{ modeldata = value; Changed();		}
				virtual void SetSoundDataCreate(const std::string &value)		{ sounddata_create = value; Changed();		}
				virtual void SetSoundDataDestroy(const std::string &value)		{ sounddata_destroy = value; Changed();		}
				virtual void SetSoundDataTouch(const std::string &value)		{ sounddata_touch = value; Changed();		}
				virtual void SetSoundDataIdle(const std::string &value)			{ sounddata_idle = value; Changed();		}


				virtual std::string GetName()							{ return name;						}
				virtual std::string GetModelData()						{ return modeldata;					}
				virtual std::string GetSoundDataCreate()				{ return sounddata_create;			} 
				virtual std::string GetSoundDataDestroy()				{ return sounddata_destroy;			}
				virtual std::string GetSoundDataTouch()					{ return sounddata_touch;			}
				virtual std::string GetSoundDataIdle()					{ return sounddata_idle;			}
				static dba::SQL *GetSchema()							{ return &schema;					}

				virtual void SetPosition(const float &x, const float &y, const float &z)    { position_x = x; position_y = y; position_z = z; }
				virtual void SetVelocity(const float &x, const float &y, const float &z)	{ velocity_x = x; velocity_y = y; velocity_z = z; }
				virtual void SetAcceleration(const float &x, const float &y, const float &z){ acceleration_x = x; acceleration_y = y; acceleration_z = z; }
				virtual void SetOrientation(const float &x, const float &y, const float &z) { orientation_x = x; orientation_y = y; orientation_z = z; }
				virtual void SetUp(const float &x, const float &y, const float &z)		    { up_x = x; up_y = y; up_z = z; }
				virtual void SetScale(const float &value)									{ scale = value; }
				virtual void SetType(const int &value)										{ type = value; }
				virtual void SetPsique(const int &value)									{ psique = value; }

				virtual void GetPosition(float &x, float &y, float &z)						{ x = position_x; y = position_y; z = position_z; }
				virtual void GetVelocity(float &x, float &y, float &z)						{ x = velocity_x; y = velocity_y; z = velocity_z; }
				virtual void GetAcceleration(float &x, float &y, float &z)					{ x = acceleration_x; y = acceleration_y; z = acceleration_z; }
				virtual void GetOrientation(float &x, float &y, float &z)					{ x = orientation_x; y = orientation_y; z = orientation_z; }
				virtual void GetUp(float &x, float &y, float &z)							{ x = up_x; y = up_y; z = up_z; }
				virtual void GetScale(float &value)											{ value = scale; }
				virtual void GetType(int &value)											{ value = type;}
				virtual void GetPsique(int &value)											{ value = psique;}

				virtual bool IsCollidable()													{ return collidable;		}
				virtual void SetCollidable(const bool &value)								{ collidable   = value;		}
				virtual float GetTimeToLive()												{ return time_to_live;		}
				virtual void  SetTimeToLive(const float &value)								{ time_to_live = value;		}

				virtual void SetPositionVelocityAcceleration(const float &px, const float &py, const float &pz,
															 const float &vx, const float &vy, const float &vz,
															 const float &ax, const float &ay, const float &az)	{	position_x = px; position_y = py; position_z = pz;
																													velocity_x = vx; velocity_y = vy; velocity_z = vz;
																													acceleration_x = ax; acceleration_y = ay; acceleration_z = az; }
				virtual void GetPositionVelocityAcceleration(float &px, float &py, float &pz,
															 float &vx, float &vy, float &vz,
															 float &ax, float &ay, float &az)	{	px = position_x; py = position_y; pz = position_z;
																									vx = velocity_x; vy = velocity_y; vz = velocity_z;
																									ax = acceleration_x; ay = acceleration_y; az = acceleration_z; }

			private:

				std::string name;

				int   type, psique, collidable;
				float position_x, position_y, position_z, 
					  velocity_x, velocity_y, velocity_z,
					  acceleration_x, acceleration_y, acceleration_z,
					  orientation_x, orientation_y, orientation_z,
					  up_x, up_y, up_z,
					  scale, time_to_live;

				std::string modeldata;
				std::string sounddata_create;
				std::string sounddata_destroy;
				std::string sounddata_touch;
				std::string sounddata_idle;

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

