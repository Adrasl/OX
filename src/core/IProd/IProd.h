#ifndef _IPROD_
#define _IPROD_

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <core/types.h>
#include <core/IProd/IEntity.h>

namespace core
{
	class _COREEXPORT_ IProd
	{
		public:

			virtual ~IProd(){}
			virtual void Delete()=0;
			virtual void SetApp(IApplication *app_) = 0 ;
			virtual void SetAppConfig(IApplicationConfiguration *app_config_) = 0;
			virtual void Init()=0;
			virtual void DoStuff()=0;
			virtual void OpenWindow()=0;
			virtual void PostLogMessage(const std::string &message) = 0;
			virtual bool RunWorld(core::IUserPersistence  *user, core::IWorldPersistence *world) = 0;
			virtual void LoadEntityFromCurrentWorld(core::IEntity * ent) = 0;
			virtual void InsertEntityIntoCurrentWorld(core::IEntity * ent, const double &after_seconds = 0.0f)=0;
			virtual void RemoveEntityFromCurrentWorld(core::IEntity * ent)=0;
			virtual void CloseWorld() = 0; 

			virtual void SetUserPosition(const core::corePoint3D<double> &pos) = 0;
			virtual void SetCamerasPDU(core::corePDU3D<double> pdu) = 0;


			virtual void InsertEntityIntoScene(core::IEntityPersistence* ent) = 0;
			virtual void InsertEntityIntoScene(core::IEntityPersistence* ent, std::vector<float> source_data, int row_step=3) = 0;
			virtual void SetAvatar(const std::string model) = 0;
			virtual void SetAvatar(std::vector<float> source_data, int row_step=3) = 0;
			virtual void SetAvatar(void *graphic_node) = 0;
			virtual void* CreateGraphicNode(std::vector<float> source_data, int row_step=3) = 0;
			virtual void* CreateGraphicNode(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data) = 0;
			virtual void RegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data) = 0; //retomar regiter entity
			virtual void CreateAndRegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data) = 0; //retomar regiter entity

			virtual std::vector<int> GetBackgroundSounds() = 0;
			virtual int  AddBackgroundSound(const std::string &file_name, const double &time_lerp=0.0) = 0;
			virtual void SetPitchBackgroundSound(const int &id, const float &value, const double &time_lerp=0.0) = 0;
			virtual void SetAmplitudeBackgroundSound(const int &id, const float &value, const double &time_lerp=0.0) = 0;
			virtual void RemoveBackgroundSound(const int &id, const double &time_lerp=0.0) = 0;
			virtual void RemoveAllBackgroundSound(const double &time_lerp=0.0) = 0;

			virtual void SetBackgroundColor(const float &R, const float &G, const float &B) = 0;
			virtual void SetFogColor(const float &R, const float &G, const float &B) = 0;
			virtual void SetFogIntensity(const float &intensity) = 0;
			virtual void SetBackgroundAndFog(const float &bg_R=0.5f, const float &bg_G=0.5f, const float &bg_B=0.5f, const float &f_R=0.5f, const float &f_G=0.5f, const float &f_B=0.5f, const float &intensity=0.5f, const float animation_time=.0f) =0;

		private:


	};
}


#endif

