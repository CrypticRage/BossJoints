#pragma once

#include <Core/UserInterface/CommandCreatedEvent.h>
#include <Core/UserInterface/CommandCreatedEventArgs.h>
#include <Core/UserInterface/CommandCreatedEventHandler.h>

#include <Core/UserInterface/ListItems.h>
#include <Core/UserInterface/DropDownCommandInput.h>
#include <Core/UserInterface/IntegerSliderCommandInput.h>

#include "DestroyEventHandler.h"
#include "InputChangedHandler.h"
#include "ExecuteHandler.h"
#include "Constants.h"

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
    class CommandCreatedHandler : public CommandCreatedEventHandler
    {
    private:
        ExecuteHandler m_executeHandler;
        DestroyEventHandler m_destroyHandler;
        InputChangedHandler m_inputChangedHandler;

        void createPositionGroup(const Ptr<CommandInputs>& inputs);
        void createStyleGroup(const Ptr<CommandInputs>& inputs);
        void createDimGroup(const Ptr<CommandInputs>& inputs);

    public:
        void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override;

    public:
        CommandCreatedHandler();
        ~CommandCreatedHandler();
    };
}