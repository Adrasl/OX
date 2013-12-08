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
				virtual void SetOrientation(const float &x, const float &y, const float &z) { orientation_x = x; orientation_y = y; orientation_z = z; }
				virtual void SetUp(const float &x, const float &y, const float &z)		    { up_x = x; up_y = y; up_z = z; }
				virtual void SetScale(const float &value)									{ scale = value; }
				virtual void SetPsique(const int &value)									{ psique = value; }
				virtual void GetPosition(float &x, float &y, float &z)						{ x = position_x; y = position_y; z = position_z; }
				virtual void GetOrientation(float &x, float &y, float &z)					{ x = orientation_x; y = orientation_y; z = orientation_z; }
				virtual void GetUp(float &x, float &y, float &z)							{ x = up_x; y = up_y; z = up_z; }
				virtual void GetScale(float &value)											{ value = scale; }
				virtual void GetPsique(int &value)											{ value = psique;}

				virtual bool IsCollidable()													{ return collidable;		}
				virtual void SetCollidable(const bool &value)								{ collidable   = value;		}
				virtual float GetTimeToLive()												{ return time_to_live;		}
				virtual void  SetTimeToLive(const float &value)								{ time_to_live = value;		}

			private:

				std::string name;

				int   psique, collidable;
				float position_x, position_y, position_z, 
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

