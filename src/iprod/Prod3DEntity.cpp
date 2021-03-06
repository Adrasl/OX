#include <Prod3DEntity.h>

#include <debugger.h> 

using namespace core::iprod;

Prod3DEntity::Prod3DEntity() : entity(NULL), nodepath(NULL), collidable(false)
{}

Prod3DEntity::Prod3DEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL), collidable(false), 
															ready_to_die(false), sound_create(), sound_destroy(), sound_idle(), sound_touch(), time_to_live(-1.0f)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity != NULL )
	{
		data		= entity->GetModelData();
		collidable	= entity->IsCollidable();
		time_to_live= entity->GetTimeToLive();
		
		//PrepareSounds();
		//if (sound_create.sound_data)
		//	sound_create.sound_data->Play();
	}
}

Prod3DEntity::~Prod3DEntity()
{
	
}

void Prod3DEntity::DeletePersistence()
{

}

void Prod3DEntity::OnDestroy()
{

}

void Prod3DEntity::PrepareSounds()
{
	if (entity)
	{
		sound_create = PrepareSound(entity->GetSoundDataCreate());
		sound_destroy = PrepareSound(entity->GetSoundDataDestroy());
		sound_touch = PrepareSound(entity->GetSoundDataTouch());
		sound_idle = PrepareSound(entity->GetSoundDataIdle());
	}
}
core::coreSound<sf::Sound, sf::SoundBuffer> Prod3DEntity::PrepareSound(std::string file_name)
{
	if (file_name != "")
	{
		//Buffer
		bool error_loadfromfile = false;
		sf::SoundBuffer *new_buffer = new sf::SoundBuffer();
		if ( (new_buffer) && (!new_buffer->LoadFromFile(file_name)))
			error_loadfromfile = true;		
		unsigned int chan = new_buffer->GetChannelsCount();

		if (!error_loadfromfile)
		{
			//Position
			float x, y, z;
			entity->GetPosition(x, y, z);
			core::corePoint3D<double> position;
			position.x = x; position.y = y; position.z = z;

			//Sound
			sf::Sound *new_sound = new sf::Sound();
			new_sound->SetBuffer(*new_buffer);
			new_sound->SetLoop(false);
			new_sound->SetPosition(x, y, z);
			new_sound->SetRelativeToListener(true);
			new_sound->SetMinDistance(10.f);
			new_sound->SetAttenuation(0.75f);
			new_sound->SetPitch(1.0f);
			new_sound->SetVolume(100.f);

			//RESULT
			return core::coreSound<sf::Sound, sf::SoundBuffer>(position, new_sound, new_buffer);
		} 
		else
			cout << "ERROR Loading sound buffer.\n";
	}//RESULT
	return core::coreSound<sf::Sound, sf::SoundBuffer>();
}

bool Prod3DEntity::IsReadyToDie()
{ 
	boost::mutex::scoped_lock lock(m_mutex); 
	return ready_to_die;
}

void Prod3DEntity::SetNodePath(NodePath *value)		
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	nodepath = value; 
	if ( (entity != NULL) && (nodepath != NULL) )
	{
		float posx, posy, posz, rotx, roty, rotz, scale;
		entity->GetPosition(posx, posy, posz);
		entity->GetOrientation(rotx, roty, rotz);
		entity->GetScale(scale);
		nodepath->set_pos(posx,posy,posz);
		nodepath->set_hpr(rotx, roty, rotz);
		nodepath->set_scale(scale,scale,scale);
	}
}

void Prod3DEntity::Load()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (entity)
		entity->Load();
}

void Prod3DEntity::Save()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (entity)
	{
		entity->Save();
	}
}

void Prod3DEntity::UpdateSoundInfo()
{ 

	if (entity)
	{
		float x, y, z;
		entity->GetPosition(x, y, z);

		if(sound_create.sound_data)
		{	sound_create.position.x = x; sound_create.position.y = y; sound_create.position.z = z;
			sound_create.sound_data->SetLoop(sound_touch.loop);
			sound_create.sound_data->SetPosition(sound_touch.position.x, sound_touch.position.y, sound_touch.position.z);
			sound_create.sound_data->SetRelativeToListener(sound_touch.relative_to_listener);
			sound_create.sound_data->SetMinDistance(sound_touch.min_distance);
			sound_create.sound_data->SetAttenuation(sound_touch.attetuation);
			sound_create.sound_data->SetPitch(sound_touch.pitch);
			sound_create.sound_data->SetVolume(sound_touch.amplitude);
		}

		if(sound_touch.sound_data)
		{	sound_touch.position.x = x; sound_touch.position.y = y; sound_touch.position.z = z;
			sound_touch.sound_data->SetLoop(sound_touch.loop);
			sound_touch.sound_data->SetPosition(sound_touch.position.x, sound_touch.position.y, sound_touch.position.z);
			sound_touch.sound_data->SetRelativeToListener(sound_touch.relative_to_listener);
			sound_touch.sound_data->SetMinDistance(sound_touch.min_distance);
			sound_touch.sound_data->SetAttenuation(sound_touch.attetuation);
			sound_touch.sound_data->SetPitch(sound_touch.pitch);
			sound_touch.sound_data->SetVolume(sound_touch.amplitude);
		}

		if(sound_destroy.sound_data)
		{	sound_destroy.position.x = x; sound_destroy.position.y = y; sound_destroy.position.z = z;
			sound_destroy.sound_data->SetLoop(sound_touch.loop);
			sound_destroy.sound_data->SetPosition(sound_touch.position.x, sound_touch.position.y, sound_touch.position.z);
			sound_destroy.sound_data->SetRelativeToListener(sound_touch.relative_to_listener);
			sound_destroy.sound_data->SetMinDistance(sound_touch.min_distance);
			sound_destroy.sound_data->SetAttenuation(sound_touch.attetuation);
			sound_destroy.sound_data->SetPitch(sound_touch.pitch);
			sound_destroy.sound_data->SetVolume(sound_touch.amplitude);
		}

		if (sound_idle.sound_data)
		{	sound_idle.position.x = x; sound_idle.position.y = y; sound_idle.position.z = z;
			sound_idle.sound_data->SetLoop(sound_touch.loop);
			sound_idle.sound_data->SetPosition(sound_touch.position.x, sound_touch.position.y, sound_touch.position.z);
			sound_idle.sound_data->SetRelativeToListener(sound_touch.relative_to_listener);
			sound_idle.sound_data->SetMinDistance(sound_touch.min_distance);
			sound_idle.sound_data->SetAttenuation(sound_touch.attetuation);
			sound_idle.sound_data->SetPitch(sound_touch.pitch);
			sound_idle.sound_data->SetVolume(sound_touch.amplitude);
			sound_idle.sound_data;
		}
	}
}

void Prod3DEntity::SetPDU(const core::corePDU3D<double> &value)
{ 
	boost::mutex::scoped_lock lock(m_mutex);
	pdu = value;	

	if (entity)
	{
		entity->SetPositionVelocityAcceleration(pdu.position.x, pdu.position.y, pdu.position.z,
												pdu.velocity.x, pdu.velocity.y, pdu.velocity.z,
												pdu.acceleration.x, pdu.acceleration.y, pdu.acceleration.z);
	}

	UpdateSoundInfo();
}

void Prod3DEntity::SetPosition(const float &x, const float &y, const float &z)    
{
	boost::mutex::scoped_lock lock(m_mutex);

	pdu.position.x = x;
	pdu.position.y = y;
	pdu.position.y = y;

	if (nodepath)
	{
		nodepath->set_pos(x, y, z);
		UpdateSoundInfo();
	}

	if (entity)
	{
		entity->SetPosition(x, y, z);
	}
}

void Prod3DEntity::SetOrientation(const float &x, const float &y, const float &z) 
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (nodepath)
		nodepath->set_hpr(x, y, z); //yaw, pitch, roll;

	if (entity)
	{
		entity->SetOrientation(x, y, z);
	}
}

void Prod3DEntity::SetUp(const float &x, const float &y, const float &z)		    
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
	{
		entity->SetUp(x, y, z);
	}
}

void Prod3DEntity::SetScale(const float &value)									
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (nodepath)
		nodepath->set_scale(value); //yaw, pitch, roll;

	if (entity)
	{
		entity->SetScale(value);
	}
}


void Prod3DEntity::SetPositionOrientationScale(const float &x, const float &y, const float &z, const float &h, const float &p, const float &r, const float &scale)
{
	boost::mutex::scoped_lock lock(m_mutex);

	pdu.position.x = x;
	pdu.position.y = y;
	pdu.position.z = z;

	if (nodepath)
	{
		nodepath->set_pos(x, y, z);
		nodepath->set_hpr(h, p, r); //yaw, pitch, roll;
		nodepath->set_scale(scale); //yaw, pitch, roll;
		UpdateSoundInfo();
	}

	if (entity)
	{
		entity->SetPosition(x, y, z);
		entity->SetOrientation(h, p, r);
		entity->SetScale(scale);
	}
}

void Prod3DEntity::SetPositionVelocityAcceleration(const float &px, const float &py, const float &pz,
											 const float &vx, const float &vy, const float &vz,
											 const float &ax, const float &ay, const float &az)
{
	boost::mutex::scoped_lock lock(m_mutex);

	pdu.position.x = px;	pdu.position.y = py;	pdu.position.z = pz;
	pdu.velocity.x = vx;	pdu.velocity.y = vy;	pdu.velocity.z = vz;
	pdu.acceleration.x = ax;pdu.acceleration.y = ay;pdu.acceleration.z = az;

	if (nodepath)
	{
		nodepath->set_pos(px, py, pz);
	}

	if (entity)
	{
		entity->SetPositionVelocityAcceleration(px, py, pz,
											    vx, vy, vz,
												ax, ay, az);
	}
}

void Prod3DEntity::GetPositionVelocityAcceleration(float &px, float &py, float &pz,
											 float &vx, float &vy, float &vz,
											 float &ax, float &ay, float &az)
{
	boost::mutex::scoped_lock lock(m_mutex);

	px = pdu.position.x;	py = pdu.position.y;	pz = pdu.position.z;
	vx = pdu.velocity.x;	vy = pdu.velocity.y;	vz = pdu.velocity.z;
	ax = pdu.acceleration.x;ay = pdu.acceleration.y;az = pdu.acceleration.z;
}

void Prod3DEntity::SetPsique(const int &value)									
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
	{
		entity->SetPsique(value);
	}
}

void Prod3DEntity::GetPosition(float &x, float &y, float &z)						
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
		entity->GetPosition(x, y, z);
}

void Prod3DEntity::GetOrientation(float &x, float &y, float &z)					
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
		entity->GetOrientation(x, y, z);
}

void Prod3DEntity::GetPositionOrientationScale(float &x, float &y, float &z, float &h, float &p, float &r, float &scale)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
	{
		entity->GetPosition(x, y, z);
		entity->GetOrientation(h, p, r);
		entity->GetScale(scale);
	}
}

void Prod3DEntity::GetUp(float &x, float &y, float &z)
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
		entity->GetUp(x, y, z);
}

void Prod3DEntity::GetScale(float &value)											
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
		entity->GetScale(value);
}
void Prod3DEntity::GetPsique(int &value)											
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity)
		entity->GetPsique(value);
}

void Prod3DEntity::OnStart()
{
	boost::mutex::scoped_lock lock(m_mutex);
}

void Prod3DEntity::OnUpdate()
{
	boost::mutex::scoped_lock lock(m_mutex);
	this;
}

void Prod3DEntity::OnCollisionCall(IEntity *otherEntity)
{
	boost::mutex::scoped_lock lock(m_mutex);

	this;
	otherEntity;
	Prod3DEntity *prod3dntity = (Prod3DEntity *)otherEntity;
	//retomar
}

void Prod3DEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	boost::mutex::scoped_lock lock(m_mutex);
	this;
	//retomar
}

void Prod3DEntity::StartAnimations()
{
	if (nodepath && (nodepath->get_error_type() == NodePath::ET_ok) )
	{
		auto_bind(nodepath->node(), anim_collection, PartGroup::HMF_ok_part_extra | PartGroup::HMF_ok_anim_extra | PartGroup::HMF_ok_wrong_root_name);
		anim_collection.loop_all(true);
	}
}

