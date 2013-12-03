#include <Entities/OXStandAloneEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXStandAloneEntity::OXStandAloneEntity(core::IEntityPersistence* ent, const float &pitch, const float &amplitude)
{

		boost::mutex::scoped_lock lock(m_mutex);

		entity		= ent; 
		nodepath	= NULL;
		collidable	= true;
		ready_to_die= false;
		timeToLive	= 25.0;
		karma		= 0.5; //good(0) --> evil(1)
		energy		= 0.0; //calm(0) --> exited(1)
		delta_time	= 0.0;
		start_timestamp			= (double)clock()/CLOCKS_PER_SEC;
		latestupdate_timestamp	= (double)clock()/CLOCKS_PER_SEC;
		otherEntities_feedback[NatureOfEntity::STANDALONE] = IA_Karma::GOOD;

		user_feedback.clear();
		otherEntities_feedback.clear(); 

		if (entity != NULL )
		{
			entity->SetPsique(NatureOfEntity::STANDALONE);
			data = entity->GetModelData();
			entity->Save();
		}

		PrepareSounds();
		SetPitch(pitch);
		SetVolume(amplitude);

		if (sound_create.sound_data)
			sound_create.sound_data->Play();

}

OXStandAloneEntity::~OXStandAloneEntity()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXStandAloneEntity::SetPitch(const float &value)
{
	sound_create.pitch = value;
	sound_destroy.pitch = value;
	sound_idle.pitch = value;
	sound_touch.pitch = value;

	UpdateSoundInfo();
}

void OXStandAloneEntity::SetVolume(const float &value)
{
	sound_create.amplitude = value;
	sound_destroy.amplitude = value;
	sound_idle.amplitude = value;
	sound_touch.amplitude = value;

	UpdateSoundInfo();
}

void OXStandAloneEntity::OnStart()
{
	boost::mutex::scoped_lock lock(m_mutex);

	start_timestamp;
	latestupdate_timestamp		= (double)clock()/CLOCKS_PER_SEC;
	current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
	lived_time					= start_timestamp - current_timestamp;
	delta_time					= latestupdate_timestamp - current_timestamp;
}
void OXStandAloneEntity::OnUpdate()
{
	{	boost::mutex::scoped_lock lock(m_mutex);
		start_timestamp;
		latestupdate_timestamp		= (double)clock()/CLOCKS_PER_SEC;
		current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
		lived_time					= current_timestamp - start_timestamp;
		delta_time					= current_timestamp - latestupdate_timestamp;

		if (lived_time > timeToLive)
			KillMyself();

		latestupdate_timestamp = current_timestamp;

		if (sound_create.sound_data)
			sound_create.pitch = start_timestamp;
	}

	//Update entity // retomar, posible necesidad de mutex, parece que algunas entidades no se mueven
	float x, y, z;
	this->GetPosition(x, y, z);
	this->SetPosition(x+0.02f, y, z);
	float h, p, r;
	this->GetOrientation(h, p, r);
	this->SetOrientation(h+0.2f, p+1.0f, r+2.0f);


	//for (std::map<core::IGuiWindow*, int>::iterator i = registered_windows.begin(); i != registered_windows.end(); i++)
	//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
	//if ( found != otherEntities_feedback.end() ) 
}

void OXStandAloneEntity::OnDeath()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXStandAloneEntity::KillMyself()
{	//I'm ready to die and ask my owner to destroy me and forget about me.
	//In the the next frame Mainprod will destroy me.
	ready_to_die = true; //descomentar //retomar x, provoca inconsistencia con CheckCollisions

//	ContentCreationController::Instance()->RemoveEntityFromCurrentWorld((core::IEntity*)this);
}

void OXStandAloneEntity::OnDestroy()
{
	boost::mutex::scoped_lock lock(m_mutex);

	//if (entity)
	//{	entity->Delete();
	//}
}

void OXStandAloneEntity::ReceiveDamage()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXStandAloneEntity::ReceivePleasure()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXStandAloneEntity::BeEated()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXStandAloneEntity::Copulate()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXStandAloneEntity::OnCollisionCall(IEntity *otherEntity)
{
	boost::mutex::scoped_lock lock(m_mutex);
	IEntity *prod3dntity = (OXStandAloneEntity *)otherEntity;
	//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
	//if ( found != otherEntities_feedback.end() ) 
	//retomar
}

void OXStandAloneEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	boost::mutex::scoped_lock lock(m_mutex);
	this;
	int testing = NatureOfEntity::STANDALONE;
	//retomar
}

void OXStandAloneEntity::PlaySound(const string &label, const bool &loop)
{
	boost::mutex::scoped_lock lock(m_mutex);
	this;
	int testing = NatureOfEntity::STANDALONE;
}

void OXStandAloneEntity::PlayAnimation(const string &label)
{
	boost::mutex::scoped_lock lock(m_mutex);
	this;
	int testing = NatureOfEntity::STANDALONE;
}
