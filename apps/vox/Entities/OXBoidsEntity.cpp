#include <Entities/OXBoidsEntity.h>
//#include <controllers/ContentCreationController.h>
//#include <Entities/OXBoidsEntity.h>

#include <debugger.h> 

using namespace core::iprod;

#define USER_HIT_THRESHOLD_STRENGHT 40.f
#define BOID_THINKTIMELAPSE 0.5f

OXBoidsEntity::OXBoidsEntity(core::IEntityPersistence* ent, const float &pitch, const float &amplitude)
{
	{	boost::mutex::scoped_lock lock(m_mutex);

		think_time			= 0;
		entity				= ent; 
		nodepath			= NULL;
		collidable			= false;
		ready_to_die		= false;
		ignore_collisons	= false;
		already_loaded_in_scene = false;
		time_to_live		= 1.5;
		karma				= 0.5; //good(0) --> evil(1)
		energy				= 0.0; //calm(0) --> exited(1)
		delta_time			= 0.0;
		killme_afterseconds = -1.0;
		recovercollisions_afterseconds = -1.0;
		current_timestamp = latestupdate_timestamp = start_timestamp = (double)clock()/CLOCKS_PER_SEC;
		killme_afterseconds = recovercollisions_afterseconds = delta_time = lived_time = 0.0f;
		otherEntities_feedback[NatureOfEntity::BOID] = IA_Karma::GOOD;
		psique = NatureOfEntity::BOID;

		user_feedback.clear();
		otherEntities_feedback.clear(); 

		if (entity != NULL )
		{
			entity->SetPsique(NatureOfEntity::BOID);
			entity->SetType(Species::SPECIES1);
			data		= entity->GetModelData();
			collidable	= entity->IsCollidable();
			time_to_live = entity->GetTimeToLive();
			entity->Save();
		}
		PrepareSounds();
	}

	core::corePoint3D<float> world_max_coords,
							 world_min_coords;
	corePoint3D<float> separation, alignment, cohesion, 
				   attraction, avoidance, worldlimits;
	world_max_coords.x = world_max_coords.y = world_max_coords.z = 1.0f;
	world_min_coords.x = world_min_coords.y = world_min_coords.z = -1.0f;
	world_max_coords.y = 6.0f; world_min_coords.y = 5.0f;
	separation.x = separation.y = separation.z = 0.0f;
	alignment.x = alignment.y = alignment.z = 0.0f;
	cohesion.x = cohesion.y = cohesion.z = 0.0f;
	attraction.x = attraction.y = attraction.z = 0.0f;
	avoidance.x = avoidance.y = avoidance.z = 0.0f;
	worldlimits.x = worldlimits.y = worldlimits.z = 0.0f;
	{	boost::mutex::scoped_lock lock(csi_mutex);
		my_species			= Species::SPECIES1;
		csi_entity			= entity;
		csi_pdu				= pdu;
		max_acceleration	= 0.5f;
		max_velocity		= 0.25f;
		perception_distance = 1.f;
		separation_distance = 0.01f;
		avoidance_distance	= 0.05f; 
		use_world_limits	= true;
		world_max			= world_max_coords;
		world_min			= world_min_coords;
		//csi_pdu.acceleration.x = csi_pdu.acceleration.y = csi_pdu.acceleration.z = 0.0f;
	}

	//UseWorldLimits(world_max_coords, world_min_coords );
	SetPitch(pitch);
	SetVolume(amplitude);
}

OXBoidsEntity::~OXBoidsEntity()
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

void OXBoidsEntity::DeletePersistence()
{
	boost::mutex::scoped_lock lock(m_mutex);

	core::ipersistence::EntityPersistence* entity_persistence = dynamic_cast<core::ipersistence::EntityPersistence*> (entity);

	if(entity_persistence)
		entity_persistence->Delete();
}

void OXBoidsEntity::SetPitch(const float &value)
{
	boost::mutex::scoped_lock lock(m_mutex);

	sound_create.pitch = value;
	sound_destroy.pitch = value;
	sound_idle.pitch = value;
	sound_touch.pitch = value;

	UpdateSoundInfo();
}

void OXBoidsEntity::SetVolume(const float &value)
{
	boost::mutex::scoped_lock lock(m_mutex);

	sound_create.amplitude = value;
	sound_destroy.amplitude = value;
	sound_idle.amplitude = value;
	sound_touch.amplitude = value;

	UpdateSoundInfo();
}

void OXBoidsEntity::OnStart()
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
//void OXBoidsEntity::OnUpdate()
//{
//	{	boost::mutex::scoped_lock lock(m_mutex);
//		start_timestamp;
//		current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
//		lived_time					= current_timestamp - start_timestamp;
//		delta_time					= current_timestamp - latestupdate_timestamp;
//
//
//		if (killme_afterseconds - current_timestamp > 0.0f)
//		{
//
//		}
//
//		if (  (lived_time > time_to_live) ||
//			  ((killme_afterseconds > 0) && (killme_afterseconds - current_timestamp < 0.0f)) )
//		{
//			KillMyself();
//		}
//
//		if (sound_create.sound_data)
//			sound_create.pitch = start_timestamp;
//		
//		latestupdate_timestamp = current_timestamp;
//	}
//
//	//Animate if touched
//	float rotation_speed_touchingfactor = 1.0;
//	bool animate_touching = recovercollisions_afterseconds - current_timestamp > 0.0;
//	if (animate_touching)
//		rotation_speed_touchingfactor = 1.75*(recovercollisions_afterseconds - current_timestamp);
//
//	////Animate if destroying
//	//bool animate_dying = killme_afterseconds - current_timestamp > 0.0;
//	//if (animate_dying)
//	//{	float scale;
//	//	this->GetScale(scale);
//	//	this->SetScale(scale*(killme_afterseconds - current_timestamp));
//	//}
//
//	//Update entity
//	float x, y, z;
//	float h, p, r;
//	float scale = 1.0;
//	//this->GetPosition(x, y, z);
//	//this->SetPosition(x, y - delta_time*3.0f, z);
//	//this->GetOrientation(h, p, r);
//	//this->SetOrientation(h+delta_time*12.0f*rotation_speed_touchingfactor, 
//	//	                 p+delta_time*60.0f*rotation_speed_touchingfactor, 
//	//					 r+delta_time*120.0f*rotation_speed_touchingfactor);
//	
//	this->GetPositionOrientationScale(x, y, z, 
//									  h, p, r, 
//									  scale);
//	
//	scale = (killme_afterseconds - current_timestamp > 0.0) ? scale*0.5*(killme_afterseconds - current_timestamp) : scale;
//	
//	this->SetPositionOrientationScale(x, y - delta_time*3.0f, z,
//									  h+delta_time*12.0f*rotation_speed_touchingfactor, 
//									  p+delta_time*60.0f*rotation_speed_touchingfactor, 
//									  r+delta_time*120.0f*rotation_speed_touchingfactor,
//									  scale+0.0001f);
//
//	//for (std::map<core::IGuiWindow*, int>::iterator i = registered_windows.begin(); i != registered_windows.end(); i++)
//	//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
//	//if ( found != otherEntities_feedback.end() ) 
//}

void OXBoidsEntity::OnUpdate()
{
	if ( think_time - current_timestamp < 0 ) 
	{
		Think();
		think_time = current_timestamp + BOID_THINKTIMELAPSE;
	}

	{	boost::mutex::scoped_lock lock(m_mutex);
		start_timestamp;
		current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
		lived_time					= current_timestamp - start_timestamp;
		delta_time					= current_timestamp - latestupdate_timestamp;

		if (  (lived_time > time_to_live) ||
			  ((killme_afterseconds > 0) && (killme_afterseconds - current_timestamp < 0.0f)) )
		{
			KillMyself();
		}
		
		latestupdate_timestamp = current_timestamp;
	}

	//Update entity
	float x, y, z;
	float h, p, r;
	float scale = 1.0;
	this->GetPositionOrientationScale(x, y, z, 
									  h, p, r, 
									  scale);
	pdu = csi_pdu;

	x = pdu.position.x + pdu.velocity.x * (current_timestamp - think_time);
	y = pdu.position.y + pdu.velocity.y * (current_timestamp - think_time);
	z = pdu.position.z + pdu.velocity.z * (current_timestamp - think_time);

	this->SetPositionOrientationScale(x, y, z,
									  h+delta_time*12.0f, 
									  p+delta_time*60.0f, 
									  r+delta_time*120.0f,
									  scale+0.0001f);
}

void OXBoidsEntity::OnDeath()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXBoidsEntity::KillMyself()
{	//I'm ready to die and ask my owner to destroy me and forget about me.
	//In the the next frame Mainprod will destroy me.
	ready_to_die = true; //descomentar //retomar x, provoca inconsistencia con CheckCollisions

//	ContentCreationController::Instance()->RemoveEntityFromCurrentWorld((core::IEntity*)this);
}

void OXBoidsEntity::OnDestroy()
{
	boost::mutex::scoped_lock lock(m_mutex);

	//if (entity)
	//{	entity->Delete();
	//}
}

void OXBoidsEntity::ReceiveDamage()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXBoidsEntity::ReceivePleasure()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXBoidsEntity::BeEated()
{
	boost::mutex::scoped_lock lock(m_mutex);
}
void OXBoidsEntity::Copulate()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void OXBoidsEntity::OnCollisionCall(IEntity *otherEntity)
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (!ignore_collisons && !(current_timestamp - recovercollisions_afterseconds < 0))
	{
		IEntity *prod3dntity = (OXBoidsEntity *)otherEntity;
		//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
		//if ( found != otherEntities_feedback.end() ) 
		//retomar
	}
}

void OXBoidsEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	if (!ignore_collisons)
	{
		float collision_strengh = pow(pow(collisionInfo.velocity.x, 2) + pow(collisionInfo.velocity.y, 2) + pow(collisionInfo.velocity.z, 2), 0.5);
		bool the_user_is_good = collision_strengh < USER_HIT_THRESHOLD_STRENGHT;

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
		ContentCreationController::Instance()->EntityHadAGoodUserFeedback(the_user_is_good);
	}
}




















//#include <Entities/OXBoidsEntity.h>
//#include <controllers/ContentCreationController.h>
//
//#include <debugger.h> 
//
//using namespace core::iprod;
//using namespace core::icog;
//
//
//OXBoidsEntity::OXBoidsEntity(core::IEntityPersistence* ent, const float &pitch, const float &amplitude)
//{}
//
//OXBoidsEntity::~OXBoidsEntity()
//{}
//
//void OXBoidsEntity::OnUpdate()
//{
//	Think();
//
//	{	boost::mutex::scoped_lock lock(m_mutex);
//		start_timestamp;
//		current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
//		lived_time					= current_timestamp - start_timestamp;
//		delta_time					= current_timestamp - latestupdate_timestamp;
//
//		if (  (lived_time > time_to_live) ||
//			  ((killme_afterseconds > 0) && (killme_afterseconds - current_timestamp < 0.0f)) )
//		{
//			KillMyself();
//		}
//		
//		latestupdate_timestamp = current_timestamp;
//	}
//
//	//Update entity
//	float x, y, z;
//	float h, p, r;
//	float scale = 1.0;
//	this->GetPositionOrientationScale(x, y, z, 
//									  h, p, r, 
//									  scale);
//	pdu = csi_pdu;
//	//pdu.velocity.x += delta_time * pdu.acceleration.x;
//	//pdu.velocity.y += delta_time * pdu.acceleration.y;
//	//pdu.velocity.z += delta_time * pdu.acceleration.z;
//
//	//pdu.position.x += delta_time * pdu.velocity.x;
//	//pdu.position.y += delta_time * pdu.velocity.y;
//	//pdu.position.z += delta_time * pdu.velocity.z;
//
//	this->SetPositionOrientationScale(pdu.position.x, pdu.position.y, pdu.position.z,
//									  h+delta_time*12.0f, 
//									  p+delta_time*60.0f, 
//									  r+delta_time*120.0f,
//									  scale+0.0001f);
//}
//
//
////////////////////////
////////////////////////
//////////////////////////#include <Entities/OXBoidsEntity.h>
//////////////////////////
//////////////////////////#include <debugger.h> 
//////////////////////////
//////////////////////////using namespace core::iprod;
//////////////////////////
//////////////////////////OXBoidsEntity::OXBoidsEntity(core::IEntityPersistence* ent) 
//////////////////////////{
//////////////////////////	entity		= ent; 
//////////////////////////	nodepath	= NULL;
//////////////////////////	collidable	= false;
//////////////////////////	timeToLive	= 5.0;
//////////////////////////	karma		= 0.5; //good --> evil
//////////////////////////	energy		= 0.0; //calm --> energyc
//////////////////////////
//////////////////////////	if (entity != NULL )
//////////////////////////	{
//////////////////////////		entity->SetPsique(NatureOfEntity::STANDALONE);
//////////////////////////		data = entity->GetModelData();
//////////////////////////		entity->Save();
//////////////////////////	}
//////////////////////////}
//////////////////////////
//////////////////////////OXBoidsEntity::~OXBoidsEntity()
//////////////////////////{
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::Delete()
//////////////////////////{
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::OnStart()
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////}
//////////////////////////void OXBoidsEntity::OnUpdate()
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::OnCollisionCall(IEntity *otherEntity)
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////	OXBoidsEntity *prod3dntity = (OXBoidsEntity *)otherEntity;
//////////////////////////	//retomar
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////	//retomar
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::PlaySound(const string &label, const bool &loop)
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////}
//////////////////////////
//////////////////////////void OXBoidsEntity::PlayAnimation(const string &label)
//////////////////////////{
//////////////////////////	this;
//////////////////////////	int testing = NatureOfEntity::BOID;
//////////////////////////}