#include <controllers/ContentCreationController.h>

#include <debugger.h> 

#define CCTIMELAPSE 0.1
#define CCCHANGEBACKGROUNDMUSIC 60.0
#define CC_MAX_HEADPOS 90.0
#define CC_MIN_HEADPOS -10.0
#define CC_MAX_PITCH 2.0
#define CC_MIN_PITCH 1.0

IApplication* ContentCreationController::app = NULL;
IApplicationConfiguration* ContentCreationController::iapp_config=NULL;
IPercept* ContentCreationController::app_mainpercept=NULL;
IProd* ContentCreationController::app_mainprod=NULL;
IUserPersistence* ContentCreationController::current_user=NULL;
IWorldPersistence* ContentCreationController::current_world=NULL;
ContentCreationController *ContentCreationController::instance = NULL;

int ContentCreationController::entity_id=0;
double ContentCreationController::start_timestamp = 0;
double ContentCreationController::latest_timestamp = 0;
double ContentCreationController::current_timestamp = 0;
double ContentCreationController::music_timestamp = 0;
double ContentCreationController::createdEntity_timesptamp = 0;

int ContentCreationController::z_step = 0;
int ContentCreationController::background_sound = 0;
float ContentCreationController::psique=1.0f; //0-1-2 good-neutral-evil
float ContentCreationController::energy=0.5f; //0-1 calm-energetic

std::map<int, core::IEntityPersistence*> ContentCreationController::RTree_Entities_by_entityIDs;
std::map<NatureOfEntity, RTree<int, float, 3, float> *> ContentCreationController::RTree_Entities_SpatialIndexes;
std::map<NatureOfEntity, std::vector<core::IEntityPersistence*>> ContentCreationController::RTree_Entities_by_Psique;
std::map<int, std::vector<std::string>> ContentCreationController::psique_melody;

boost::try_mutex ContentCreationController::m_mutex;


float RandomFloat(const float &Min, const float &Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

ContentCreationController::ContentCreationController()
{}

void ContentCreationController::SetApp(IApplication *app_, core::IApplicationConfiguration* iapp_config_, IPercept *app_mainpercept_, IProd *app_mainprod_) 
{
	app = app_; 
	iapp_config = iapp_config_; 

	app_mainpercept=app_mainpercept_; 
	app_mainprod=app_mainprod_;

	srand (time(NULL));

	if ( iapp_config )
	{	
		//Prepare melodies
		std::stringstream sound_filename_base;
		std::stringstream sound_filename_MG1, sound_filename_MG2, 
			              sound_filename_MN1, sound_filename_MN2, //sound_filename_MN3,
						  sound_filename_ME1, sound_filename_ME2;
		
		sound_filename_base << iapp_config->GetSoundDirectory();
		sound_filename_MG1 << sound_filename_base.str() << "MG0008.wav";
		sound_filename_MG2 << sound_filename_base.str() << "MG0010.wav";
		sound_filename_MN1 << sound_filename_base.str() << "MN0001.wav";
		sound_filename_MN2 << sound_filename_base.str() << "MN0003.wav";
		//sound_filename_MN3 << sound_filename_base.str() << "MN0006.wav";
		sound_filename_ME1 << sound_filename_base.str() << "ME0001.wav";
		sound_filename_ME2 << sound_filename_base.str() << "ME0002.wav";

		std::vector<std::string> good_melodies;
		std::vector<std::string> neutral_melodies;
		std::vector<std::string> evil_melodies;

		good_melodies.push_back(sound_filename_MG1.str());
		good_melodies.push_back(sound_filename_MG2.str());
		neutral_melodies.push_back(sound_filename_MN1.str());
		neutral_melodies.push_back(sound_filename_MN2.str());
		//neutral_melodies.push_back(sound_filename_MN3.str());
		evil_melodies.push_back(sound_filename_ME1.str());
		evil_melodies.push_back(sound_filename_ME2.str());
		
		psique_melody[IA_Karma::GOOD]	= good_melodies;
		psique_melody[IA_Karma::NEUTRAL]= neutral_melodies;
		psique_melody[IA_Karma::EVIL]	= evil_melodies;
	}



}

ContentCreationController *ContentCreationController::Instance ()
{

	if (!instance)
	{
		boost::mutex::scoped_lock lock(m_mutex);

		instance = new ContentCreationController;
		start_timestamp = (double)clock()/CLOCKS_PER_SEC;

		RTree_Entities_SpatialIndexes[NatureOfEntity::NONINTERACTIVE]	= new RTree<int, float, 3, float>();
		RTree_Entities_SpatialIndexes[NatureOfEntity::STANDALONE]		= new RTree<int, float, 3, float>();
		RTree_Entities_SpatialIndexes[NatureOfEntity::BOID]				= new RTree<int, float, 3, float>();
		RTree_Entities_SpatialIndexes[NatureOfEntity::TREE]				= new RTree<int, float, 3, float>();

		std::vector<std::string> good_melodies;
		std::vector<std::string> neutral_melodies;
		std::vector<std::string> evil_melodies;



		//Start statistical acccumulators
		accumulator_set<double, stats<tag::mean, tag::moment<2> > > acc;

		// push in some data ...
		acc(1.2);
		acc(2.3);
		acc(3.4);
		acc(4.5);

		// Display the results ...
		std::cout << "Mean:   " << mean(acc) << std::endl;
		std::cout << "Moment: " << moment<2>(acc) << std::endl;

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
		{	IWorldPersistence* latest_world = current_world;
			current_world = app->GetCurrentWorld();
			current_user  = app->GetCurrentUser();
			if (!current_user)
				current_user = app->GetDefaultUser();

			if (latest_world != current_world)
			{
				psique = (float)IA_Karma::NEUTRAL;
				energy = (float)IA_Energy::EXITED/2.0f;
				if (current_user)
				{	int aux_psique;
					current_user->GetPsique(aux_psique);
					psique= (float)aux_psique;
				}
				latest_world = current_world;
			}
			if (!current_world)
				current_world = app->GetDefaultWorld();

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

		latest_timestamp = current_timestamp;
		current_timestamp = (double)clock()/CLOCKS_PER_SEC;
		double time_since_start = current_timestamp - start_timestamp;
		double time_since_latest = current_timestamp - latest_timestamp;

		//retomar, make static
		core::corePoint3D<double> head_pos, presence_center_of_mass, 
					  space_bounding_box_min, space_bounding_box_max, space_center, 
					  main_lateraldominance, main_orientation, main_eccentricity;
		std::vector<MotionElement> motion_elements;
		bool presence_detected = false;

		space_bounding_box_max.x = space_bounding_box_max.y = space_bounding_box_max.z = 0.0;
		space_bounding_box_min.x = space_bounding_box_min.y = space_bounding_box_min.z = 0.0;
		presence_center_of_mass.x = presence_center_of_mass.y = presence_center_of_mass.z = 0.0;


		//CHANGE THE WORLD
		//------------------------------------------------------
		if ((time_since_start >= 1) )
		{
			if (app_mainpercept)
			{	
				presence_detected = app_mainpercept->PresenceDetected();
				app_mainpercept->GetHeadPosition(head_pos);
				app_mainpercept->GetFeaturePosition("CENTER OF MASS", presence_center_of_mass);
				app_mainpercept->GetSpaceBoundingBox(space_bounding_box_min, space_bounding_box_max);
				app_mainpercept->GetMainLateralDominance(main_lateraldominance);
				app_mainpercept->GetMainOrientation(main_orientation);
				app_mainpercept->GetMainEccentricity(main_eccentricity);
				motion_elements = app_mainpercept->GetMotionElements(); //The first one is about the whole image

				if (app_mainprod)
				{	
					switch ((int)psique)
					{
						case IA_Karma::GOOD :
						{	//Do good stuff
							break;
						}
						case IA_Karma::NEUTRAL :
						{	//Do neutral stuff
							break;
						}
						case IA_Karma::EVIL :
						{	//Do evil stuff
							break;
						}
						default : {break;}
					}					

					std::vector<int> background_sounds = app_mainprod->GetBackgroundSounds();
					if ( iapp_config && 
						( (background_sounds.size() == 0) || ((current_timestamp - music_timestamp) > CCCHANGEBACKGROUNDMUSIC) ) )
					{
						int n_melodies = 0;
						int random_index = 0;
						std::string filename;
						if ((int)psique < psique_melody.size())
						{
							app_mainprod->RemoveAllBackgroundSound(5.0f);
							filename = *(psique_melody[(int)psique].begin()+(rand()%(psique_melody[(int)psique].size())));
							background_sound = app_mainprod->AddBackgroundSound(filename, 5.0f);
							music_timestamp = current_timestamp;
						}
					}

					if (false)//(app_mainpercept->FaceDetected())// (head_pos.x || head_pos.y || head_pos.z)
					{
						double relativetoheadpos_pitch = CC_MAX_PITCH * head_pos.z / (CC_MAX_HEADPOS - CC_MIN_HEADPOS);
						if (relativetoheadpos_pitch < CC_MIN_PITCH)
							relativetoheadpos_pitch = CC_MIN_PITCH;
						if (relativetoheadpos_pitch > CC_MAX_PITCH)
							relativetoheadpos_pitch = CC_MAX_PITCH;

						app_mainprod->SetPitchBackgroundSound( background_sound,relativetoheadpos_pitch);
						cout << "HEAD POS Z: " << head_pos.z << "\n";
						cout << "MELODY PITCH: " << relativetoheadpos_pitch << "\n";
					}
				}
			}
			else //perception is not available
			{}
		}

		//CREATE ENTITIES
		//------------------------------------------------------
		if ((current_timestamp - createdEntity_timesptamp >= CCTIMELAPSE)) 
		{
			//if (z_step > 20) //retomar descomentar
			//	z_step = 0;

			//create new entities and insert them into the world
			//------------------------------------------------------
			//Rect3F search_rect(fX-search_delta,fY-search_delta,fZ-search_delta, fX+search_delta,fY+search_delta,fZ+search_delta);
			//int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults_callback, NULL);
			std::stringstream model_url;
			if ( iapp_config )
				model_url << iapp_config->GetModelDirectory() << "tricube_004";	 //"panda-model";	
			std::string modelpath = model_url.str();
			Filename pandafile = Filename::from_os_specific(modelpath);
			std::cout << pandafile.get_fullpath() << "\n";
			
			entity_id++;
			z_step++;
			std::stringstream wop_newEntity;
			wop_newEntity << "StandAloneEntity_" << z_step;
			core::ipersistence::EntityPersistence *genesis = new core::ipersistence::EntityPersistence(wop_newEntity.str());
			genesis->SetPsique(NatureOfEntity::STANDALONE);
			genesis->SetModelData(pandafile);
			//genesis->SetModelData("cube_star.egg");
			//genesis->SetModelData("panda-model");
			//genesis->SetModelData("teapot");
			//genesis->SetModelData("/F/etc/repos/OX/bin/data/models/cube_star.egg");
			//genesis->SetSoundDataCreate("f://etc//repos//OX//motor_old.wav");
			//genesis->SetPosition(0,10,z_step);
			
			genesis->SetSoundDataCreate(iapp_config->GetSoundDirectory()+"B0006.wav");
			space_bounding_box_min;
			space_bounding_box_max;
			corePDU3D<double> candidatepdu;
			//candidatepdu.position.x = RandomFloat(presence_center_of_mass.x - 1.0, presence_center_of_mass.x + 1.0);
			//candidatepdu.position.y = RandomFloat(presence_center_of_mass.y + 10.0, presence_center_of_mass.y + 20.0);
			//candidatepdu.position.z = RandomFloat(presence_center_of_mass.z - 0.0, presence_center_of_mass.z + 1.0);
			candidatepdu.position.x = RandomFloat(-5.0,  5.0);
			candidatepdu.position.y = RandomFloat(5.0, 20.0);
			candidatepdu.position.z = RandomFloat( 0.0,  2.0);
			float scale = RandomFloat( 0.05,  1.0);

			//cout << "NEW ENTITY POS: " << candidatepdu.position.x << ", " << candidatepdu.position.y << ", " << candidatepdu.position.z << "\n";
			genesis->SetPosition(candidatepdu.position.x, candidatepdu.position.y, candidatepdu.position.z);
			genesis->SetScale(scale);
			genesis->Save();
			cout << "N-ENTITIES : " << z_step << "\n";
			core::iprod::OXStandAloneEntity *new_entity = new core::iprod::OXStandAloneEntity((core::IEntityPersistence *)genesis); //retomar descomentar (float)z_step/5.0 );

			if (app) app->AddNewEntityIntoCurrentWorld((core::IEntity*)new_entity);
			createdEntity_timesptamp = current_timestamp;
			//------------------------------------------------------

			//cout << "CONTENT CREATION LOOP: " << time_since_start << "\n";
			//time_start = timestamp;
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

void ContentCreationController::EntityHadAGoodUserFeedback(const bool &was_good)
{
	boost::mutex::scoped_lock lock(m_mutex); // retomar posible bloqueo mutex

	cout << "USER HIT ENTITY, but was GOOD?: " << was_good << "\n";

	if (was_good)
		psique = (psique-0.35f <= 0.0f) ? 0.0f : psique-0.35f;
	else
		psique = (psique+0.35f >= 1.0f) ? 1.0f : psique+0.35f;

	if (current_user)
	{	current_user->SetPsique((int) psique);
		current_user->Save();
	}
}