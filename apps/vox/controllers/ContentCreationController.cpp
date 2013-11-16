#include <controllers/ContentCreationController.h>

#include <debugger.h> 

ContentCreationController *ContentCreationController::instance = NULL;

ContentCreationController::ContentCreationController()
{}


ContentCreationController *ContentCreationController::Instance ()
{
  if (!instance)
    instance = new ContentCreationController;

  return instance;
}
