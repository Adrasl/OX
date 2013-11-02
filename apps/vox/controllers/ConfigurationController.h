#ifndef _CONFIGURATIONCONTROLLER_
#define _CONFIGURATIONCONTROLLER_

#include <ApplicationConfiguration.h>
#include <vector>


class ConfigurationController
{
	public:

		ConfigurationController();
		virtual ~ConfigurationController();

		bool Load();
		bool Save();
	
	private:

		void LoadCamConfig(const std::string &value);
		void LoadDisplayConfig(const std::string &value);
		std::string ParseCamConfig();
		std::string ParseDisplayConfig();
};

#endif