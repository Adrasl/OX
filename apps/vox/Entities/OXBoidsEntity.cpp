#include <Entities/OXBoidsEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXBoidsEntity::OXBoidsEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL), collidable(false)
{
	if (entity != NULL )
	{
		data = entity->GetModelData();
	}
}

OXBoidsEntity::~OXBoidsEntity()
{
}

void OXBoidsEntity::Delete()
{
}

void OXBoidsEntity::SetNodePath(NodePath *value)		
{ 
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

void OXBoidsEntity::OnStart()
{}
void OXBoidsEntity::OnUpdate()
{
	this;
}

void OXBoidsEntity::OnCollisionCall(IEntity *otherEntity)
{
	this;
	otherEntity;
	OXBoidsEntity *prod3dntity = (OXBoidsEntity *)otherEntity;
	//retomar
}

void OXBoidsEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	this;
	//retomar
}

void OXBoidsEntity::PlaySound(const string &label, const bool &loop)
{
}

void OXBoidsEntity::PlayAnimation(const string &label)
{}

/*_collider_Ptr->add_in_pattern( "into-%in" );
global::_framework.get_event_handler().add_hook( "into-" + _collider_Ptr->get_name(), &TestFunction );

void OXBoidsEntity::TestFunction( const Event * e )
 {
    TypedWritableReferenceCount * value = e->get_parameter(0).get_ptr();
    PT(CollisionEntry) entry = DCAST(CollisionEntry, value); 
    nassertv(entry != NULL);
    
    cerr << "Collision from " << entry->get_from_node_path() 
         << " into " << entry->get_into_node_path() << "\n";
 }*/