#include <mainpercept.h>
#include <iostream>

#include <debugger.h> 

using namespace core;
using namespace core::ipercept;

#define THRESHOLD 0.25
#define OFFSET_STEP 0.25



MainPercept::MainPercept(IApplicationConfiguration *app_config_) : app_config(app_config_), isReady(false), perceptAudio_module(NULL), perceptVideo_module(NULL), user_dataModel_controller(NULL)
{

}

MainPercept::~MainPercept()
{
	delete perceptAudio_module;
	delete perceptVideo_module;
}

void MainPercept::Delete()
{
	if (perceptAudio_module)
		perceptAudio_module->Delete();
	if (perceptVideo_module)
		perceptVideo_module->Delete();
}

void MainPercept::Init()
{
	if (perceptVideo_module == NULL)
		perceptVideo_module = new PerceptVideo(app_config);

	perceptVideo_module->Init();

	DoInit();

	isReady=true;
}

void MainPercept::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&MainPercept::DoMainLoop, this ) ));
	}
}

void MainPercept::DoMainLoop()
{
	initialized = true;
	
	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void MainPercept::Iterate()
{	
	boost::try_mutex::scoped_lock lock(m_mutex);
	if (lock && user_dataModel_controller && perceptVideo_module)
	{
		corePoint3D<double> head_position, presence_center_of_mass, com_to_head;

		GetHeadPosition(head_position);
		GetFeaturePosition("CENTER OF MASS", presence_center_of_mass);
		com_to_head.x = head_position.x - presence_center_of_mass.x;
		com_to_head.y = head_position.y - presence_center_of_mass.y;
		com_to_head.z = head_position.z - presence_center_of_mass.z;
	
		std::vector<MotionElement> motion_elements = GetMotionElements();

		user_dataModel_controller->SetPresenceDetected(PresenceDetected());
		user_dataModel_controller->SetMotionElements(motion_elements);
		user_dataModel_controller->SetHeadPosition(head_position);
		user_dataModel_controller->SetCenterOfMass(presence_center_of_mass);
		user_dataModel_controller->SetCenterOfMassToHead(com_to_head);

		perceptVideo_module->CalculateFullUserDataElements(user_dataModel_controller);
	}
	else
		user_dataModel_controller = app->GetUserDataModelControllerInstance();
}

void MainPercept::CalculatePointCloud()
{
	if (user_dataModel_controller && perceptVideo_module && app_config)
	{

	}
}

void MainPercept::Calibrate(const bool &value)
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->Calibrate(value);
}

void MainPercept::CalculateHomography()
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->CalculateHomography();
}

void MainPercept::TrainBackground()
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->TrainBackground();
}

bool MainPercept::SetCameraRecording(const bool &value)
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->SetCameraRecording(value);
	return false;
}

bool MainPercept::SetUseRecording(const bool &value, const std::string &video_url_basename)
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->SetUseRecording(value, video_url_basename);
	return false;
}

void MainPercept::GetHeadPosition(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetHeadPosition(result);
}

void MainPercept::GetMainLateralDominance(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetMainLateralDominance(result);
}

void MainPercept::GetMainOrientation(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetMainOrientation(result);
}

void MainPercept::GetMainEccentricity(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetMainEccentricity(result);
}

void MainPercept::GetFeaturePosition(const std::string &feature, corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetFeaturePosition(feature, result);
}

void MainPercept::GetFeaturePositions(const std::string &feature, std::vector<float> &result, int &row_step, const float &scale)
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetFeaturePositions(feature, result, row_step, scale);
}

void MainPercept::GetFeatureWeightedPositions(const std::string &feature, std::map< int, std::vector<corePDU3D<double>> > &result, const float &scale)
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->GetFeatureWeightedPositions(feature, result, scale);
}

std::vector<MotionElement> MainPercept::GetMotionElements()
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->GetMotionElements();
	
	std::vector<MotionElement> empty;
	return empty;
}

bool MainPercept::PresenceDetected()
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->PresenceDetected();
	return false;
}

bool MainPercept::FaceDetected()
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->FaceDetected();
	return false;
}

void MainPercept::GetSpaceBoundingBox(corePoint3D<double> &min, corePoint3D<double> &max, const bool &recalculate)
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->GetSpaceBoundingBox(min, max, recalculate);
}

char * MainPercept::GetCopyOfCurrentFeature(const std::string &feature, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->GetCopyOfCurrentFeature(feature, size_x, size_y, n_channels, depth, width_step, switch_rb);
	return NULL;
}

int  MainPercept::RecognizeCurrentFace()
{
	if (isReady && perceptVideo_module != NULL)
		return perceptVideo_module->RecognizeCurrentFace();
	return -1;
}

void MainPercept::AddNewUserToRecognition(const int &user_id)
{
	if (isReady && perceptVideo_module != NULL)
		perceptVideo_module->AddNewUserToRecognition(user_id);
}

bool MainPercept::IsFacePoolReady()
{
	if (isReady && perceptVideo_module)
		return perceptVideo_module->IsFacePoolReady();
	return false;
}
