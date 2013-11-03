#ifndef _MAINPERCEPT_
#define _MAINPERCEPT_

//#include <gmtl/gmtl.h>
//#include <gmtl/Matrix.h>

#include <core/IPercept/IPercept.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <core/IUserDataModelController.h>
#include <ipercept/PerceptAudio.h>
#include <ipercept/PerceptVideo.h>

#include <string>
#include <vector>

class Application;

namespace core
{
	namespace ipercept
	{
		class MainPercept : public core::IPercept
		{
			public:

				MainPercept(IApplicationConfiguration *app_config_);
				virtual ~MainPercept();
				virtual void Delete();
				virtual void SetApp(IApplication *app_) {app = app_;}
				virtual void SetAppConfig(IApplicationConfiguration *app_config_) {app_config = app_config_;}
				virtual void Init();
				virtual void Calibrate(const bool &value);
				virtual void CalculateHomography();
				virtual void TrainBackground();
				virtual bool SetCameraRecording(const bool &value);	
				virtual bool SetUseRecording(const bool &value, const std::string &url);
				
				/** \brief Obtains whether a presence has been detected or not. */
				virtual bool MainPercept::PresenceDetected();
				/** \brief Obtains whether a Face has been detected or not. */
				virtual bool MainPercept::FaceDetected();
				/** \brief Obtains the position of the head. */
				virtual void GetHeadPosition(corePoint3D<double> &result);
				/** \brief Obtains the position of the given feature. */
				virtual void GetFeaturePosition(const std::string &feature, corePoint3D<double> &result);
				/** \brief Obtains the position of the given feature. */
				virtual void GetFeaturePositions(const std::string &feature, std::vector<float> &result, int &row_step, const float &scale = 1);
				/** \brief Obtains the position of the given feature. */
				virtual void GetFeatureWeightedPositions(const std::string &feature, std::map< int, std::vector<vector3F> > &result, const float &scale);
				/** \brief Obtains the motion elements. */
				virtual std::vector<MotionElement> GetMotionElements();
				/** \brief Obtains the Boundig Box of the detected space*/
				virtual void GetSpaceBoundingBox(corePoint3D<double> &min, corePoint3D<double> &max, const bool &recalculate=true);

				/** \brief Obtains the most recent image of the requested feature*/
				virtual char * GetCopyOfCurrentFeature(const std::string &feature, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				/** \brief Analizes the current face image in order to recognize whether it is of a known user or not */
				virtual int  RecognizeCurrentFace();
				/** \brief Uses latest set of faces to add a new user and retrain Face Recognizer */
				virtual void AddNewUserToRecognition(const int &user_id);
				virtual bool IsFacePoolReady();

			private:

				IApplication *app;
				IApplicationConfiguration *app_config;
				IUserDataModelController *user_dataModel_controller;
				PerceptAudio* perceptAudio_module;
				PerceptVideo* perceptVideo_module;

			private:

				void DoInit();
				void DoMainLoop();
				void Iterate();

				void CalculatePointCloud();
				void InitMatrixTransform();

				boost::shared_ptr<boost::thread> m_thread;
				boost::try_mutex m_mutex;
				bool initialized, stop_requested;
				bool isReady;

				//gmtl::Matrix<float, 4, 4>	MatrixTransform_CubeFRONT, MatrixTransform_CubeBACK,
				//			MatrixTransform_CubeLEFT, MatrixTransform_CubeRIGHT,
				//			MatrixTransform_CubeTOP, MatrixTransform_CubeBOTTOM,
				//			MatrixTransform;

		};
	}
}

#endif

