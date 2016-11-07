#pragma once

#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepVertex.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>

#include <Core/UserInterface/InputChangedEventHandler.h>
#include <Core/UserInterface/InputChangedEvent.h>
#include <Core/UserInterface/InputChangedEventArgs.h>

#include <Core/UserInterface/ListItem.h>
#include <Core/UserInterface/Selection.h>
#include <Core/UserInterface/CommandInput.h>
#include <Core/UserInterface/CommandInputs.h>
#include <Core/UserInterface/GroupCommandInput.h>
#include <Core/UserInterface/DropDownCommandInput.h>
#include <Core/UserInterface/SelectionCommandInput.h>
#include <Core/UserInterface/IntegerSliderCommandInput.h>
#include <Core/UserInterface/DistanceValueCommandInput.h>
#include <Core/UserInterface/BoolValueCommandInput.h>
#include <Core/UserInterface/Selection.h>

#include "Debug.h"
#include "Constants.h"

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
    class InputChangedHandler : public InputChangedEventHandler
    {
    private:
        Ptr<BRepEdge> m_edge;
        Ptr<CommandInputs> m_inputs;

        void processStyleChange();
        void processEdgeChange();
        void processEdgeUseSelectChange();

        void updateCenterOffset();
        void setPositionGroupEnable(bool enable, bool allowEdgeSelect);

        void printInputs(const Ptr<CommandInputs>& inputs);

    public:
        InputChangedHandler();
        ~InputChangedHandler();

        // be careful, the group of inputs attached to the event args only includes
        // inputs in the same command group as the input that changed, NOT all
        // the inputs in the panel
        void notify(const Ptr<InputChangedEventArgs>& eventArgs) override;

        void setInputs(const Ptr<CommandInputs> inputs);
    };
}

inline void BossJoints::InputChangedHandler::setInputs(const Ptr<CommandInputs> inputs)
{
    m_inputs = inputs;
}
