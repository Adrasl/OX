#ifndef _IPERCEPT_
#define _IPERCEPT_

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IPercept
	{
		public:

			virtual ~IPercept(){}
			virtual void Delete()=0;
			virtual void Init()=0;
			virtual void SetApp(IApplication *app_) = 0 ;
			virtual void SetAppConfig(IApplicationConfiguration *app_config_) = 0;
			//virtual void Capture()=0;
			virtual void Calibrate(const bool &value)=0;
			virtual void CalculateHomography()=0;
			virtual void TrainBackground()=0;
			virtual bool SetCameraRecording(const bool &value)=0;	
			virtual bool SetUseRecording(const bool &value, const std::string &video_url_basename)=0;

			/** \brief Obtains whether a presence has been detected or not. */
			virtual bool PresenceDetected() = 0;
			/** \brief Obtains whether a Face has been detected or not. */
			virtual bool FaceDetected() = 0;
			/** \brief Obtains the position of the head. */
			virtual void GetHeadPosition(corePoint3D<double> &result) = 0;
			/** \brief Obtains lateral dominance. */
			virtual void GetMainLateralDominance(corePoint3D<double> &result)=0;
			/** \brief Obtains orientarion. */
			virtual void GetMainOrientation(corePoint3D<double> &result)=0;
			/** \brief Obtains eccencitricity. */
			virtual void GetMainEccentricity(corePoint3D<double> &zero_means_round)=0;
			/** \brief Obtains the position of the given feature. */
			virtual void GetFeaturePosition(const std::string &feature, corePoint3D<double> &result) = 0;
			/** \brief Obtains the position of the given feature. */
			virtual void GetFeaturePositions(const std::string &feature, std::vector<float> &result, int &row_step, const float &scale = 1) = 0;
			/** \brief Obtains the position of the given feature. */
			virtual void GetFeatureWeightedPositions(const std::string &feature, std::map< int, std::vector<corePDU3D<double>> > &result, const float &scale) = 0;
			/** \brief Obtains the Boundig Box of the detected space*/
			virtual void GetSpaceBoundingBox(corePoint3D<double> &min, corePoint3D<double> &max, const bool &recalculate=true) = 0;
			/** \brief Obtains the motion elements */
			virtual std::vector<MotionElement> GetMotionElements() = 0;

			/** \brief Obtains the most recent image of the requested feature*/
			virtual char * GetCopyOfCurrentFeature(const std::string &feature, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			/** \brief Analizes the current face image in order to recognize whether it is of a known user or not */
			virtual int  RecognizeCurrentFace() = 0;
			/** \brief Uses latest set of faces to add a new user and retrain Face Recognizer */
			virtual void AddNewUserToRecognition(const int &user_id) = 0;
			virtual bool IsFacePoolReady() = 0;
	};
}


#endif

