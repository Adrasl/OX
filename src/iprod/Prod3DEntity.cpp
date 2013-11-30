#include <Prod3DEntity.h>

#include <debugger.h> 

using namespace core::iprod;

Prod3DEntity::Prod3DEntity() : entity(NULL), nodepath(NULL), collidable(false)
{}

Prod3DEntity::Prod3DEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL), collidable(false), ready_to_die(false)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (entity != NULL )
		data = entity->GetModelData();
}

Prod3DEntity::~Prod3DEntity()
{
}

void Prod3DEntity::Delete()
{
	if(nodepath)
		delete nodepath;
}

void Prod3DEntity::OnDestroy()
{

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

void Prod3DEntity::SetPDU(const core::corePDU3D<double> &value)
{ 
	boost::mutex::scoped_lock lock(m_mutex);
	pdu = value;	
	if (entity)
		entity->SetPosition(pdu.position.x, pdu.position.y, pdu.position.z);
}

void Prod3DEntity::SetPosition(const float &x, const float &y, const float &z)    
{
	boost::mutex::scoped_lock lock(m_mutex);

	pdu.position.x = x;
	pdu.position.y = y;
	pdu.position.y = y;

	if (nodepath)
		nodepath->set_pos(x, y, z);

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

//void Prod3DEntity::PlaySound(const std::string &label, const bool &loop){}
//void Prod3DEntity::PlaySound(const std::string &label, const bool &loop){}

//void Prod3DEntity::PlayAnimation(const std::string &label){}
//void Prod3DEntity::PlayAnimation(const std::string &label){}




/*_collider_Ptr->add_in_pattern( "into-%in" );
global::_framework.get_event_handler().add_hook( "into-" + _collider_Ptr->get_name(), &TestFunction );

void Prod3DEntity::TestFunction( const Event * e )
 {
    TypedWritableReferenceCount * value = e->get_parameter(0).get_ptr();
    PT(CollisionEntry) entry = DCAST(CollisionEntry, value); 
    nassertv(entry != NULL);
    
    cerr << "Collision from " << entry->get_from_node_path() 
         << " into " << entry->get_into_node_path() << "\n";
 }*/