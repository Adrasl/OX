#ifndef _COMMON_SWARMINDIVIDUAL_
#define _COMMON_SWARMINDIVIDUAL_

#include <icog/CommonObstacle.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>

#include <boost/parameter/aux_/parameter_requirements.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/thread.hpp>

#include <RTreeTemplate/RTree.h>

#include <string>
#include <vector>
#include <iostream> 
#include <map>
#include <cmath> 

using namespace boost::accumulators;

namespace core
{
	namespace icog
	{
		class CommonSwarmIndividual 
		{
			public:

				CommonSwarmIndividual() {};
				CommonSwarmIndividual(const int &species_, core::IEntityPersistence* entity_, 
									  const core::corePDU3D<double> &initial_pdu, 
									  const float &max_velocity = 1.0f, const float &max_acceleration = 1.0f, 
									  const float &perception_distance_ = 100.0f);

				virtual ~CommonSwarmIndividual();

				static void		SetEcosystem(const std::map<int, std::vector<core::IEntityPersistence*>> &ecosystem_);
				static bool		Register_SearchResults(int id);

				virtual int		GetSpecies()	{return my_species;}
				virtual void	AddForce(const corePoint3D<float> &force);
				virtual void	UseWorldLimits(const core::corePoint3D<float> &max, const core::corePoint3D<float> &min );
				virtual void	DontUseWorldLimits();
			

			protected:

				static void ClassifyEcosystem();
				static void ClearEcosystem();

				virtual void Think(); // By default it will fly with the flock, 
									  // and stay away from other species. 
									  // Override as preferred.
				virtual core::corePoint3D<float> CalculateSeparation();   
				virtual core::corePoint3D<float> CalculateAlignment();      
				virtual core::corePoint3D<float> CalculateCohesion();   
				virtual core::corePoint3D<float> CalculateAttraction(); 
				virtual core::corePoint3D<float> CalculateAvoidance();	
				virtual void					 ResetAccumulators();
				virtual void					 CheckPDULimits();

				//shared knowledge
				static std::map<int, core::IEntityPersistence*> individuals_by_spatialIndex;
				static std::map<core::IEntityPersistence*, int> species_of_individual;
				static std::map<int, std::vector<core::IEntityPersistence*>> ecosystem;

				static RTree<int, float, 3, float> RTree_Everything_spatialIndexes;
				static std::map<int, RTree<int, float, 3, float>*> RTree_BySpecies_SpatialIndexes;

				static boost::mutex csi_mutex;

				//own info
				int my_species;
				core::IEntityPersistence* csi_entity;
				core::corePDU3D<double> csi_pdu;
				float max_acceleration, max_velocity;
				float perception_distance;
				corePoint3D<float> separation, alignment, cohesion, 
								   attraction, avoidance, worldlimits;

				corePoint3D<accumulator_set<double, stats<tag::count, tag::mean> >> accumulators_separation,
																					accumulators_alignment, 
																					accumulators_cohesion, 
																					accumulators_attraction,
																					accumulators_avoidance;
				float separation_distance, 
					  avoidance_distance;
				bool use_world_limits;
				corePoint3D<float> world_max, world_min;

				static std::vector<int> RTree_search_results;

		};
	}
}

#endif