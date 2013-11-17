#include <Entities/OXStandAloneEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXStandAloneEntity::OXStandAloneEntity(core::IEntityPersistence* ent)  
{
	entity     = ent; 
	nodepath   = NULL;
	collidable = false;

	if (entity != NULL )
	{
		entity->SetPsique(NatureOfEntity::STANDALONE);
		data = entity->GetModelData();
		entity->Save();
	}
}

OXStandAloneEntity::~OXStandAloneEntity()
{
}

void OXStandAloneEntity::Delete()
{
}

void OXStandAloneEntity::OnStart()
{
	this;
	int testing = NatureOfEntity::STANDALONE;
}
void OXStandAloneEntity::OnUpdate()
{
	this;
	int testing = NatureOfEntity::STANDALONE;
}

void OXStandAloneEntity::OnCollisionCall(IEntity *otherEntity)
{
	this;
	int testing = NatureOfEntity::STANDALONE;
	OXStandAloneEntity *prod3dntity = (OXStandAloneEntity *)otherEntity;
	//retomar
}

void OXStandAloneEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	this;
	int testing = NatureOfEntity::STANDALONE;
	//retomar
}

void OXStandAloneEntity::PlaySound(const string &label, const bool &loop)
{
	this;
	int testing = NatureOfEntity::STANDALONE;
}

void OXStandAloneEntity::PlayAnimation(const string &label)
{
	this;
	int testing = NatureOfEntity::STANDALONE;
}
