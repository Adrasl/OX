#include <Entities/OXBoidsEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXBoidsEntity::OXBoidsEntity(core::IEntityPersistence* ent) 
{
	entity		= ent; 
	nodepath	= NULL;
	collidable	= false;
	timeToLive	= 5.0;
	karma		= 0.5; //good --> evil
	energy		= 0.0; //calm --> energyc

	if (entity != NULL )
	{
		entity->SetPsique(NatureOfEntity::STANDALONE);
		data = entity->GetModelData();
		entity->Save();
	}
}

OXBoidsEntity::~OXBoidsEntity()
{
}

void OXBoidsEntity::Delete()
{
}

void OXBoidsEntity::OnStart()
{
	this;
	int testing = NatureOfEntity::BOID;
}
void OXBoidsEntity::OnUpdate()
{
	this;
	int testing = NatureOfEntity::BOID;
}

void OXBoidsEntity::OnCollisionCall(IEntity *otherEntity)
{
	this;
	int testing = NatureOfEntity::BOID;
	OXBoidsEntity *prod3dntity = (OXBoidsEntity *)otherEntity;
	//retomar
}

void OXBoidsEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	this;
	int testing = NatureOfEntity::BOID;
	//retomar
}

void OXBoidsEntity::PlaySound(const string &label, const bool &loop)
{
	this;
	int testing = NatureOfEntity::BOID;
}

void OXBoidsEntity::PlayAnimation(const string &label)
{
	this;
	int testing = NatureOfEntity::BOID;
}