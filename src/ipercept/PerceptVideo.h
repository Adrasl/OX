#ifndef _PERCEPTVIDEO_
#define _PERCEPTVIDEO_

#include <core/IPercept/IPerceptVideo.h>
#include <core/IUserDataModelController.h>
#include <ipercept/video/MotionDetection.h>
#include <ipercept/video/FaceRecognition.h>
#include <ipercept/video/SimpleFaceDetection.h>
#include <ipercept/video/Encara2FaceDetection.h>
#include <ipercept/video/PresenceDetection.h>
#include <core/IApplicationConfiguration.h>
#include <ipercept/CamWindow.h>
#include <core/types.h>

#include <string>
#include <vector>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace core
{
	namespace ipercept
	{
		class PerceptVideo : public core::IPerceptVideo
		{
			public:

				PerceptVideo(IApplicationConfiguration *app_config_);
				virtual ~PerceptVideo();
				virtual void Delete();
				virtual void Init();
				virtual void Calibrate(const bool &value);
				virtual void CalculateHomography();
				virtual void TrainBackground();

				virtual char * GetCopyOfCurrentImage(const int camera_index, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				virtual char * GetCopyOfCurrentFeature(const std::string &feature, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				int GetNumCameras() { return capture_cam_array.size(); }

				static void ShowCamCapture(const bool &value);
				static void ShowHomography(const bool &value);
				static void ShowFaceDetection(const bool &value);
				static void ShowForeground(const bool &value);				
				static void ShowMotion(const bool &value);	

				/** \brief Record every camera into a video file in disk. */
				virtual bool SetCameraRecording(const bool &value);	
				/** \brief Use the video file url as it was the camera input. */
				virtual bool SetUseRecording(const bool &value, const std::string &video_url_basename);	
				/** \brief Obtains whether a presence has been detected or not. */
				virtual bool PresenceDetected();
				/** \brief Obtains whether a face has been detected or not. */
				virtual bool FaceDetected();
				/** \brief Obtains the position of the head. */
				virtual void GetHeadPosition(corePoint3D<double> &result);
				/** \brief Obtains lateral dominance. */
				virtual void GetMainLateralDominance(corePoint3D<double> &result);
				/** \brief Obtains orientarion. */
				virtual void GetMainOrientation(corePoint3D<double> &result);
				/** \brief Obtains eccencitricity. */
				virtual void GetMainEccentricity(corePoint3D<double> &zero_means_round);
				/** \brief Obtains the position of the given feature. */
				virtual void GetFeaturePosition(const std::string &feature, corePoint3D<double> &result);
				/** \brief Obtains position array of the given feature. */
				virtual void GetFeaturePositions(const std::string &feature, std::vector<float> &result, int &row_step, const float &scale = 1);
				/** \brief Obtains weigthed position array of the given feature. weight represents the size of the point, used as key in result map */
				virtual void GetFeatureWeightedPositions(const std::string &feature, std::map< int, std::vector<corePDU3D<double>> > &result, const float &scale=1);
				/** \brief Obtains motion elements. */
				virtual std::vector<MotionElement> PerceptVideo::GetMotionElements();
				/** \brief Obtains the Boundig Box of the detected space*/
				virtual void GetSpaceBoundingBox(corePoint3D<double> &min, corePoint3D<double> &max, const bool &recalculate=true);
				/** \brief Analizes the current face image in order to recognize whether it is of a known user or not */
				virtual int  RecognizeCurrentFace();
				/** \brief Uses latest set of faces to add a new user and retrain Face Recognizer */
				virtual void AddNewUserToRecognition(const int &user_id);
				virtual bool IsFacePoolReady();
				/** \brief Calculates presence model, point cloud of model, including additional information, such as velocity */
				virtual void CalculatePresenceModel(std::vector<float> &result, int &row_step, const float &scale);
				/** \brief Calcules UserData Elments */
				void CalculateFullUserDataElements(IUserDataModelController* user_datamodel_c);
				
			private:

				static IApplicationConfiguration *app_config;
				static IUserDataModelController *user_dataModel_controller;
				static void DoInit();
				static void DoMainLoop();
				static void Iterate();
				static void Capture();
				static void DoCalibrate();
				static void ShowDebugWindows();
				static void SendImages();
				static void ObtainCenterOfMass(corePoint3D<double> &result);
				static void ObtainPresenceVolume(std::vector<float> &result, int &row_step, const float &scale=1);
				static void ObtainPresenceVolumeAsWeightPoints(std::map< int, std::vector<corePDU3D<double>> > &weighted_points, const float &scale=1);
				static void TransformCoordinates(corePoint2D<int> &result, const int &cam_index);
				static void UpdateFaceHistory();
				static bool RegisterPointIDIntoSearchResults(int id, void* arg);

				static unsigned int num_cams;
				static std::map< int, CvCapture* > capture_cam_array;
				static std::map< int, CvCapture* > capture_videofiles_array;
				static std::map< int, CvVideoWriter* > capture_videowriter;
				static std::map< std::string, CamWindow* > camWindow_array;
				static std::map< std::string, CamWindow* > debugcamWindow_array;
				static std::map<int, IplImage *>capture_img;
				static std::map< int, CvMat* > intrinsics;
				static std::map< int, CvMat* > distortion;
				static std::map< int, IplImage* > undistort_mapx;
				static std::map< int, IplImage* > undistort_mapy;
				static std::map< int, CvMat* > homography;
				static std::map<int, vector2I> cam_capture_size;
				
				static boost::shared_ptr<boost::thread> m_thread;
				static boost::try_mutex m_mutex, bbox_mutex;
				static boost::try_mutex homography_mutex;
				static bool initialized, stop_requested, is_recording, is_using_videosource;

				static std::vector<bool> flip_h, flip_v;
				static std::vector<int> search_results;

				static IFaceRecognition * face_recognizer;
				static std::vector<core::Image> face_history;
				static std::vector<IFaceDetection*> face_detectors;
				static std::vector<IMotionDetection*> motion_detectors;
				static std::vector<IPresenceDetection*> presence_detectors;
				static bool calibrating, show_cam_capture, show_homography, show_face_detection, show_foreground, show_motion;

				static int background_trainning_frames;
				static double las_time, capture_fps;
				static double timestamp_Recording_latestframe;
				static bool presence_detected;

				static corePoint3D<double> BoundinBoxMin, BoundinBoxMax;

		};
	}
}

#endif

