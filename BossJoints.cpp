#include <Core/Utils.h>

#include <Core/Application/Application.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>

#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandDefinitions.h>

#include <string>

#include "CommandCreatedHandler.h"
#include "Debug.h"
#include "Constants.h"

using namespace adsk::core;
using namespace adsk::fusion;

using namespace BossJoints;

Ptr<Application> app;
Ptr<UserInterface> ui;
static CommandCreatedHandler s_createHandler;

// create the command definition.
Ptr<CommandDefinition> createCommandDefinition()
{
    Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
    if (!commandDefinitions)
        return nullptr;

    // be fault tolerant in case the command is already added.
    Ptr<CommandDefinition> cmDef = commandDefinitions->itemById(COMMAND::ID);
    if (!cmDef)
    {
        // absolute resource file path is specified
        cmDef = commandDefinitions->addButtonDefinition(
            COMMAND::ID,
            COMMAND::NAME,
            COMMAND::DESCRIPTION,
            COMMAND::RESOURCE_PATH
        );
    }

    return cmDef;
}

extern "C" XI_EXPORT bool run(const char* context)
{
    app = Application::get();
    if (!app)
        return false;

    ui = app->userInterface();
    if (!ui)
        return false;

    Ptr<CommandDefinition> command = createCommandDefinition();
    if (!command)
        return false;

    Ptr<CommandCreatedEvent> commandCreatedEvent = command->commandCreated();
    if (!commandCreatedEvent)
        return false;
    commandCreatedEvent->add(&s_createHandler);
    command->execute();

    // prevent this module from being terminate when the script returns,
    // because we are waiting for event handlers to fire
    adsk::autoTerminate(false);

    return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
    if (ui) {
        // ui->messageBox("in stop");
        ui = nullptr;
    }

    return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#endif // XI_WIN
