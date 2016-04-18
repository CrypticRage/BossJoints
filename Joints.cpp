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

#include "BoxJoint.h"
#include "Debug.h"

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "BoxJoints";
const std::string commandName = "Create A Box Joint";
const std::string commandDescription = "Create a box joint.";

#if 0
for x in range(0, toothCount) :
if x == 0 :
sp3.translateBy(v1)

sketch.sketchCurves.sketchLines.addTwoPointRectangle(
sketch.modelToSketchSpace(sp1),
sketch.modelToSketchSpace(sp3)
)

sp1.translateBy(v1)
sp1.translateBy(v1)
sp3.translateBy(v1)
sp3.translateBy(v1)
#endif

#if 0
boxLines = sketch.sketchCurves.sketchLines.addTwoPointRectangle(p1, p4)
for line in boxLines :
line.isConstruction = True
#endif

// CommandExecuted event handler.
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
            Ptr<CommandInput> planeInputBase = inputs->itemById(commandId + "_plane_selection");
            Ptr<SelectionCommandInput> planeInput = planeInputBase;
            Ptr<CommandInput> edgeInputBase = inputs->itemById(commandId + "_edge_selection");
            Ptr<SelectionCommandInput> edgeInput = edgeInputBase;
            Ptr<CommandInput> toothCountInputBase = inputs->itemById(commandId + "_tooth_count");
            Ptr<IntegerSliderCommandInput> toothCountInput = toothCountInputBase;
            Ptr<ValueCommandInput> matThicknessInput = inputs->itemById(commandId + "_material_thickness");
            Ptr<ValueCommandInput> toolDiamInput = inputs->itemById(commandId + "_tool_diameter");

            unsigned int toothCount = 10;
            Ptr<BRepFace> plane;
            Ptr<BRepEdge> edge;
            double matThickness = 0.01;
            double toolDiameter = 0.01;

            if (!toothCountInput || !planeInput || !edgeInput || !matThicknessInput || !toolDiamInput)
            {
                ui->messageBox("One of the inputs doesn't exist.");
            }
            else
            {
                toothCount = (unsigned int)(toothCountInput->valueOne());
                plane = planeInput->selection(0)->entity();
                edge = edgeInput->selection(0)->entity();
                matThickness = matThicknessInput->value();
                toolDiameter = toolDiamInput->value();
            }

            m_boxJoint = BoxJoint::create(plane, edge, toothCount, matThickness);
            
            if (m_boxJoint != NULL)
            {
                m_boxJoint->createBorderSketch();
                m_boxJoint->createGapSketch();
                m_boxJoint->createFilletSketch(toolDiameter);
                // m_boxJoint->extrudeGaps();
            }
        }
};

// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
    public:
        void notify(const Ptr<CommandEventArgs>& eventArgs) override
        {
            adsk::terminate();
        }
};

#if 0
global handlers

# Keep the handler referenced beyond this function
handlers.append(onDestroy)
handlers.append(onExecute)            inputs = cmd.commandInputs
#endif

// CommandCreated event handler.
class BoxJointCommandCreatedHandler : public CommandCreatedEventHandler
{
    private:
        OnExecuteEventHander m_onExecuteHandler;
        OnDestroyEventHandler m_onDestroyHandler;

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

            // Create tooth count input
            inputs->addIntegerSliderCommandInput(commandId + "_tooth_count", "Tooth Count", 1, 20);

#if 0
            // Create tooth depth input
            Ptr<DistanceValueCommandInput> toothDepthInput = inputs->addDistanceValueCommandInput(
                commandId + "_tooth_depth",
                "Tooth Depth",
                ValueInput::createByReal(.01)
            );
            if (!toothDepthInput)
                return;
            // toothDepthInput->setManipulator(Point3D::create(0, 0, 0), Vector3D::create(1, 0, 0));
            toothDepthInput->minimumValue(0);
            toothDepthInput->isMinimumValueInclusive(false);
            toothDepthInput->maximumValue(10);
            toothDepthInput->isMaximumValueInclusive(true);
#endif

            // Create material thickness input
            Ptr<ValueCommandInput> materialThicknessInput = inputs->addValueInput(
                commandId + "_material_thickness",
                "Material Thickness",
                "in",
                ValueInput::createByString("0.5 in")
            );
            if (!materialThicknessInput)
                return;
            /*
            materialThicknessInput->setManipulator(Point3D::create(0, 0, 0), Vector3D::create(1, 0, 0));
            materialThicknessInput->minimumValue(0);
            materialThicknessInput->isMinimumValueInclusive(false);
            materialThicknessInput->maximumValue(10);
            materialThicknessInput->isMaximumValueInclusive(true);
            */

            // Create tool diameter input
            Ptr<ValueCommandInput> toolDiameterInput = inputs->addValueInput(
                commandId + "_tool_diameter",
                "Tool Diameter",
                "in",
                ValueInput::createByString("0.125 in")
            );
            if (!toolDiameterInput)
                return;
        }
};
static BoxJointCommandCreatedHandler s_commandCreatedHandler;

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
