#ifndef _IPROD_3D_ENTITY_
#define _IPROD_3D_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>

#include <string>
#include <vector>

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <modelPool.h>
#include <NodePathCollection.h>
#include <GeomVertexFormat.h>
#include <GeomVertexData.h>
#include <GeomVertexWriter.h>
#include <GeomTriangles.h>

#include <boost/thread.hpp>

#include <SFML/Audio.hpp>

//#include <pandaFramework.h>

namespace core
{
	namespace iprod
	{
		class Prod3DEntity : public core::IEntity
		{
			public:

				Prod3DEntity();
				Prod3DEntity(core::IEntityPersistence* ent);
				virtual ~Prod3DEntity();

				std::string GetData()					{ boost::mutex::scoped_lock lock(m_mutex); return data;		}
				NodePath*	GetNodePath()				{ boost::mutex::scoped_lock lock(m_mutex); return nodepath;	}
				corePDU3D<double> GetPDU()				{ boost::mutex::scoped_lock lock(m_mutex); return pdu;		}
				core::IEntityPersistence* GetEntity()	{ boost::mutex::scoped_lock lock(m_mutex); return entity;	}
				void SetData(const std::string &value)	{ boost::mutex::scoped_lock lock(m_mutex); data = value;	}
				
				virtual bool IsCollidable()						{ boost::mutex::scoped_lock lock(m_mutex); return collidable;}
				virtual bool IsReadyToDie();
				virtual void SetCollidable(const bool &value)	{ boost::mutex::scoped_lock lock(m_mutex); collidable =value;}

				void SetPDU(const core::corePDU3D<double> &value);
				void SetNodePath(NodePath *value);
				void SetPosition(const float &x, const float &y, const float &z);
				void SetOrientation(const float &x, const float &y, const float &z);
				void SetUp(const float &x, const float &y, const float &z);
				void SetScale(const float &value);
				void SetPsique(const int &value);
				void GetPosition(float &x, float &y, float &z);
				void GetOrientation(float &x, float &y, float &z);
				void GetUp(float &x, float &y, float &z);
				void GetScale(float &value);
				void GetPsique(int &value);

				virtual void Delete();
				virtual void Load();
				virtual void Save();
				virtual void OnStart();
				virtual void OnUpdate();
				virtual void OnDestroy();
				virtual void OnCollisionCall(IEntity *otherEntity); 
				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);
				//virtual void PlaySound(const std::string &label, const bool &loop);
				//virtual void PlayAnimation(const std::string &label);

			protected:

				void PrepareSounds();
				core::coreSound<sf::Sound, sf::SoundBuffer> PrepareSound(std::string file_name);
				void UpdateSoundInfo();

				boost::mutex m_mutex;

				std::string					data; //3d model file
				core::IEntityPersistence	*entity;
				NodePath					*nodepath;
				coreSound<sf::Sound, sf::SoundBuffer> sound_create, sound_destroy, sound_idle, sound_touch;
				core::corePDU3D<double>		pdu; //pos, vel, acc
				bool						collidable;
				bool						ready_to_die;

		};
	}
}

#endif

