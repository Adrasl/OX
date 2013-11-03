#include <igui/controllers/GUIGenericController.h>

using namespace core::igui;

GUIGenericController* GUIGenericController::instance = NULL;

GUIGenericController::GUIGenericController(MainGui *main_gui_) : main_gui(main_gui_)
{
	if (instance == NULL)
		instance = this;
}

GUIGenericController::~GUIGenericController()
{
	//if ( main_gui != NULL )  delete main_gui;
}

bool GUIGenericController::CreateUser(const std::string &name, const std::string &passwd)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->CreateUser(name, passwd);
		if (success) main_gui->PostLogMessage("User created: " + name);
	}
	return success;
}

bool GUIGenericController::DeleteUser(const std::string &name)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->DeleteUser(name);
		if (success) main_gui->PostLogMessage("User deleted: " + name);
	}
	return success;
}

bool GUIGenericController::DeleteWorld(const std::string &name)
{	
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->DeleteWorld(name);
		if (success) main_gui->PostLogMessage("World deleted: " + name);
	}
	return success;
}

bool GUIGenericController::CreateWorld(const std::string &name, const std::string &owner, const int &permissions)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->CreateWorld(name, owner, permissions);
		std::stringstream wop;
		wop << "World created: " << name << ", owner: " << owner << " permissions: " << permissions;
		if (success) main_gui->PostLogMessage(wop.str());
	}
	return success;
}

bool GUIGenericController::LoginUser(const std::string &name, const std::string &passwd)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->LoginUser(name, passwd);
		if (success) main_gui->PostLogMessage("User logged in: " + name);
	}
	return success;
}

void GUIGenericController::LogOut()
{
	if (main_gui != NULL)
	{
		main_gui->LogOut();
		main_gui->PostLogMessage("User logged out.\nBye bye...");
	}
}

bool GUIGenericController::UserExists(const std::string &name)
{
	if (main_gui != NULL)
		return main_gui->UserExists(name);
	return false;
}

bool GUIGenericController::WorldExists(const std::string &name)
{
	if (main_gui != NULL)
		return main_gui->WorldExists(name);
	return false;
}

bool GUIGenericController::RunWorld(const std::string &name)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->RunWorld(name);
		if (success) main_gui->PostLogMessage("World running: " + name);
	}
	return success;
}

bool GUIGenericController::SaveWorldPermissions(const std::string &name, const int permissions)
{
	bool success = false;
	if (main_gui != NULL)
	{
		success = main_gui->SaveWorldPermissions(name, permissions);
		std::stringstream wop;
		wop << "World permissions changed: " << name << " permissions: " << permissions;
		if (success) main_gui->PostLogMessage(wop.str());
	}
	return success;
}

core::IUserPersistence* GUIGenericController::GetCurrentUser()
{
	if (main_gui != NULL)
		return main_gui->GetCurrentUser();
	return NULL;
}

core::IWorldPersistence* GUIGenericController::GetCurrentWorld()
{
	if (main_gui != NULL)
		return main_gui->GetCurrentWorld();
	return NULL;
}

void GUIGenericController::GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions)
{
	if (main_gui != NULL)
		main_gui->GetWorldList(user_name, names, permissions);
}

void GUIGenericController::ViewUserInfoPanel()
{
	if (main_gui != NULL)
		main_gui->ViewUserInfoPanel();
}

void GUIGenericController::ViewLogPanel()
{
	if (main_gui != NULL)
		main_gui->ViewLogPanel();
}

void GUIGenericController::ViewConfigurePanel()
{
	if (main_gui != NULL)
		main_gui->ViewConfigurePanel();
}

void GUIGenericController::Calibrate()
{
	if (main_gui != NULL)
		main_gui->Calibrate();
}

void GUIGenericController::CalculateHomography()
{
	if (main_gui != NULL)
		main_gui->CalculateHomography();
}

void GUIGenericController::TrainBackground()
{
	if (main_gui != NULL)
		main_gui->TrainBackground();
}

bool GUIGenericController::SetCameraRecording(const bool &value)
{
	if (main_gui != NULL)
		return main_gui->SetCameraRecording(value);
	return false;
}
bool GUIGenericController::SetUseRecording(const bool &value, const std::string &url)
{
	if (main_gui != NULL)
		return main_gui->SetUseRecording(value, url);
	return false;
}