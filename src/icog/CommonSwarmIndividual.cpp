#include <CommonSwarmIndividual.h>

#include <debugger.h> 

using namespace core::icog;

std::map<int, core::IEntityPersistence*> CommonSwarmIndividual::individuals_by_spatialIndex;
std::map<core::IEntityPersistence*, int> CommonSwarmIndividual::species_of_individual;
std::map<int, std::vector<core::IEntityPersistence*>> CommonSwarmIndividual::ecosystem;

RTree<int, float, 3, float> CommonSwarmIndividual::RTree_Everything_spatialIndexes;
std::map<int, RTree<int, float, 3, float>*> CommonSwarmIndividual::RTree_BySpecies_SpatialIndexes;

boost::mutex CommonSwarmIndividual::csi_mutex;

std::vector<int> CommonSwarmIndividual::RTree_search_results;

CommonSwarmIndividual::CommonSwarmIndividual(const int &species_, core::IEntityPersistence* entity_, 
											 const core::corePDU3D<double> &initial_pdu, 
											 const float &max_velocity_, const float &max_acceleration_, const float &perception_distance_) 
	: my_species(species_), csi_entity(entity_), csi_pdu(initial_pdu), 
	  max_acceleration(max_acceleration_), max_velocity(max_velocity_),
	  perception_distance(perception_distance_), 
	  separation_distance(1.0f),
	  avoidance_distance(3.0f), 
	  use_world_limits(false)
{
	csi_pdu.acceleration.x = csi_pdu.acceleration.y = csi_pdu.acceleration.z = 0.0f;
}

CommonSwarmIndividual::~CommonSwarmIndividual()
{
}

void CommonSwarmIndividual::UseWorldLimits(const core::corePoint3D<float> &max, const core::corePoint3D<float> &min )
{
	boost::mutex::scoped_lock lock(csi_mutex);
	use_world_limits = true;

	world_max = max;
	world_min = min;
}
void CommonSwarmIndividual::DontUseWorldLimits()
{
	boost::mutex::scoped_lock lock(csi_mutex);
	use_world_limits = false;
}

bool Register_SearchResults_callback(int id, void* arg)
{
	return CommonSwarmIndividual::Register_SearchResults(id);
}

bool CommonSwarmIndividual::Register_SearchResults(int id)
{
	//boost::mutex::scoped_lock lock(csi_mutex);
	RTree_search_results.push_back(id);
	return true; // don't stop, keep on searching
}

void CommonSwarmIndividual::SetEcosystem(const std::map<int, std::vector<core::IEntityPersistence*>> &ecosystem_) 
{ 
	boost::mutex::scoped_lock lock(csi_mutex);

	ecosystem.clear();
	ecosystem = ecosystem_;

	ClassifyEcosystem();
}

void CommonSwarmIndividual::ExtractFromEcosystem( const int &species, core::IEntityPersistence* entity_to_extract)
{
	boost::mutex::scoped_lock lock(csi_mutex);

	core::IEntityPersistence* entity_to_extract_ = dynamic_cast<core::IEntityPersistence*> (entity_to_extract);

	if (entity_to_extract_)
	{
		std::map<int, std::vector<core::IEntityPersistence*>>::iterator iter_ecosystem = ecosystem.find(species);
		if (iter_ecosystem != ecosystem.end())
		{
			bool found = false;
			for (std::vector<core::IEntityPersistence*>::iterator population_iter = (iter_ecosystem->second).begin(); 
				 !found && (population_iter != (iter_ecosystem->second).end()); )
			{
				 if (entity_to_extract_ == (*population_iter))
				 {
					 found = true;
					 iter_ecosystem->second.erase(population_iter);
				 } 
				 else
					 population_iter++;
			}				
		}
	}
	ecosystem;
}

void CommonSwarmIndividual::ClearEcosystem() 
{ 
	for (std::map<int, RTree<int, float, 3, float>*>::iterator iter = RTree_BySpecies_SpatialIndexes.begin(); iter != RTree_BySpecies_SpatialIndexes.end(); iter ++)
	{
		iter->second->RemoveAll();
		delete iter->second;
	}
	RTree_BySpecies_SpatialIndexes.clear();

	RTree_Everything_spatialIndexes.RemoveAll();
	individuals_by_spatialIndex.clear();
	species_of_individual.clear();
}

void CommonSwarmIndividual::ClassifyEcosystem() 
{
	ClearEcosystem();

	for ( std::map<int, std::vector<core::IEntityPersistence*>>::iterator iter = ecosystem.begin(); iter != ecosystem.end(); iter++)
	{
		iter->first;
		iter->second;

		for (std::vector<core::IEntityPersistence*>::iterator entity_iter = iter->second.begin(); entity_iter != iter->second.end(); entity_iter++)
		{
			core::Subject *ient_subject = dynamic_cast<core::Subject*> (*entity_iter);
			if (*entity_iter && ient_subject && (ient_subject->ObserversCount() == 1 ))
			{
				int spatial_index = individuals_by_spatialIndex.size() + 1;
				individuals_by_spatialIndex[spatial_index] = (*entity_iter);
				species_of_individual[*entity_iter] = iter->first;

				float envelope = 0.05f;
				corePoint3D<float> position;
				(*entity_iter)->GetPosition(position.x, position.y, position.z); //retomar, se está llamando a una entidad que ya ha sido destruída
				core::icog::CommonObstacle *obstacle = dynamic_cast<core::icog::CommonObstacle*> (*entity_iter);
				if (obstacle)
					envelope = obstacle->GetBoundingRadius();

				Rect3F position_rect(position.x-envelope,position.y-envelope,position.z-envelope,
									 position.x+envelope,position.y+envelope,position.z+envelope);

				RTree_Everything_spatialIndexes.Insert(position_rect.min, position_rect.max, spatial_index);
				static std::map<int, RTree<int, float, 3, float>*>::iterator iter_RTree_BySpecies_SpatialIndexes = RTree_BySpecies_SpatialIndexes.find(iter->first);
				if (iter_RTree_BySpecies_SpatialIndexes == RTree_BySpecies_SpatialIndexes.end())
				{
					RTree_BySpecies_SpatialIndexes[iter->first] = new RTree<int, float, 3, float>();
				}
				RTree_BySpecies_SpatialIndexes[iter->first]->Insert(position_rect.min, position_rect.max, spatial_index);
			}
		}
	}
}

void CommonSwarmIndividual::Think()
{
	boost::mutex::scoped_lock lock(csi_mutex);

	perception_distance;
	corePoint3D<float> worldlimits;
    corePoint3D<float> position;
	worldlimits.x = worldlimits.y = worldlimits.z = 0.0f;
	csi_entity->GetPosition(position.x, position.y, position.z);
	Rect3F search_rect(position.x - perception_distance, position.y - perception_distance,position.z - perception_distance, 
					   position.x + perception_distance, position.y + perception_distance,position.z + perception_distance);

	RTree_search_results.clear();
	int overlapping_size = RTree_Everything_spatialIndexes.Search(search_rect.min, search_rect.max, 
																  Register_SearchResults_callback,
																  NULL);
	ResetAccumulators();

	for (std::vector<int>::iterator result_iter = RTree_search_results.begin(); result_iter != RTree_search_results.end(); result_iter++)
	{
		core::IEntityPersistence* result_entity = individuals_by_spatialIndex[*result_iter];
		core::Subject *entitiy_is_subject = dynamic_cast<core::Subject*> (result_entity);
		
		if ( result_entity 
			 && entitiy_is_subject && (entitiy_is_subject->ObserversCount() >0 ))
		{
			int me_id, other_id, me_type, other_type;
			result_entity->GetId(other_id);
			csi_entity->GetId(me_id);

			if(other_id != me_id)
			{
				result_entity->GetType(other_type);
				csi_entity->GetType(me_type);
				if ( other_type == me_type ) //my species
				{
					corePoint3D<float> my_position, other_position,
									   my_velocity, other_velocity,
									   my_acceleration, other_acceleration;
					
					csi_entity->GetPositionVelocityAcceleration
									(my_position.x, my_position.y, my_position.z,
									 my_velocity.x, my_velocity.y, my_velocity.z,
									 my_acceleration.x, my_acceleration.y, my_acceleration.z);

					result_entity->GetPositionVelocityAcceleration
									(other_position.x, other_position.y, other_position.z,
									 other_velocity.x, other_velocity.y, other_velocity.z,
									 other_acceleration.x, other_acceleration.y, other_acceleration.z);

					//WorldLimit: based on distance to the world limits
					//---------------------------------------------------------
					if (use_world_limits)
					{	
						if      ((world_max.x - my_position.x) < 0.0f ) worldlimits.x = world_max.x - 1.0f;
						else if ((my_position.x - world_min.x) < 0.0f ) worldlimits.x = world_min.x + 1.0f;
						if      ((world_max.y - my_position.y) < 0.0f ) worldlimits.y = world_max.y - 1.0f;
						else if ((my_position.y - world_min.y) < 0.0f ) worldlimits.y = world_min.y + 1.0f;
						if      ((world_max.z - my_position.z) < 0.0f ) worldlimits.z = world_max.z - 1.0f;
						else if ((my_position.z - world_min.z) < 0.0f ) worldlimits.z = world_min.z + 1.0f;
					}

					//Separation: based on distance to near mates minus desired separation
					//---------------------------------------------------------
					corePoint3D<float> distance_separation, separation_position;
					separation_position.x = separation_position.y = separation_position.z = 0.0f;
					distance_separation.x = other_position.x - my_position.x;
					distance_separation.y = other_position.y - my_position.y;
					distance_separation.z = other_position.z - my_position.z;
					if ( (std::abs(distance_separation.x) < separation_distance) ||
						 (std::abs(distance_separation.y) < separation_distance) ||
						 (std::abs(distance_separation.z) < separation_distance)   )
					{
						separation_position.x = my_position.x;
						separation_position.y = my_position.y;
						separation_position.z = my_position.z;
						if (std::abs(distance_separation.x) < separation_distance)
							separation_position.x = (distance_separation.x > 0.0f) ? other_position.x - separation_distance : other_position.x + separation_distance;
						if (std::abs(distance_separation.y) < separation_distance)
							separation_position.y = (distance_separation.y > 0.0f) ? other_position.y - separation_distance : other_position.y + separation_distance;
						if (std::abs(distance_separation.z) < separation_distance)
							separation_position.z = (distance_separation.z > 0.0f) ? other_position.z - separation_distance : other_position.z + separation_distance;

						accumulators_separation.x(separation_position.x);
						accumulators_separation.y(separation_position.y);
						accumulators_separation.z(separation_position.z);
					}
					
					//Cohesion: based on position of near mates
					//---------------------------------------------------------
					accumulators_cohesion.x(other_position.x);
					accumulators_cohesion.y(other_position.y);
					accumulators_cohesion.z(other_position.z);

					//Alignment: based on velocity of near mates 
					//---------------------------------------------------------
					accumulators_cohesion.x(other_velocity.x);
					accumulators_cohesion.y(other_velocity.y);
					accumulators_cohesion.z(other_velocity.z);

				}
				else //other species
				{
					corePoint3D<float> my_position, other_position;
					csi_entity->GetPosition(my_position.x, my_position.y, my_position.z);
					result_entity->GetPosition(other_position.x, other_position.y, other_position.z);

					//Avoidance: based on distance to undesired others minus desired separation
					//---------------------------------------------------------
					corePoint3D<float> distance_separation, separation_position;
					distance_separation.x = other_position.x - my_position.x;
					distance_separation.y = other_position.y - my_position.y;
					distance_separation.z = other_position.z - my_position.z;
					if ( (std::abs(distance_separation.x) < avoidance_distance) ||
						 (std::abs(distance_separation.y) < avoidance_distance) ||
						 (std::abs(distance_separation.z) < avoidance_distance)   )
					{
						separation_position.x = my_position.x;
						separation_position.y = my_position.y;
						separation_position.z = my_position.z;
						if (std::abs(distance_separation.x) < avoidance_distance)
							separation_position.x = other_position.x - avoidance_distance;
						if (std::abs(distance_separation.y) < avoidance_distance)
							separation_position.y = other_position.y - avoidance_distance;
						if (std::abs(distance_separation.z) < avoidance_distance)
							separation_position.z = other_position.z - avoidance_distance;

						accumulators_avoidance.x(separation_position.x);
						accumulators_avoidance.y(separation_position.y);
						accumulators_avoidance.z(separation_position.z);
					}
					
					//Attraction: based on position of attractors
					//---------------------------------------------------------
					if (false)
					{	accumulators_cohesion.x(other_position.x);
						accumulators_cohesion.y(other_position.y);
						accumulators_cohesion.z(other_position.z);
					}

				}
			}
		}
	}

	worldlimits;
	separation	= CalculateSeparation();   
	alignment	= CalculateAlignment();      
	cohesion	= CalculateCohesion();   
	attraction	= CalculateAttraction(); 
	avoidance	= CalculateAvoidance();

	float separation_factor	= 1.0f;
	float alignment_factor	= 1.0f;
	float cohesion_factor	= 1.0f;
	float attraction_factor	= 1.0f;
	float avoidance_factor	= 2.0f;
	float worldlimits_factor= 2.0f;

	separation.x *= separation_factor; 
	separation.y *= separation_factor; 
	separation.z *= separation_factor;

	alignment.x *= alignment_factor; 
	alignment.y *= alignment_factor; 
	alignment.z *= alignment_factor;

	cohesion.x *= cohesion_factor; 
	cohesion.y *= cohesion_factor; 
	cohesion.z *= cohesion_factor;

	attraction.x *= attraction_factor; 
	attraction.y *= attraction_factor; 
	attraction.z *= attraction_factor;

	avoidance.x *= avoidance_factor; 
	avoidance.y *= avoidance_factor; 
	avoidance.z *= avoidance_factor;

	worldlimits.x *= worldlimits_factor;
	worldlimits.y *= worldlimits_factor;
	worldlimits.z *= worldlimits_factor;

	AddForce(separation);
	AddForce(alignment);
	AddForce(cohesion);
	AddForce(attraction);
	AddForce(avoidance);
	AddForce(worldlimits);

	CheckPDULimits();

	csi_pdu.velocity.x += csi_pdu.acceleration.x;
	csi_pdu.velocity.y += csi_pdu.acceleration.y;
	csi_pdu.velocity.z += csi_pdu.acceleration.z;
	csi_pdu.acceleration.x = csi_pdu.acceleration.y = csi_pdu.acceleration.z = 0.0f;

	CheckPDULimits();

	csi_pdu.position.x += csi_pdu.velocity.x;
	csi_pdu.position.y += csi_pdu.velocity.y;
	csi_pdu.position.z += csi_pdu.velocity.z;
}

void CommonSwarmIndividual::AddForce(const core::corePoint3D<float> &force)
{
	csi_pdu.acceleration.x += force.x;
	csi_pdu.acceleration.y += force.y;
	csi_pdu.acceleration.z += force.z;
	CheckPDULimits();
}

core::corePoint3D<float> CommonSwarmIndividual::CalculateSeparation()
{
	corePoint3D<float> result, position;
	result.x = result.y = result.z = 0.0f;

	int n_datum = extract_result< tag::count >( accumulators_separation.x );

	if ( (n_datum > 0) && csi_entity)
	{
		double mean_x = extract_result< tag::mean >( accumulators_separation.x );
		double mean_y = extract_result< tag::mean >( accumulators_separation.y );
		double mean_z = extract_result< tag::mean >( accumulators_separation.z );

		csi_entity->GetPosition(position.x, position.y, position.z);
		result.x = mean_x - position.x;
		result.y = mean_y - position.y;
		result.z = mean_z - position.z;
	}
	return result;
}

core::corePoint3D<float> CommonSwarmIndividual::CalculateAlignment()
{
	corePoint3D<float> result, velocity;
	result.x = result.y = result.z = 0.0f;

	int n_datum = extract_result< tag::count >( accumulators_alignment.x );

	if ( (n_datum > 0) && csi_entity)
	{
		double mean_x = extract_result< tag::mean >( accumulators_alignment.x );
		double mean_y = extract_result< tag::mean >( accumulators_alignment.y );
		double mean_z = extract_result< tag::mean >( accumulators_alignment.z );

		csi_entity->GetVelocity(velocity.x, velocity.y, velocity.z);
		result.x = mean_x - velocity.x;
		result.y = mean_y - velocity.y;
		result.z = mean_z - velocity.z;
	}
	return result;
}

core::corePoint3D<float> CommonSwarmIndividual::CalculateCohesion()
{
	corePoint3D<float> result, position;
	result.x = result.y = result.z = 0.0f;

	int n_datum = extract_result< tag::count >( accumulators_cohesion.x );

	if ( (n_datum > 0) && csi_entity)
	{
		double mean_x = extract_result< tag::mean >( accumulators_cohesion.x );
		double mean_y = extract_result< tag::mean >( accumulators_cohesion.y );
		double mean_z = extract_result< tag::mean >( accumulators_cohesion.z );

		csi_entity->GetPosition(position.x, position.y, position.z);
		result.x = mean_x - position.x;
		result.y = mean_y - position.y;
		result.z = mean_z - position.z;
	}
	return result;
}

core::corePoint3D<float> CommonSwarmIndividual::CalculateAttraction()
{
	corePoint3D<float> result, position;
	result.x = result.y = result.z = 0.0f;

	int n_datum = extract_result< tag::count >( accumulators_attraction.x );

	if ( (n_datum > 0) && csi_entity)
	{
		double mean_x = extract_result< tag::mean >( accumulators_attraction.x );
		double mean_y = extract_result< tag::mean >( accumulators_attraction.y );
		double mean_z = extract_result< tag::mean >( accumulators_attraction.z );

		csi_entity->GetPosition(position.x, position.y, position.z);
		result.x = mean_x - position.x;
		result.y = mean_y - position.y;
		result.z = mean_z - position.z;
	}
	return result;
}

core::corePoint3D<float> CommonSwarmIndividual::CalculateAvoidance()
{
	corePoint3D<float> result, position;
	result.x = result.y = result.z = 0.0f;

	int n_datum = extract_result< tag::count >( accumulators_avoidance.x );

	if ( (n_datum > 0) && csi_entity)
	{
		double mean_x = extract_result< tag::mean >( accumulators_avoidance.x );
		double mean_y = extract_result< tag::mean >( accumulators_avoidance.y );
		double mean_z = extract_result< tag::mean >( accumulators_avoidance.z );

		csi_entity->GetPosition(position.x, position.y, position.z);
		result.x = mean_x - position.x;
		result.y = mean_y - position.y;
		result.z = mean_z - position.z;
	}
	return result;
}

void CommonSwarmIndividual::ResetAccumulators()
{
	accumulators_separation.x = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_separation.y = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_separation.z = accumulator_set<double, stats<tag::count, tag::mean> >();

	accumulators_alignment.x = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_alignment.y = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_alignment.z = accumulator_set<double, stats<tag::count, tag::mean> >();

	accumulators_cohesion.x = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_cohesion.y = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_cohesion.z = accumulator_set<double, stats<tag::count, tag::mean> >();

	accumulators_attraction.x = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_attraction.y = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_attraction.z = accumulator_set<double, stats<tag::count, tag::mean> >();

	accumulators_avoidance.x = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_avoidance.y = accumulator_set<double, stats<tag::count, tag::mean> >();
	accumulators_avoidance.z = accumulator_set<double, stats<tag::count, tag::mean> >();
}

void CommonSwarmIndividual::CheckPDULimits()
{
	float mod_acc = pow ( (double)	(csi_pdu.acceleration.x*csi_pdu.acceleration.x + 
									 csi_pdu.acceleration.y*csi_pdu.acceleration.y +
									 csi_pdu.acceleration.z*csi_pdu.acceleration.z), 0.5);

	if (mod_acc > max_acceleration)
	{
		csi_pdu.acceleration.x = max_acceleration * csi_pdu.acceleration.x/mod_acc;
		csi_pdu.acceleration.y = max_acceleration * csi_pdu.acceleration.y/mod_acc;
		csi_pdu.acceleration.z = max_acceleration * csi_pdu.acceleration.z/mod_acc;
	}

	float mod_vecc = pow ( (double)	(csi_pdu.velocity.x*csi_pdu.velocity.x + 
									 csi_pdu.velocity.y*csi_pdu.velocity.y +
									 csi_pdu.velocity.z*csi_pdu.velocity.z), 0.5);

	if (mod_vecc > max_velocity)
	{
		csi_pdu.velocity.x = max_velocity * csi_pdu.velocity.x/mod_vecc;
		csi_pdu.velocity.y = max_velocity * csi_pdu.velocity.y/mod_vecc;
		csi_pdu.velocity.z = max_velocity * csi_pdu.velocity.z/mod_vecc;
	}

}


