#ifndef _IPROD_
#define _IPROD_

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>

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
			virtual void CloseWorld() = 0; 

			virtual void SetCamerasPosition(const core::corePoint3D<double> &pos) = 0;
			virtual void SetCamerasPDU(core::corePDU3D<double> pdu) = 0;

			virtual void InsertEntityIntoScene(core::IEntityPersistence* ent) = 0;
			virtual void InsertEntityIntoScene(core::IEntityPersistence* ent, std::vector<float> source_data, int row_step=3) = 0;
			virtual void SetAvatar(const std::string model) = 0;
			virtual void SetAvatar(std::vector<float> source_data, int row_step=3) = 0;
			virtual void SetAvatar(void *graphic_node) = 0;
			virtual void* CreateGraphicNode(std::vector<float> source_data, int row_step=3) = 0;
			virtual void* CreateGraphicNode(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data) = 0;

		private:


	};
}


#endif

