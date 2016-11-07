#pragma once

#include <string>

#include <Core/Utils.h>

#include <Core/UserInterface/ListItem.h>
#include <Core/UserInterface/Selection.h>

#include <Core/UserInterface/Command.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/CommandEventHandler.h>

#include <Core/UserInterface/CommandInputs.h>
#include <Core/UserInterface/ValueCommandInput.h>
#include <Core/UserInterface/StringValueCommandInput.h>
#include <Core/UserInterface/GroupCommandInput.h>
#include <Core/UserInterface/BoolValueCommandInput.h>
#include <Core/UserInterface/DropDownCommandInput.h>
#include <Core/UserInterface/SelectionCommandInput.h>
#include <Core/UserInterface/IntegerSliderCommandInput.h>
#include <Core/UserInterface/DistanceValueCommandInput.h>

#include "Constants.h"
#include "Joint.h"

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
    class ExecuteHandler : public CommandEventHandler
    {
    private:
        Joint *m_boxJoint;

    public:
        ExecuteHandler();
        ~ExecuteHandler();

        void notify(const Ptr<CommandEventArgs>& eventArgs) override;
    };
}
