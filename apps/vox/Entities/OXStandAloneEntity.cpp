#include <Entities/OXStandAloneEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXStandAloneEntity::OXStandAloneEntity(core::IEntityPersistence* ent)  
{
	entity		= ent; 
	nodepath	= NULL;
	collidable	= false;
	timeToLive	= 5.0;
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
}

OXStandAloneEntity::~OXStandAloneEntity()
{}

void OXStandAloneEntity::Delete()
{}

void OXStandAloneEntity::Destroy()
{
	//Delete from Escene, World and DB
}

void OXStandAloneEntity::OnStart()
{
	start_timestamp;
	latestupdate_timestamp		= (double)clock()/CLOCKS_PER_SEC;
	current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
	lived_time					= start_timestamp - current_timestamp;
	delta_time					= latestupdate_timestamp - current_timestamp;
}
void OXStandAloneEntity::OnUpdate()
{
	start_timestamp;
	latestupdate_timestamp		= (double)clock()/CLOCKS_PER_SEC;
	current_timestamp			= (double)clock()/CLOCKS_PER_SEC;
	lived_time					= start_timestamp - current_timestamp;
	delta_time					= latestupdate_timestamp - current_timestamp;

	if (lived_time > timeToLive)
	{	OnDeath();
		return;
	}

	//for (std::map<core::IGuiWindow*, int>::iterator i = registered_windows.begin(); i != registered_windows.end(); i++)
	//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
	//if ( found != otherEntities_feedback.end() ) 
}

void OXStandAloneEntity::OnDeath()
{}

void OXStandAloneEntity::ReceiveDamage()
{}
void OXStandAloneEntity::ReceivePleasure()
{}
void OXStandAloneEntity::BeEated()
{}
void OXStandAloneEntity::Copulate()
{}

void OXStandAloneEntity::OnCollisionCall(IEntity *otherEntity)
{
	IEntity *prod3dntity = (OXStandAloneEntity *)otherEntity;
	//std::map<NatureOfEntity, float>::iterator found  = otherEntities_feedback.find(id); 
	//if ( found != otherEntities_feedback.end() ) 
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
