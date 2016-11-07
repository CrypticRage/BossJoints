#pragma once

#include <Core/Utils.h>

#include <Core/UserInterface/CommandEvent.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/CommandEventHandler.h>

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
    class DestroyEventHandler : public CommandEventHandler
    {
    public:
        DestroyEventHandler();
        ~DestroyEventHandler();

        void notify(const Ptr<CommandEventArgs>& eventArgs) override
        {
            adsk::terminate();
        }
    };
}
