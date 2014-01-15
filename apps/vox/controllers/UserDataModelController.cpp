#include <controllers/UserDataModelController.h>

#include <debugger.h> 

std::vector<int> UserDataModelController::RTree_search_results;

bool USerData_RegisterPointIDIntoSearchResults_callback(int id, void* arg) 
{	
	return UserDataModelController::RegisterPointIDIntoSearchResults(id);
}


UserDataModelController::UserDataModelController()
{
	instance = this;
}

UserDataModelController::~UserDataModelController()
{	
}

void UserDataModelController::Clear()
{	boost::mutex::scoped_lock lock(m_mutex);
	user_data.clear();
	RTree_search_results.clear();
	spatial_index.RemoveAll();
}

void UserDataModelController::InsertUserDataElement(const core::UserDataElement &value)
{ 
	boost::mutex::scoped_lock lock(m_mutex);

	int id = user_data.size() + 1;
	user_data[id] = value;
}

std::vector<core::UserDataElement> UserDataModelController::GetUserDataElements(const core::Rect3F &search_rect)
{	boost::mutex::scoped_lock lock(m_mutex);

	RTree_search_results.clear();
	std::vector<core::UserDataElement> result;
	int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, USerData_RegisterPointIDIntoSearchResults_callback, NULL);

	for (unsigned int i = 0; i < RTree_search_results.size(); i++)
	{	std::map< int, core::UserDataElement >::iterator iter = user_data.find(RTree_search_results[i]);
		if (iter  != user_data.end())
			result.push_back(iter->second);
	}
	return result;
}

void UserDataModelController::SetHeadPosition(const core::corePoint3D<double> &value)										{ boost::mutex::scoped_lock lock(m_mutex); head_pos = value; }
void UserDataModelController::SetCenterOfMass(const core::corePoint3D<double> &value)										{ boost::mutex::scoped_lock lock(m_mutex); presence_center_of_mass = value; }
void UserDataModelController::SetCenterOfMassToHead(const core::corePoint3D<double> &value)									{ boost::mutex::scoped_lock lock(m_mutex); com_to_head = value; }
void UserDataModelController::SetBoundingBox(const core::corePoint3D<double> &min, const core::corePoint3D<double> &max)	{ boost::mutex::scoped_lock lock(m_mutex); space_bounding_box_min = min; space_bounding_box_max = max; }
void UserDataModelController::SetSpaceCenter(const core::corePoint3D<double> &value)										{ boost::mutex::scoped_lock lock(m_mutex); space_center = value; }
void UserDataModelController::SetThresholdDistanceToMinMax(const core::corePoint3D<double> &value)							{ boost::mutex::scoped_lock lock(m_mutex); threshold_distance_to_minmax = value; }
void UserDataModelController::SetSpaceOffset(const core::corePoint3D<double> &value)										{ boost::mutex::scoped_lock lock(m_mutex); space_offset = value; }
void UserDataModelController::SetPresenceDetected(const bool &value)														{ boost::mutex::scoped_lock lock(m_mutex); presence_detected = value; }
void UserDataModelController::SetMotionElements(const std::vector<core::MotionElement> &value)								{ boost::mutex::scoped_lock lock(m_mutex); motion_elements = value; }

std::vector<core::MotionElement> UserDataModelController::GetMotionElements()							{ boost::mutex::scoped_lock lock(m_mutex); return motion_elements; }
bool UserDataModelController::GetPresenceDetected()														{ boost::mutex::scoped_lock lock(m_mutex); return presence_detected; }
core::corePoint3D<double> UserDataModelController::GetHeadPosition()									{ boost::mutex::scoped_lock lock(m_mutex); return head_pos; }
core::corePoint3D<double> UserDataModelController::GetCenterOfMass()									{ boost::mutex::scoped_lock lock(m_mutex); return presence_center_of_mass; }
core::corePoint3D<double> UserDataModelController::GetCenterOfMassToHead()								{ boost::mutex::scoped_lock lock(m_mutex); return com_to_head; }
core::corePoint3D<double> UserDataModelController::GetSpaceCenter()										{ boost::mutex::scoped_lock lock(m_mutex); return space_center; }
core::corePoint3D<double> UserDataModelController::GetThresholdDistanceToMinMax()						{ boost::mutex::scoped_lock lock(m_mutex); return threshold_distance_to_minmax; }
core::corePoint3D<double> UserDataModelController::GetSpaceOffset()										{ boost::mutex::scoped_lock lock(m_mutex); return space_offset; }
void UserDataModelController::GetBoundingBox( core::corePoint3D<double> &min, core::corePoint3D<double> &max)	{ boost::mutex::scoped_lock lock(m_mutex);  min = space_bounding_box_min; max = space_bounding_box_max; }

//RTree callback
bool UserDataModelController::RegisterPointIDIntoSearchResults(int id) 
{	//printf("Hit data rect %d\n", id);
	RTree_search_results.push_back(id);
	return true; // keep going
}