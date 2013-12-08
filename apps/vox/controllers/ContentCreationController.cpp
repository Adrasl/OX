#include <controllers/ContentCreationController.h>

#include <debugger.h> 

#define CCTIMELAPSE 1.0f
#define CCCHANGEBACKGROUNDMUSIC 60.0f
#define CC_MAX_HEADPOS 90.0f
#define CC_MIN_HEADPOS -10.0f
#define CC_MAX_PITCH 2.0f
#define CC_MIN_PITCH 1.0f
#define CC_GOOD_STEP 0.0015f
#define CC_EVIL_STEP 0.0075f
#define CC_RECOVERCOL_EVAL 1.0f;

IApplication* ContentCreationController::app = NULL;
IApplicationConfiguration* ContentCreationController::iapp_config=NULL;
IPercept* ContentCreationController::app_mainpercept=NULL;
IProd* ContentCreationController::app_mainprod=NULL;
IUserPersistence* ContentCreationController::current_user=NULL;
IWorldPersistence* ContentCreationController::current_world=NULL;
ContentCreationController *ContentCreationController::instance = NULL;

int ContentCreationController::entity_id = 0;
double ContentCreationController::start_timestamp = 0;
double ContentCreationController::latest_timestamp = 0;
double ContentCreationController::current_timestamp = 0;
double ContentCreationController::music_timestamp = 0;
double ContentCreationController::createdEntity_timesptamp = 0;
double ContentCreationController::recover_collisionevaluation_aftertime = 0;

int ContentCreationController::z_step = 0;
int ContentCreationController::background_sound = 0;
float ContentCreationController::psique=1.0f; //0-1-2 good-neutral-evil
float ContentCreationController::energy=0.5f; //0-1 calm-energetic

std::map<int, core::IEntityPersistence*> ContentCreationController::RTree_Entities_by_entityIDs;
std::map<NatureOfEntity, RTree<int, float, 3, float> *> ContentCreationController::RTree_Entities_SpatialIndexes;
std::map<NatureOfEntity, std::vector<core::IEntityPersistence*>> ContentCreationController::RTree_Entities_by_Psique;
std::map<int, std::vector<std::string>> ContentCreationController::psique_melody;
std::string ContentCreationController::current_melody = "";

boost::mutex ContentCreationController::m_mutex;

std::map<ContentCreationController::IA_Karma, corePoint3D<float>>	ContentCreationController::background_color;
std::map<ContentCreationController::IA_Karma, corePoint3D<float>>	ContentCreationController::fog_color;
std::map<ContentCreationController::IA_Karma, float>				ContentCreationController::fog_intensity;
corePoint3D<float> ContentCreationController::current_background_color;
corePoint3D<float> ContentCreationController::current_fog_color;
float			   ContentCreationController::current_fog_intensity = 0.0f;
ContentCreationController::IA_Karma ContentCreationController::i_am_being = ContentCreationController::IA_Karma::NEUTRAL;
bool ContentCreationController::must_change_background = false;
bool ContentCreationController::must_change_music = false;

std::map<core::IEntity *, double> ContentCreationController::new_timed_entities;


float RandomFloat(const float &Min, const float &Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

ContentCreationController::ContentCreationController()
{
	background_color[IA_Karma::GOOD].x = 0.15; 
	background_color[IA_Karma::GOOD].y = 0.15; 
	background_color[IA_Karma::GOOD].z = 1.0;
	background_color[IA_Karma::NEUTRAL].x = 0.15; 
	background_color[IA_Karma::NEUTRAL].y = 1.0; 
	background_color[IA_Karma::NEUTRAL].z = 0.15;
	background_color[IA_Karma::EVIL].x = 1.0; 
	background_color[IA_Karma::EVIL].y = 0.15; 
	background_color[IA_Karma::EVIL].z = 0.15;

	fog_color[IA_Karma::GOOD].x = 0.15; 
	fog_color[IA_Karma::GOOD].y = 0.15; 
	fog_color[IA_Karma::GOOD].z = 1.0;
	fog_color[IA_Karma::NEUTRAL].x = 0.15; 
	fog_color[IA_Karma::NEUTRAL].y = 1.0; 
	fog_color[IA_Karma::NEUTRAL].z = 0.15;
	fog_color[IA_Karma::EVIL].x = 1.0; 
	fog_color[IA_Karma::EVIL].y = 0.15; 
	fog_color[IA_Karma::EVIL].z = 0.15;

	fog_intensity[IA_Karma::GOOD] = 0.001;
	fog_intensity[IA_Karma::NEUTRAL] = 0.1;
	fog_intensity[IA_Karma::EVIL] = 1.0;

	current_background_color.x = background_color[IA_Karma::NEUTRAL].x; current_background_color.y = background_color[IA_Karma::NEUTRAL].y; current_background_color.z = background_color[IA_Karma::NEUTRAL].z;
	current_fog_color.x = fog_color[IA_Karma::NEUTRAL].x; current_fog_color.y = fog_color[IA_Karma::NEUTRAL].y; current_fog_color.z = fog_color[IA_Karma::NEUTRAL].z; 
	current_fog_intensity = fog_intensity[IA_Karma::NEUTRAL];

}

void ContentCreationController::SetApp(IApplication *app_, core::IApplicationConfiguration* iapp_config_, IPercept *app_mainpercept_, IProd *app_mainprod_) 
{
	boost::mutex::scoped_lock lock(m_mutex);

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
			              sound_filename_MN1, sound_filename_MN2, 
						  sound_filename_ME1, sound_filename_ME2;
		
		sound_filename_base << iapp_config->GetSoundDirectory();
		sound_filename_MG1 << sound_filename_base.str() << "MG0008.wav";
		sound_filename_MG2 << sound_filename_base.str() << "MG0010.wav";
		sound_filename_MN1 << sound_filename_base.str() << "MN0001.wav";
		sound_filename_MN2 << sound_filename_base.str() << "MN0003.wav";
		sound_filename_ME1 << sound_filename_base.str() << "ME0001.wav";
		sound_filename_ME2 << sound_filename_base.str() << "ME0002.wav";

		std::vector<std::string> good_melodies;
		std::vector<std::string> neutral_melodies;
		std::vector<std::string> evil_melodies;

		good_melodies.push_back(sound_filename_MG1.str());
		good_melodies.push_back(sound_filename_MG2.str());
		neutral_melodies.push_back(sound_filename_MN1.str());
		neutral_melodies.push_back(sound_filename_MN2.str());
		evil_melodies.push_back(sound_filename_ME1.str());
		evil_melodies.push_back(sound_filename_ME2.str());
		
		psique_melody[IA_Karma::GOOD]	= good_melodies;
		psique_melody[IA_Karma::NEUTRAL]= neutral_melodies;
		psique_melody[IA_Karma::EVIL]	= evil_melodies;
	}

	DoNotified();

	//if (app_mainprod)
	//	app_mainprod->SetBackgroundAndFog(current_background_color.x, current_background_color.y, current_background_color.z,
	//							  current_fog_color.x, current_fog_color.y, current_fog_color.z,
	//							  current_fog_intensity);
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
	
	if (app_mainprod && must_change_background)
	{
		app_mainprod->SetBackgroundAndFog(current_background_color.x, current_background_color.y, current_background_color.z,
								  current_fog_color.x, current_fog_color.y, current_fog_color.z,
								  current_fog_intensity);
		{boost::mutex::scoped_lock lock(m_mutex); must_change_background = false;}
	}

	{	boost::mutex::scoped_lock lock(m_mutex);

		current_world = app->GetCurrentWorld();
		current_user  = app->GetCurrentUser();

		if (!current_world && !current_user)
		{
			current_world = app->GetDefaultWorld();
			current_user = app->GetDefaultUser();
			RestartCurrentUserBackgroundAndFog();
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
				//retomar //descomentar: el rect está generando volúmenes negativos?!
				entity_id++;			
			}
		}
	}
}

void ContentCreationController::Update()
{
	bool animate_background = false;
	core::iprod::OXStandAloneEntity *new_entityx = NULL;
	std::vector<int> background_sounds = app_mainprod->GetBackgroundSounds();
	bool change_music = false;

	{	boost::mutex::scoped_lock lock(m_mutex);
		
		if (!current_world || !current_user)
			return;

		current_timestamp = (double)clock()/CLOCKS_PER_SEC; 

		change_music = must_change_music && (current_melody !="");
	}

	Reset(); 

	if ( change_music )
	{
			app_mainprod->RemoveAllBackgroundSound(5.0f);
			int music_id = app_mainprod->AddBackgroundSound(current_melody, 5.0f);
			{	
				boost::mutex::scoped_lock lock(m_mutex);
				background_sound = music_id;
				music_timestamp = current_timestamp;
				must_change_music = false;
			}
	}

	{	boost::mutex::scoped_lock lock(m_mutex);

		////Interpolate Background color and fog
		////------------------------------------
		animate_background = (recover_collisionevaluation_aftertime - current_timestamp > 0.0);
		if (animate_background)
		{
			int psique_index = (int)psique;
			int target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
			float factor =  (psique - (float)psique_index) / ((float)target_psique_index - (float)psique_index);

			current_background_color.x = background_color[(IA_Karma)psique_index].x + (factor*(background_color[(IA_Karma)target_psique_index].x - background_color[(IA_Karma)psique_index].x)) ;
			current_background_color.y = background_color[(IA_Karma)psique_index].y + (factor*(background_color[(IA_Karma)target_psique_index].y - background_color[(IA_Karma)psique_index].y)) ;
			current_background_color.z = background_color[(IA_Karma)psique_index].z + (factor*(background_color[(IA_Karma)target_psique_index].z - background_color[(IA_Karma)psique_index].z)) ;

			current_fog_color.x = fog_color[(IA_Karma)psique_index].x + (factor*(fog_color[(IA_Karma)target_psique_index].x - fog_color[(IA_Karma)psique_index].x)) ;
			current_fog_color.y = fog_color[(IA_Karma)psique_index].y + (factor*(fog_color[(IA_Karma)target_psique_index].y - fog_color[(IA_Karma)psique_index].y)) ;
			current_fog_color.z = fog_color[(IA_Karma)psique_index].z + (factor*(fog_color[(IA_Karma)target_psique_index].z - fog_color[(IA_Karma)psique_index].z)) ;

			current_fog_intensity = fog_intensity[(IA_Karma)psique_index] + (factor*(fog_intensity[(IA_Karma)target_psique_index] - fog_intensity[(IA_Karma)psique_index])) ;
		}
		//	//int psique_index = (int)psique;
		//	//int target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
		//	//float psique_factor = ((float)target_psique_index) - psique;
		//	//float psique_factor = recover_collisionevaluation_aftertime - current_timestamp;

		//	int psique_index = (int)psique;
		//	int target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
		//	float previous_psique = 0.0;
		//	float current_animated_psique = 0.0f;
		//	
		//	if (i_am_being == IA_Karma::GOOD)
		//	{
		//		previous_psique = psique - (CC_GOOD_STEP);
		//		psique_index = (int)previous_psique;
		//		target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
		//		current_animated_psique = psique; 
		//		current_animated_psique -= CC_GOOD_STEP * (recover_collisionevaluation_aftertime - current_timestamp) / CC_RECOVERCOL_EVAL;
		//	}
		//	else if (i_am_being == IA_Karma::EVIL)
		//	{
		//		previous_psique = psique - (CC_EVIL_STEP);
		//		psique_index = (int)previous_psique;
		//		target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
		//		current_animated_psique = psique; 
		//		current_animated_psique -= CC_EVIL_STEP * (recover_collisionevaluation_aftertime - current_timestamp) / CC_RECOVERCOL_EVAL;
		//	}

		//	float psique_factor = ((float)target_psique_index) - current_animated_psique;

		//	//int psique_index = (int)current_animated_psique;
		//	//int target_psique_index = (psique_index >= IA_Karma::NEUTRAL) ? IA_Karma::EVIL : psique_index + 1;
		//	//float psique_factor = ((float)target_psique_index) - current_animated_psique;
		//	cout << "PSIQUE: " << psique << "CURRENT PSIQUE: " << current_animated_psique << ", PSIQUE INDEX: " << psique_index << ", TARGET PSIQUE INDEX: " << target_psique_index << ", factor:" << psique_factor << "\n";

		//	current_background_color.x = background_color[(IA_Karma)psique_index].x +((background_color[(IA_Karma)target_psique_index].x - background_color[(IA_Karma)psique_index].x)*(1.0-psique_factor));		
		//	current_background_color.y = background_color[(IA_Karma)psique_index].y +((background_color[(IA_Karma)target_psique_index].y - background_color[(IA_Karma)psique_index].y)*(1.0-psique_factor));		
		//	current_background_color.z = background_color[(IA_Karma)psique_index].z +((background_color[(IA_Karma)target_psique_index].z - background_color[(IA_Karma)psique_index].z)*(1.0-psique_factor));		

		//	current_fog_color.x = fog_color[(IA_Karma)psique_index].x +((fog_color[(IA_Karma)target_psique_index].x - fog_color[(IA_Karma)psique_index].x)*(1.0-psique_factor));		
		//	current_fog_color.y = fog_color[(IA_Karma)psique_index].y +((fog_color[(IA_Karma)target_psique_index].y - fog_color[(IA_Karma)psique_index].y)*(1.0-psique_factor));		
		//	current_fog_color.z = fog_color[(IA_Karma)psique_index].z +((fog_color[(IA_Karma)target_psique_index].z - fog_color[(IA_Karma)psique_index].z)*(1.0-psique_factor));		

		//	current_fog_intensity = fog_intensity[(IA_Karma)psique_index]+((fog_intensity[(IA_Karma)target_psique_index] - fog_intensity[(IA_Karma)psique_index])*(1.0-psique_factor)) ;
		//}
		////else
		////	i_am_being = IA_Karma::NEUTRAL;
		//------------------------------------

		latest_timestamp = current_timestamp;
		current_timestamp = (double)clock()/CLOCKS_PER_SEC;
		double time_since_start = current_timestamp - start_timestamp;
		double time_since_latest = current_timestamp - latest_timestamp;

		if (current_user && current_world && (time_since_start >= 5.0))
		{
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

						if ( iapp_config && 
							( (background_sounds.size() == 0) || 
							  ((current_timestamp - music_timestamp) > CCCHANGEBACKGROUNDMUSIC) ) )
						{
							int n_melodies = 0;
							int random_index = 0;
							std::string filename;
							if ((int)psique < psique_melody.size())
							{
								must_change_music = true;
								current_melody = *(psique_melody[(int)floor(psique+0.5f)].begin()+(rand()%(psique_melody[(int)floor(psique+0.5f)].size())));
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
							//cout << "HEAD POS Z: " << head_pos.z << "\n";
							//cout << "MELODY PITCH: " << relativetoheadpos_pitch << "\n";
						}
					}
				}
				else //perception is not available
				{}
			}

			//CREATE ENTITIES
			//------------------------------------------------------
			if (current_world && (current_timestamp - createdEntity_timesptamp >= CCTIMELAPSE)) 
			{
				CreatePresetOfEntities2(1.0f);
				CreatePresetOfEntities2(1.25f);
				CreatePresetOfEntities1(1.5f);
			}
		}
	}
	if (app && app_mainprod) 
	{
		if (new_timed_entities.size() > 0)
		{
			for (std::map<core::IEntity *, double>::iterator entity_iter = new_timed_entities.begin(); entity_iter != new_timed_entities.end(); entity_iter++)
				app->AddNewEntityIntoCurrentWorld(entity_iter->first, entity_iter->second);
			new_timed_entities.clear();
		}

		//background_color[ContentCreationController::IA_Karma::EVIL].x += 0.01; background_color[ContentCreationController::IA_Karma::EVIL].y += 0.01; background_color[ContentCreationController::IA_Karma::EVIL].z += 0.01;
		//fog_color[ContentCreationController::IA_Karma::EVIL].x += 0.01; fog_color[ContentCreationController::IA_Karma::EVIL].y += 0.01; fog_color[ContentCreationController::IA_Karma::EVIL].z += 0.01;
		//fog_intensity[ContentCreationController::IA_Karma::EVIL] += 0.01;

		//app_mainprod->SetBackgroundColor(background_color[ContentCreationController::IA_Karma::EVIL].x, background_color[ContentCreationController::IA_Karma::EVIL].y, background_color[ContentCreationController::IA_Karma::EVIL].z);
		//app_mainprod->SetFogColor(fog_color[ContentCreationController::IA_Karma::EVIL].x, fog_color[ContentCreationController::IA_Karma::EVIL].y, fog_color[ContentCreationController::IA_Karma::EVIL].z);
		//app_mainprod->SetFogIntensity(fog_intensity[ContentCreationController::IA_Karma::EVIL]);

		//app_mainprod->SetBackgroundColor(new_background_color.x, new_background_color.y, new_background_color.z);
		//app_mainprod->SetFogColor(new_fog_color.x, new_fog_color.y, new_fog_color.z);
		//app_mainprod->SetFogIntensity(new_fog_intensity);

		if (animate_background)
		{
			app_mainprod->SetBackgroundAndFog(current_background_color.x, current_background_color.y, current_background_color.z,
											  current_fog_color.x, current_fog_color.y, current_fog_color.z,
											  current_fog_intensity, 
											  1.0f);
			recover_collisionevaluation_aftertime = current_timestamp;
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
	boost::mutex::scoped_lock lock(m_mutex); 

	if (!(recover_collisionevaluation_aftertime - current_timestamp > 0))
	{	
		recover_collisionevaluation_aftertime = current_timestamp + CC_RECOVERCOL_EVAL; 

		if (was_good)
		{	psique = (psique - CC_GOOD_STEP <= 0.0f) ? 0.0f : psique - CC_GOOD_STEP;
			i_am_being = IA_Karma::GOOD;		}
		else
		{	psique = (psique + CC_EVIL_STEP >= 2.0f) ? 2.0f : psique + CC_EVIL_STEP;
			i_am_being = IA_Karma::EVIL;		}

		cout << "NUEVA PSIQUE TRAS COLISION: " << psique << "\n";
		if (current_user)
		{	int previous_psique = 1.0f;
			current_user->GetPsique(previous_psique);
			current_user->SetPsique(floor(psique + 0.5));
			//if (previous_psique != floor(psique + 0.5))
				//current_user->Save();
		}
	}
}

void ContentCreationController::Notified()
{
	boost::mutex::scoped_lock lock(m_mutex); 
	DoNotified();
}

void ContentCreationController::DoNotified()
{
	if(app)
	{	
		psique = (float)IA_Karma::NEUTRAL;
		energy = (float)IA_Energy::EXITED/2.0f;

		current_world = app->GetCurrentWorld();
		current_user  = app->GetCurrentUser();

		RestartCurrentUserBackgroundAndFog();
	}
}

void ContentCreationController::RestartCurrentUserBackgroundAndFog()
{
	if (current_user)
	{	int aux_psique;
		current_user->GetPsique(aux_psique);
		psique = (float)aux_psique;
		current_background_color.x = background_color[(IA_Karma)(int)floor(psique + 0.5)].x;
		current_background_color.y = background_color[(IA_Karma)(int)floor(psique + 0.5)].y;
		current_background_color.z = background_color[(IA_Karma)(int)floor(psique + 0.5)].z;
		current_fog_color.x = fog_color[(IA_Karma)(int)floor(psique + 0.5)].x;
		current_fog_color.y = fog_color[(IA_Karma)(int)floor(psique + 0.5)].y;
		current_fog_color.z = fog_color[(IA_Karma)(int)floor(psique + 0.5)].z;
		current_fog_intensity = fog_intensity[(IA_Karma)(int)floor(psique + 0.5)];
		
		must_change_background = true;
		recover_collisionevaluation_aftertime = current_timestamp + CC_RECOVERCOL_EVAL;
	}

}

void ContentCreationController::CreatePresetOfEntities1(const double &time)
{
	//create new entities and insert them into the world
	//------------------------------------------------------
	//Rect3F search_rect(fX-search_delta,fY-search_delta,fZ-search_delta, fX+search_delta,fY+search_delta,fZ+search_delta);
	//int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults_callback, NULL);

	std::stringstream model_url;
	if ( iapp_config )
		model_url << iapp_config->GetModelDirectory() << "tricube_004";	 //"panda-model";	
	std::string modelpath = model_url.str();
	Filename pandafile = Filename::from_os_specific(modelpath);
	//std::cout << pandafile.get_fullpath() << "\n";
	
	entity_id++;
	z_step++;
	std::stringstream wop_newEntity;
	wop_newEntity << "StandAloneEntity_1_" << z_step;
	core::ipersistence::EntityPersistence *genesis = new core::ipersistence::EntityPersistence(wop_newEntity.str());
	genesis->SetPsique(NatureOfEntity::STANDALONE);
	genesis->SetModelData(pandafile);			
	genesis->SetSoundDataCreate(iapp_config->GetSoundDirectory()+"B0006.wav");
	genesis->SetSoundDataDestroy(iapp_config->GetSoundDirectory()+"D0004.wav");
	genesis->SetSoundDataTouch(iapp_config->GetSoundDirectory()+"D0003.wav");
	corePDU3D<double> candidatepdu;

	float user_pos_x, user_pos_y, user_pos_z;
	user_pos_x = user_pos_y =user_pos_z = 0;
	if (current_user)
		current_user->GetPosition(user_pos_x, user_pos_y, user_pos_z);
	candidatepdu.position.x = RandomFloat(user_pos_x - 3.0, user_pos_x + 3.0);
	candidatepdu.position.y = RandomFloat(user_pos_y + 6.0, user_pos_y + 15.0);
	candidatepdu.position.z = RandomFloat(user_pos_z - 0.5, user_pos_z + 2.0);
	float scale = RandomFloat( 0.5,  0.95);

	//cout << "NEW ENTITY POS: " << candidatepdu.position.x << ", " << candidatepdu.position.y << ", " << candidatepdu.position.z << "\n";
	genesis->SetPosition(candidatepdu.position.x, candidatepdu.position.y, candidatepdu.position.z);
	genesis->SetScale(scale);

	core::iprod::OXStandAloneEntity *new_entity = new core::iprod::OXStandAloneEntity((core::IEntityPersistence *)genesis); //retomar descomentar (float)z_step/5.0 );
	current_world->AddEntity(*((core::IEntityPersistence *)genesis));
	current_world->Save();

	createdEntity_timesptamp = current_timestamp;
	new_timed_entities[(core::IEntity *)new_entity] = time;
				
}
void ContentCreationController::CreatePresetOfEntities2(const double &time)
{
	//create new entities and insert them into the world
	//------------------------------------------------------
	//Rect3F search_rect(fX-search_delta,fY-search_delta,fZ-search_delta, fX+search_delta,fY+search_delta,fZ+search_delta);
	//int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults_callback, NULL);

	std::stringstream model_url;
	if ( iapp_config )
		model_url << iapp_config->GetModelDirectory() << "tricube_006";	 //"panda-model";	
	std::string modelpath = model_url.str();
	Filename pandafile = Filename::from_os_specific(modelpath);
	//std::cout << pandafile.get_fullpath() << "\n";
	
	entity_id++;
	z_step++;
	std::stringstream wop_newEntity;
	wop_newEntity << "StandAloneEntity_2_" << z_step;
	core::ipersistence::EntityPersistence *genesis = new core::ipersistence::EntityPersistence(wop_newEntity.str());
	genesis->SetPsique(NatureOfEntity::STANDALONE);
	genesis->SetModelData(pandafile);			
	genesis->SetSoundDataCreate(iapp_config->GetSoundDirectory()+"B0007.wav");
	genesis->SetSoundDataDestroy(iapp_config->GetSoundDirectory()+"D0004.wav");
	genesis->SetSoundDataTouch(iapp_config->GetSoundDirectory()+"D0003.wav");
	corePDU3D<double> candidatepdu;

	float user_pos_x, user_pos_y, user_pos_z;
	user_pos_x = user_pos_y =user_pos_z = 0;
	if (current_user)
		current_user->GetPosition(user_pos_x, user_pos_y, user_pos_z);
	candidatepdu.position.x = RandomFloat(user_pos_x - 5.0, user_pos_x + 5.0);
	candidatepdu.position.y = RandomFloat(user_pos_y + 6.0, user_pos_y + 20.0);
	candidatepdu.position.z = RandomFloat(user_pos_z - 0.5, user_pos_z + 2.0);
	float scale = RandomFloat( 0.1,  0.25);

	//cout << "NEW ENTITY POS: " << candidatepdu.position.x << ", " << candidatepdu.position.y << ", " << candidatepdu.position.z << "\n";
	genesis->SetPosition(candidatepdu.position.x, candidatepdu.position.y, candidatepdu.position.z);
	genesis->SetScale(scale);

	core::iprod::OXStandAloneEntity *new_entity = new core::iprod::OXStandAloneEntity((core::IEntityPersistence *)genesis); //retomar descomentar (float)z_step/5.0 );
	//new_entity->SetCollidable(false);
	//new_entity->SetTimeToLive(0.5f);
	current_world->AddEntity(*((core::IEntityPersistence *)genesis));
	current_world->Save();

	createdEntity_timesptamp = current_timestamp;
	new_timed_entities[(core::IEntity *)new_entity] = time;
}