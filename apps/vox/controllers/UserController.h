#ifndef _USERDATAMODELCONTROLLER_
#define _USERDATAMODELCONTROLLER_

//#include <Application.h>
//#include <core/IPersistence/IPersistence.h>
//#include <core/IPersistence/IUserPersistence.h>
//#include <core/IPersistence/IWorldPersistence.h>
//#include <ipersistence/types.h>
#include <boost/thread.hpp>
#include <core/types.h>
#include <vector>
//#include <SpatialIndex.h>
#include <RTreeTemplate/RTree.h>


class UserDataModelController
{
	public:
		UserDataModelController();
		~UserDataModelController();

		void Clear();
		void InsertUserDataElement(const core::UserDataElement &value);
		std::vector<core::UserDataElement> GetUserDataElements(const core::Rect3F &search_rect);
		
		static bool RegisterPointIDIntoSearchResults(int id);

		void SetHeadPosition(const core::corePoint3D<double> &value);
		void SetCenterOfMass(const core::corePoint3D<double> &value);
		void SetCenterOfMassToHead(const core::corePoint3D<double> &value);
		void SetBoundingBox(const core::corePoint3D<double> &min, const core::corePoint3D<double> &max);
		void SetSpaceCenter(const core::corePoint3D<double> &value);
		void SetThresholdDistanceToMinMax(const core::corePoint3D<double> &value);
		void SetSpaceOffset(const core::corePoint3D<double> &value);

		core::corePoint3D<double> GetHeadPosition();
		core::corePoint3D<double> GetCenterOfMass();
		core::corePoint3D<double> GetCenterOfMassToHead();
		core::corePoint3D<double> GetSpaceCenter();
		core::corePoint3D<double> GetThresholdDistanceToMinMax();
		core::corePoint3D<double> GetSpaceOffset();
		void GetBoundingBox( core::corePoint3D<double> &min, core::corePoint3D<double> &max);

	private:

		boost::shared_ptr<boost::thread> m_thread;
		boost::try_mutex m_mutex;
		
		core::corePoint3D<double> head_pos, presence_center_of_mass, com_to_head,
								  space_bounding_box_min, space_bounding_box_max, space_center, 
								  threshold_distance_to_minmax, space_offset;

		std::map<int, core::UserDataElement> user_data;
		RTree<int, float, 3, float> spatial_index;
		static std::vector<int> RTree_search_results;

};

#endif