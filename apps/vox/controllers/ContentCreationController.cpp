#include <controllers/ContentCreationController.h>

#include <debugger.h> 

#define CCTIMELAPSE 5.0

IApplication* ContentCreationController::app = NULL;
IPercept* ContentCreationController::app_mainpercept=NULL;
IProd* ContentCreationController::app_mainprod=NULL;
IUserPersistence* ContentCreationController::current_user=NULL;
IWorldPersistence* ContentCreationController::current_world=NULL;
ContentCreationController *ContentCreationController::instance = NULL;

int ContentCreationController::entity_id=0;
double ContentCreationController::time_start = 0;
int ContentCreationController::z_step = 0;

std::map<int, core::IEntityPersistence*> ContentCreationController::RTree_Entities_by_entityIDs;
std::map<NatureOfEntity, RTree<int, float, 3, float> *> ContentCreationController::RTree_Entities_SpatialIndexes;
std::map<NatureOfEntity, std::vector<core::IEntityPersistence*>> ContentCreationController::RTree_Entities_by_Psique;

boost::try_mutex ContentCreationController::m_mutex;


ContentCreationController::ContentCreationController()
{}


ContentCreationController *ContentCreationController::Instance ()
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (!instance)
	{
	instance = new ContentCreationController;
	time_start = (double)clock()/CLOCKS_PER_SEC;

	RTree_Entities_SpatialIndexes[NatureOfEntity::NONINTERACTIVE]	= new RTree<int, float, 3, float>();
	RTree_Entities_SpatialIndexes[NatureOfEntity::STANDALONE]		= new RTree<int, float, 3, float>();
	RTree_Entities_SpatialIndexes[NatureOfEntity::BOID]				= new RTree<int, float, 3, float>();
	RTree_Entities_SpatialIndexes[NatureOfEntity::TREE]				= new RTree<int, float, 3, float>();
	}

	return instance;
}

void ContentCreationController::Clear()
{
	boost::mutex::scoped_lock lock(m_mutex);

	for (std::map<NatureOfEntity, RTree<int, float, 3, float>*>::iterator iter = RTree_Entities_SpatialIndexes.begin(); iter != RTree_Entities_SpatialIndexes.end(); iter++)
		if (iter->second->Count())
			iter->second->RemoveAll();

	entity_id = 0;
	RTree_Entities_by_Psique.clear();
	RTree_Entities_by_entityIDs.clear();

	RTree_Entities_by_Psique[NatureOfEntity::NONINTERACTIVE]	= std::vector<core::IEntityPersistence*>();
	RTree_Entities_by_Psique[NatureOfEntity::STANDALONE]		= std::vector<core::IEntityPersistence*>();
	RTree_Entities_by_Psique[NatureOfEntity::BOID]				= std::vector<core::IEntityPersistence*>();
	RTree_Entities_by_Psique[NatureOfEntity::TREE]				= std::vector<core::IEntityPersistence*>();
}

void ContentCreationController::Reset()
{
	Clear();


	{	boost::mutex::scoped_lock lock(m_mutex);

		if(app)
		{	current_world = app->GetCurrentWorld();
			current_user  = app->GetCurrentUser();

			if (!current_world)
				current_world = app->GetDefaultWorld();
			if (!current_user)
				current_user = app->GetDefaultUser();
		}

		if (current_world)
		{	int num_entities = current_world->GetNumEntities();
			for (int i=0; i < current_world->GetNumEntities(); i++)
			{	
				IEntityPersistence *ient = current_world->GetEntity(i);
				
				float envelope = 0.5;
				corePDU3D<double> position;
				Rect3F position_rect(position.position.x-envelope,position.position.y-envelope,position.position.z-envelope,
								  position.position.x+envelope,position.position.y+envelope,position.position.z+envelope);
				
				int ient_psique = 0;
				ient->GetPsique(ient_psique);
				RTree_Entities_by_entityIDs[entity_id] = ient;
				RTree_Entities_by_Psique[(NatureOfEntity)ient_psique].push_back(ient);
				//RTree_Entities_SpatialIndexes[(NatureOfEntity)ient_psique]->Insert(position_rect.min, position_rect.max, entity_id);
				//retomar //descomentar: el rect est� generando vol�menes negativos?!
				entity_id++;			
			}
		}
	}
}

void ContentCreationController::Update()
{
	Reset();

	{	boost::mutex::scoped_lock lock(m_mutex);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		double dif_time = timestamp - time_start;

		if ((dif_time >= 1) ) //retomar, en ocasiones parecen faltar entidades (puede que en el momento de la petici�n el try-lock de producci�n decida salt�rselo)
		{
			//change theme of the world
			//------------------------------------------------------
			if (app_mainpercept)
			{
				core::corePoint3D<double> head_pos, presence_center_of_mass, 
							  space_bounding_box_min, space_bounding_box_max, space_center, 
							  main_lateraldominance, main_orientation, main_eccentricity;

				bool presence_detected = app_mainpercept->PresenceDetected();

				app_mainpercept->GetHeadPosition(head_pos);
				app_mainpercept->GetFeaturePosition("CENTER OF MASS", presence_center_of_mass);
				app_mainpercept->GetSpaceBoundingBox(space_bounding_box_min, space_bounding_box_max);
				app_mainpercept->GetMainLateralDominance(main_lateraldominance);
				app_mainpercept->GetMainOrientation(main_orientation);
				app_mainpercept->GetMainEccentricity(main_eccentricity);
				int i = 666;

		
			}

			//------------------------------------------------------
		}

		if ((dif_time >= CCTIMELAPSE) && (z_step < 10)) //retomar, en ocasiones parecen faltar entidades (puede que en el momento de la petici�n el try-lock de producci�n decida salt�rselo)
		{
			//create new entities and insert them into the world
			//------------------------------------------------------
			//Rect3F search_rect(fX-search_delta,fY-search_delta,fZ-search_delta, fX+search_delta,fY+search_delta,fZ+search_delta);
			//int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults_callback, NULL);
			entity_id++;
			z_step++;
			std::stringstream wop_newEntity;
			wop_newEntity << "StandAloneEntity_" << z_step;
			core::ipersistence::EntityPersistence *genesis = new core::ipersistence::EntityPersistence(wop_newEntity.str());
			genesis->SetPsique(NatureOfEntity::STANDALONE);
			genesis->SetModelData("teapot");
			genesis->SetPosition(0,10,z_step);
			genesis->SetScale(1.5);
			genesis->Save();

			core::iprod::OXStandAloneEntity *new_entity = new core::iprod::OXStandAloneEntity((core::IEntityPersistence *)genesis);
			if (app) app->AddNewEntityIntoCurrentWorld((core::IEntity*)new_entity);
			//------------------------------------------------------

			//cout << "CONTENT CREATION LOOP: " << dif_time << "\n";
			time_start = timestamp;
		}
	}
}

void ContentCreationController::RemoveEntityFromCurrentWorld(core::IEntity *entity)
{
	if (app)
	{
		app->RemoveEntityFromCurrentWorld(entity);
	}
}