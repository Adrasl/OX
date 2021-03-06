#include <Entities/OXStandAloneEntity.h>

#include <debugger.h> 

using namespace core::iprod;

#define USER_HIT_THRESHOLD_STRENGHT 40.f

OXStandAloneEntity::OXStandAloneEntity(core::IEntityPersistence* ent, const float &pitch, const float &amplitude)
{
	{

		boost::mutex::scoped_lock lock(m_mutex);

		entity				= ent; 
		nodepath			= NULL;
		ready_to_die		= false;
		ignore_collisons	= false;
		already_loaded_in_scene = false;
		karma				= 0.5; //good(0) --> evil(1)
		energy				= 0.0; //calm(0) --> exited(1)
		delta_time			= 0.0;
		killme_afterseconds = -1.0;
		recovercollisions_afterseconds = -1.0;
		start_timestamp			= (double)clock()/CLOCKS_PER_SEC;
		latestupdate_timestamp	= (double)clock()/CLOCKS_PER_SEC;
		otherEntities_feedback[NatureOfEntity::STANDALONE] = IA_Karma::GOOD;
		pdu.position.x = pdu.position.y = pdu.position.z = 0.0f;
		pdu.velocity.x = pdu.velocity.y = pdu.velocity.z = 0.0f;
		pdu.acceleration.x = pdu.acceleration.y = pdu.acceleration.z = 0.0f;


		user_feedback.clear();
		otherEntities_feedback.clear(); 

		if (entity != NULL )
		{
			entity->SetPsique(NatureOfEntity::STANDALONE);
			data		= entity->GetModelData();
			collidable	= entity->IsCollidable();
			time_to_live = entity->GetTimeToLive();
			float position_x, position_y, position_z,
				  velocity_x, velocity_y, velocity_z,
				  acceleration_x, acceleration_y, acceleration_z;
			entity->GetPositionVelocityAcceleration(position_x, position_y, position_z,
													velocity_x, velocity_y, velocity_z,
													acceleration_x, acceleration_y, acceleration_z);
			pdu.position.x = position_x, pdu.position.y = position_y, pdu.position.z = position_z,
			pdu.velocity.x = velocity_x, pdu.velocity.y = velocity_y, pdu.velocity.z = velocity_z,
			pdu.acceleration.x = acceleration_x, pdu.acceleration.y = acceleration_y, pdu.acceleration.z = acceleration_z;
			entity->Save();
		}

		PrepareSounds();
	}
		SetPitch(pitch);
		SetVolume(amplitude);
}

OXStandAloneEntity::~OXStandAloneEntity()
{
	boost::mutex::scoped_lock lock(m_mutex);

	if(nodepath)
		delete nodepath;

	if (sound_create.sound_data!=NULL)
	{	sound_create.sound_data->Stop();
		delete sound_create.sound_data; }
	if (sound_create.sound_buffer!=NULL)
		delete sound_create.sound_buffer;

	if (sound_destroy.sound_data!=NULL)
	{	sound_destroy.sound_data->Stop();
		delete sound_destroy.sound_data; }
	if (sound_destroy.sound_buffer!=NULL)
		delete sound_destroy.sound_buffer;

	if (sound_idle.sound_data!=NULL)
	{	sound_idle.sound_data->Stop();
		delete sound_idle.sound_data; }
	if (sound_idle.sound_buffer!=NULL)
		delete sound_idle.sound_buffer;

	if (sound_touch.sound_data!=NULL)
	{	sound_touch.sound_data->Stop();
		delete sound_touch.sound_data; }
	if (sound_touch.sound_buffer!=NULL)
		delete sound_touch.sound_buffer;
}

void OXStandAloneEntity::DeletePersistence()
{
	boost::mutex::scoped_lock lock(m_mutex);

	core::ipersistence::EntityPersistence* entity_persistence = dynamic_cast<core::ipersistence::EntityPersistence*> (entity);

	if(entity_persistence)
		entity_persistence->Delete();
}

void OXStandAloneEntity::SetPitch(const float &value)
{
	boost::mutex::scoped_lock lock(m_mutex);

	sound_create.pitch = value;
	sound_destroy.pitch = value;
	sound_idle.pitch = value;
	sound_touch.pitch = value;

	UpdateSoundInfo();
}

void OXStandAloneEntity::SetVolume(const float &value)
{
	boost::mutex::scoped_lock lock(m_mutex);

	sound_create.amplitude = value;
	sound_destroy.amplitude = value;
	sound_idle.amplitude = value;
	sound_touch.amplitude = value;

	UpdateSoundInfo();
}

void OXStandAloneEntity::OnStart()
{
	boost::mutex::scoped_lock lock(m_mutex);

	start_timestamp				= (double)clock()/CLOCKS_PER_SEC;
	latestupdate_timestamp		= (double)clock()/CLOCKS_PER_SEC;
	current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
	lived_time					= start_timestamp - current_timestamp;
	delta_time					= latestupdate_timestamp - current_timestamp;

	if (!already_loaded_in_scene)
	{	
		already_loaded_in_scene = true;
		if (sound_create.sound_data)
			sound_create.sound_data->Play();
	}
}
void OXStandAloneEntity::OnUpdate()
{
	{	boost::mutex::scoped_lock lock(m_mutex);
		start_timestamp;
		current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
		lived_time					= current_timestamp - start_timestamp;
		delta_time					= current_timestamp - latestupdate_timestamp;


		if (killme_afterseconds - current_timestamp > 0.0f)
		{

		}

		if (  (lived_time > time_to_live) ||
			  ((killme_afterseconds > 0) && (killme_afterseconds - current_timestamp < 0.0f)) )
		{
			KillMyself();
		}

		if (sound_create.sound_data)
			sound_create.pitch = start_timestamp;
		
		latestupdate_timestamp = current_timestamp;
	}

	//Animate if touched
	float rotation_speed_touchingfactor = 1.0;
	bool animate_touching = recovercollisions_afterseconds - current_timestamp > 0.0;
	if (animate_touching)
		rotation_speed_touchingfactor = 1.75*(recovercollisions_afterseconds - current_timestamp);

	//Update entity
	float x, y, z;
	float h, p, r;
	float scale = 1.0;
	
	this->GetPositionOrientationScale(x, y, z, 
									  h, p, r, 
									  scale);
	
	scale = (killme_afterseconds - current_timestamp > 0.0) ? scale*0.5*(killme_afterseconds - current_timestamp) : scale;

	pdu.position.x = x; pdu.position.y = y; pdu.position.z = z;
	pdu.position.y -= delta_time*3.0f;

	this->SetPositionOrientationScale(pdu.position.x, pdu.position.y , pdu.position.z,
									  h+delta_time*12.0f*rotation_speed_touchingfactor, 
									  p+delta_time*60.0f*rotation_speed_touchingfactor, 
									  r+delta_time*120.0f*rotation_speed_touchingfactor,
									  scale+0.0001f);

	this->SetPositionVelocityAcceleration(pdu.position.x, pdu.position.y, pdu.position.z,
										  pdu.velocity.x, pdu.velocity.y, pdu.velocity.z,
										  pdu.acceleration.x, pdu.acceleration.y, pdu.acceleration.z);
}

void OXStandAloneEntity::OnDeath()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXStandAloneEntity::KillMyself()
{	//I'm ready to die and ask my owner to destroy me and forget about me.
	//In the the next frame Mainprod will destroy me.
	ready_to_die = true; 
}

void OXStandAloneEntity::OnDestroy()
{
	boost::mutex::scoped_lock lock(m_mutex);
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
	if (!ignore_collisons && !(current_timestamp - recovercollisions_afterseconds < 0))
	{
		IEntity *prod3dntity = (OXStandAloneEntity *)otherEntity;
	}
}

void OXStandAloneEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	if (!ignore_collisons)
	{
		float collision_strengh = pow(pow(collisionInfo.velocity.x, 2) + pow(collisionInfo.velocity.y, 2) + pow(collisionInfo.velocity.z, 2), 0.5);
		bool the_user_is_good = collision_strengh < USER_HIT_THRESHOLD_STRENGHT;
		cout << "USER COLLISION STRENGHT: " << collision_strengh << "\n";

		{	boost::mutex::scoped_lock lock(m_mutex);

			if (!ignore_collisons && !the_user_is_good)
			{	ignore_collisons = true;
				if (sound_destroy.sound_data)
					sound_destroy.sound_data->Play();
				killme_afterseconds = current_timestamp + 2.0f;
			} 
			else if (!ignore_collisons && 
					 !(current_timestamp - recovercollisions_afterseconds < 0))
			{	
					if (sound_touch.sound_data)
						sound_touch.sound_data->Play();
					recovercollisions_afterseconds = current_timestamp + 2.0f;
			}			
		}
		ContentCreationController::Instance()->EntityHadAGoodUserFeedback(entity, the_user_is_good);
	}
}


