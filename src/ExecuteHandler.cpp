#include "ExecuteHandler.h"

using namespace BossJoints;

BossJoints::ExecuteHandler::ExecuteHandler()
{
}


BossJoints::ExecuteHandler::~ExecuteHandler()
{
}

void BossJoints::ExecuteHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    Ptr<Command> command = eventArgs->firingEvent()->sender();
    if (!command)
        return;

    Ptr<CommandInputs> inputs = command->commandInputs();

    // we need access to the inputs within a command during the execute.
    Ptr<SelectionCommandInput> planeInput =                 inputs->itemById(PLANE_SELECT::ID);
    if (!planeInput) return;
    Ptr<SelectionCommandInput> edgeInput =                  inputs->itemById(EDGE_SELECT::ID);
    if (!edgeInput) return;

    Ptr<BoolValueCommandInput> useEntireEdgeInput =         inputs->itemById(EDGE_USE_SELECT::ID);
    if (!useEntireEdgeInput) return;
    Ptr<BoolValueCommandInput> centerOffsetSelectInput =    inputs->itemById(CENTER_OFFSET_SELECT::ID);
    if (!centerOffsetSelectInput) return;
    Ptr<DistanceValueCommandInput> centerOffsetValueInput = inputs->itemById(CENTER_OFFSET_VALUE::ID);
    if (!centerOffsetValueInput) return;
    Ptr<DistanceValueCommandInput> widthValueInput =        inputs->itemById(WIDTH_VALUE::ID);
    if (!widthValueInput) return;

    Ptr<DropDownCommandInput> styleInput =                  inputs->itemById(STYLE_SELECT::ID);
    if (!styleInput) return;
    Ptr<IntegerSliderCommandInput>  toothCountInput =       inputs->itemById(TOOTH_COUNT::ID);
    if (!toothCountInput) return;
    Ptr<IntegerSliderCommandInput>  gapCountInput =         inputs->itemById(GAP_COUNT::ID);
    if (!gapCountInput) return;

    Ptr<ValueCommandInput> matThicknessInput =              inputs->itemById(MATERIAL_THICKNESS::ID);
    if (!matThicknessInput) return;
    Ptr<ValueCommandInput> toolDiamInput =                  inputs->itemById(TOOL_DIAMETER::ID);
    if (!toolDiamInput) return;
    Ptr<ValueCommandInput> wiggleRoomInput =                inputs->itemById(WIGGLE_ROOM::ID);
    if (!wiggleRoomInput) return;

    Ptr<BRepFace> plane = planeInput->selection(0)->entity();
    Ptr<BRepEdge> edge = edgeInput->selection(0)->entity();
    Ptr<ListItem> styleItem = styleInput->selectedItem();
    std::string styleString = styleItem->name();

    if (m_boxJoint != NULL)
    {
        delete m_boxJoint;
        m_boxJoint = NULL;
    }

    m_boxJoint = Joint::create(plane, edge, styleString);
    if (m_boxJoint == NULL)
    {
        return;
    }

    bool useEntireEdge = useEntireEdgeInput->value();
    double centerOffset = 0.0;
    double width = 0.0;
    if (!useEntireEdge)
    {
        centerOffset = centerOffsetValueInput->value();
        width = widthValueInput->value();

        m_boxJoint->setWidth(width);
        m_boxJoint->setCenterOffset(centerOffset);
    }

    int toothCount = toothCountInput->valueOne();
    int gapCount = gapCountInput->valueOne();

    double matThickness = matThicknessInput->value();
    double toolDiameter = toolDiamInput->value();
    double wiggleRoom = wiggleRoomInput->value();

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
    m_boxJoint->setMatThickness(matThickness);
    m_boxJoint->setToolDiameter(toolDiameter);

    m_boxJoint->sketch();
    m_boxJoint->extrude();
}
