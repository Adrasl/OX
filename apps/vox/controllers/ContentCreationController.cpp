#include <controllers/ContentCreationController.h>

#include <debugger.h> 

#define CCTIMELAPSE 0.1

ContentCreationController *ContentCreationController::instance = NULL;
IApplication* ContentCreationController::app = NULL;
double ContentCreationController::time_start = 0;

ContentCreationController::ContentCreationController()
{}


ContentCreationController *ContentCreationController::Instance ()
{
  if (!instance)
  {
    instance = new ContentCreationController;
	time_start = (double)clock()/CLOCKS_PER_SEC;
  }

  return instance;
}

void ContentCreationController::Update()
{
	double timestamp = (double)clock()/CLOCKS_PER_SEC;
	double dif_time = timestamp - time_start;

	if (dif_time >= CCTIMELAPSE)
	{
		//change theme of the world
		//create new entities and insert them into the world
		app;


		//core::ipersistence::EntityPersistence genesis("GENESIS");
		//genesis.SetPsique(NatureOfEntity::STANDALONE);
		//genesis.SetModelData("teapot");
		//genesis.SetPosition(0,20,0);
		//genesis.SetScale(1.5);
		//genesis.Save();

		//default_world.AddEntity( (*(core::IEntityPersistence*)&flower) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&genesis) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&field ) );
		//default_world.AddEntity( (*(core::IEntityPersistence*)&panda ) );
		//default_world.Save();
		//-----------------------------


		//cout << "CONTENT CREATION LOOP: " << dif_time << "\n";
		time_start = timestamp;
	}
}