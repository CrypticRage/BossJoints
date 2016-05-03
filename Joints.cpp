#include <Core/Utils.h>

#include <Core/Application/ApplicationEvents.h>
#include <Core/Application/Application.h>
#include <Core/Application/Documents.h>
#include <Core/Application/Document.h>
#include <Core/Application/Product.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/ValueInput.h>

#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandCreatedEvent.h>
#include <Core/UserInterface/CommandCreatedEventArgs.h>
#include <Core/UserInterface/CommandCreatedEventHandler.h>
#include <Core/UserInterface/InputChangedEvent.h>
#include <Core/UserInterface/InputChangedEventHandler.h>
#include <Core/UserInterface/InputChangedEventArgs.h>
#include <Core/UserInterface/CommandEvent.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/CommandEventHandler.h>
#include <Core/UserInterface/Command.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/CommandInputs.h>
#include <Core/UserInterface/ValueCommandInput.h>
#include <Core/UserInterface/StringValueCommandInput.h>
#include <Core/UserInterface/TabCommandInput.h>
#include <Core/UserInterface/GroupCommandInput.h>
#include <Core/UserInterface/BoolValueCommandInput.h>
#include <Core/UserInterface/ButtonRowCommandInput.h>
#include <Core/UserInterface/DropDownCommandInput.h>
#include <Core/UserInterface/SelectionCommandInput.h>
#include <Core/UserInterface/FloatSliderCommandInput.h>
#include <Core/UserInterface/IntegerSliderCommandInput.h>
#include <Core/UserInterface/FloatSpinnerCommandInput.h>
#include <Core/UserInterface/DistanceValueCommandInput.h>
#include <Core/UserInterface/IntegerSpinnerCommandInput.h>
#include <Core/UserInterface/RadioButtonGroupCommandInput.h>
#include <Core/UserInterface/ImageCommandInput.h>
#include <Core/UserInterface/ListItems.h>
#include <Core/UserInterface/ListItem.h>
#include <Core/UserInterface/Selection.h>
#include <Core/UserInterface/Selections.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/BoundingBox3D.h>

#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchArcs.h>
#include <Fusion/Sketch/SketchArc.h>
#include <Fusion/Sketch/GeometricConstraints.h>
#include <Fusion/Sketch/PerpendicularConstraint.h>

#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/PathPatternFeatures.h>
#include <Fusion/Features/PathPatternFeatureInput.h>

#include <Fusion/Fusion/Design.h>
#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepVertex.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Construction/ConstructionPlane.h>

#include <string>

#include "BoxJoint.h"
#include "Debug.h"

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "Joints";
const std::string commandName = "Create A Joint";
const std::string commandDescription = "Create a joint.";


// OnExecuteEventHander
class OnExecuteEventHander : public adsk::core::CommandEventHandler
{
private:
    BoxJoint *m_boxJoint;

public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override
    {
        Ptr<Command> command = eventArgs->firingEvent()->sender();
        if (!command)
            return;
            
        Ptr<CommandInputs> inputs = command->commandInputs();

        // We need access to the inputs within a command during the execute.
        Ptr<SelectionCommandInput> planeInput = inputs->itemById(commandId + "_plane_selection");
        Ptr<SelectionCommandInput> edgeInput = inputs->itemById(commandId + "_edge_selection");
        Ptr<IntegerSliderCommandInput>  toothCountInput = inputs->itemById(commandId + "_tooth_count");
        Ptr<IntegerSliderCommandInput>  gapCountInput = inputs->itemById(commandId + "_gap_count");
        Ptr<ValueCommandInput> matThicknessInput = inputs->itemById(commandId + "_material_thickness");
        Ptr<ValueCommandInput> toolDiamInput = inputs->itemById(commandId + "_tool_diameter");
        Ptr<ValueCommandInput> wiggleRoomInput = inputs->itemById(commandId + "_wiggle_room");
        Ptr<DropDownCommandInput> styleInput = inputs->itemById(commandId + "_style");

        int toothCount = 5;
        int gapCount = 5;
        Ptr<BRepFace> plane;
        Ptr<BRepEdge> edge;
        double matThickness = 0.5;
        double toolDiameter = 0.125;
        double wiggleRoom = 0.004;
        Ptr<ListItem> styleItem;
        std::string styleString;

        if (!toothCountInput || !gapCountInput || !planeInput || !edgeInput ||
            !matThicknessInput || !toolDiamInput || !wiggleRoomInput || !styleInput)
        {
            ui->messageBox("One of the inputs doesn't exist.");
            return;
        }

        toothCount = toothCountInput->valueOne();
        gapCount = gapCountInput->valueOne();
        plane = planeInput->selection(0)->entity();
        edge = edgeInput->selection(0)->entity();
        matThickness = matThicknessInput->value();
        toolDiameter = toolDiamInput->value();
        wiggleRoom = wiggleRoomInput->value();
        styleItem = styleInput->selectedItem();
        styleString = styleItem->name();

        if (m_boxJoint != NULL)
        {
            delete m_boxJoint;
            m_boxJoint = NULL;
        }

        m_boxJoint = BoxJoint::create(plane, edge, matThickness);
        if (m_boxJoint == NULL)
        {
            return;
        }

        if (toothCountInput->isVisible())
        {
            XTRACE(L"setting tooth count : (%i)\n", toothCount);
            m_boxJoint->setToothCount((unsigned int)toothCount);
        }
        if (gapCountInput->isVisible())
        {
            XTRACE(L"setting gap count : (%i)\n", gapCount);
            m_boxJoint->setGapCount((unsigned int)gapCount);
        }
        m_boxJoint->setWiggleRoom(wiggleRoom);
        m_boxJoint->setStyle(styleString);

        m_boxJoint->createBorderSketch();
        m_boxJoint->createGapSketch();
        m_boxJoint->createFilletSketch(toolDiameter);
        m_boxJoint->extrudeGaps();
        m_boxJoint->extrudeFillets();
    }
};

// OnDestroyEventHandler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
    public:
        void notify(const Ptr<CommandEventArgs>& eventArgs) override
        {
            adsk::terminate();
        }
};

// Event handler for the inputChanged event.
class OnInputChangedEventHandler : public InputChangedEventHandler
{
private:
    Ptr<IntegerSliderCommandInput> m_toothCountInput;
    Ptr<IntegerSliderCommandInput> m_gapCountInput;

public:
    void notify(const Ptr<InputChangedEventArgs>& eventArgs) override
    {
        // Code to react to the event.
        if (eventArgs->input()->name() == "Style")
        {
            Ptr<DropDownCommandInput> styleInput = eventArgs->input();
            if (styleInput->selectedItem()->name() == "Start With Tooth")
            {
                m_toothCountInput->isEnabled(true);
                m_gapCountInput->isEnabled(false);

                m_toothCountInput->isVisible(true);
                m_gapCountInput->isVisible(false);
            }
            else if (styleInput->selectedItem()->name() == "Start With Gap")
            {
                m_toothCountInput->isEnabled(false);
                m_gapCountInput->isEnabled(true);

                m_toothCountInput->isVisible(false);
                m_gapCountInput->isVisible(true);
            }
        }
    }

    void setCountInputs(const Ptr<IntegerSliderCommandInput>& gapInput, const Ptr<IntegerSliderCommandInput>& toothInput)
    {
        m_toothCountInput = toothInput;
        m_gapCountInput = gapInput;
    }
};

// JointsCommandCreatedHandler
class JointsCommandCreatedHandler : public CommandCreatedEventHandler
{
private:
    Ptr<DropDownCommandInput> m_styleInput;
    Ptr<IntegerSliderCommandInput> m_gapCountInput;
    Ptr<IntegerSliderCommandInput> m_toothCountInput;

    OnExecuteEventHander m_onExecuteHandler;
    OnDestroyEventHandler m_onDestroyHandler;
    OnInputChangedEventHandler m_onInputChangedHandler;

public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
    {
        if (!eventArgs)
            return;

        Ptr<Command> command = eventArgs->command();

        if (!command)
            return;

        Ptr<CommandEvent> onDestroy = command->destroy();
        if (!onDestroy)
            return;

        bool isOk = onDestroy->add(&m_onDestroyHandler);
        if (!isOk)
            return;

        Ptr<CommandEvent> onExecute = command->execute();
        if (!onExecute)
            return;

        isOk = onExecute->add(&m_onExecuteHandler);
        if (!isOk)
            return;

        Ptr<CommandInputs> inputs = command->commandInputs();
        if (!inputs)
            return;

        Ptr<InputChangedEvent> inputChangedEvent = command->inputChanged();
        if (!inputChangedEvent)
            return;

        isOk = inputChangedEvent->add(&m_onInputChangedHandler);
        if (!isOk) return;

        // Create plane selection input
        Ptr<SelectionCommandInput> planeSelectionInput = inputs->addSelectionInput(
            commandId + "_plane_selection",
            "Select Plane",
            "Select the primary plane..."
        );
        planeSelectionInput->setSelectionLimits(1, 1);
        planeSelectionInput->addSelectionFilter("PlanarFaces");

        // Create edge selection input
        Ptr<SelectionCommandInput> edgeSelectionInput = inputs->addSelectionInput(
            commandId + "_edge_selection",
            "Select Edge",
            "Select the primary edge..."
        );
        edgeSelectionInput->setSelectionLimits(1, 1);
        edgeSelectionInput->addSelectionFilter("LinearEdges");

        // Create style input
        m_styleInput = inputs->addDropDownCommandInput(
            commandId + "_style",
            "Style",
            DropDownStyles::TextListDropDownStyle
            );
        if (!m_styleInput)
            return;
        Ptr<ListItems> styleItems = m_styleInput->listItems();
        if (!styleItems)
            return;
        styleItems->add("Start With Tooth", true, "");
        styleItems->add("Start With Gap", false, "");

        // Create tooth count input
        m_toothCountInput = inputs->addIntegerSliderCommandInput(commandId + "_tooth_count", "Tooth Count", 2, 20);

        // Create gap count input
        m_gapCountInput = inputs->addIntegerSliderCommandInput(commandId + "_gap_count", "Gap Count", 2, 20);
        m_gapCountInput->isVisible(false);
        m_gapCountInput->isEnabled(false);
        m_onInputChangedHandler.setCountInputs(m_gapCountInput, m_toothCountInput);

        // Create material thickness input
        Ptr<ValueCommandInput> materialThicknessInput = inputs->addValueInput(
            commandId + "_material_thickness",
            "Material Thickness",
            "in",
            ValueInput::createByString("0.5 in")
        );
        if (!materialThicknessInput)
            return;

        // Create tool diameter input
        Ptr<ValueCommandInput> toolDiameterInput = inputs->addValueInput(
            commandId + "_tool_diameter",
            "Tool Diameter",
            "in",
            ValueInput::createByString("0.125 in")
        );
        if (!toolDiameterInput)
            return;
        
        // Create wiggle room input
        Ptr<ValueCommandInput> wiggleRoomInput = inputs->addValueInput(
            commandId + "_wiggle_room",
            "Wiggle Room",
            "in",
            ValueInput::createByString("0.005 in")
        );
        if (!wiggleRoomInput)
            return;
    }
};
static JointsCommandCreatedHandler s_commandCreatedHandler;

// Create the command definition.
Ptr<CommandDefinition> createCommandDefinition()
{
    Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
    if (!commandDefinitions)
        return nullptr;

    // Be fault tolerant in case the command is already added.
    Ptr<CommandDefinition> cmDef = commandDefinitions->itemById(commandId);
    if (!cmDef)
    {
        std::string resourcePath = "./resources"; // absolute resource file path is specified
        cmDef = commandDefinitions->addButtonDefinition(
            commandId,
            commandName,
            commandDescription,
            resourcePath
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
    commandCreatedEvent->add(&s_commandCreatedHandler);
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
