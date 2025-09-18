#if WITH_EDITOR

#pragma once 


#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FEditorUtilityModule : public IModuleInterface
{
public:

	virtual void StartupModule()override;	
	virtual void ShutdownModule()override;		

private:

	TSharedPtr<class FUICommandList> IGCCommandList;


};

#endif