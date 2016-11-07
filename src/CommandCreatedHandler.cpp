#include "CommandCreatedHandler.h"

using namespace BossJoints;

BossJoints::CommandCreatedHandler::CommandCreatedHandler()
{
}

BossJoints::CommandCreatedHandler::~CommandCreatedHandler()
{
}

void BossJoints::CommandCreatedHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    bool isOk = false;

    if (!eventArgs) return;

    Ptr<Command> command = eventArgs->command();

    if (!command) return;

    // add the execute handler
    Ptr<CommandEvent> onExecute = command->execute();
    if (!onExecute) return;

    isOk = onExecute->add(&m_executeHandler);
    if (!isOk) return;

    // add the destroy handler
    Ptr<CommandEvent> onDestroy = command->destroy();
    if (!onDestroy) return;

    isOk = onDestroy->add(&m_destroyHandler);
    if (!isOk) return;

    // add the input changed handler
    Ptr<InputChangedEvent> inputChangedEvent = command->inputChanged();
    if (!inputChangedEvent) return;

    isOk = inputChangedEvent->add(&m_inputChangedHandler);
    if (!isOk) return;

    // create all inputs
    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs) return;

    // create plane selection input
    Ptr<SelectionCommandInput> planeSelectionInput = inputs->addSelectionInput(
        PLANE_SELECT::ID,
        PLANE_SELECT::NAME,
        PLANE_SELECT::PROMPT
    );
    planeSelectionInput->setSelectionLimits(1, 1);
    planeSelectionInput->addSelectionFilter("PlanarFaces");

    // create edge selection input
    Ptr<SelectionCommandInput> edgeSelectionInput = inputs->addSelectionInput(
        EDGE_SELECT::ID,
        EDGE_SELECT::NAME,
        EDGE_SELECT::PROMPT
    );
    edgeSelectionInput->setSelectionLimits(1, 1);
    edgeSelectionInput->addSelectionFilter("LinearEdges");

    // create input sub groups
    createPositionGroup(inputs);
    createStyleGroup(inputs);
    createDimGroup(inputs);

    m_inputChangedHandler.setInputs(inputs);
}

void BossJoints::CommandCreatedHandler::createPositionGroup(const Ptr<CommandInputs>& inputs)
{
    // create group input for joint position
    Ptr<GroupCommandInput> positionGroupInput = inputs->addGroupCommandInput(
        POSITION_GROUP::ID,
        POSITION_GROUP::NAME
    );
    if (!positionGroupInput) return;
    positionGroupInput->isExpanded(true);
    positionGroupInput->isEnabledCheckBoxDisplayed(false);

    Ptr<CommandInputs> groupChildInputs = positionGroupInput->children();
    if (!groupChildInputs) return;

    // create edge use select input
    Ptr<BoolValueCommandInput> useEntireEdge = groupChildInputs->addBoolValueInput(
        EDGE_USE_SELECT::ID,
        EDGE_USE_SELECT::NAME,
        true,
        "",
        true
    );
    useEntireEdge->isEnabled(false);

    // create center offset select input
    Ptr<BoolValueCommandInput> offsetAtCenter = groupChildInputs->addBoolValueInput(
        CENTER_OFFSET_SELECT::ID,
        CENTER_OFFSET_SELECT::NAME,
        true,
        "",
        false
    );
    offsetAtCenter->isEnabled(false);

    // create distance value input
    Ptr<DistanceValueCommandInput> offsetValueInput = groupChildInputs->addDistanceValueCommandInput(
        CENTER_OFFSET_VALUE::ID,
        CENTER_OFFSET_VALUE::NAME,
        ValueInput::createByReal(0.0)
    );
    if (!offsetValueInput) return;
    offsetValueInput->isEnabled(false);

    // create distance value input
    Ptr<DistanceValueCommandInput> widthValueInput = groupChildInputs->addDistanceValueCommandInput(
        WIDTH_VALUE::ID,
        WIDTH_VALUE::NAME,
        ValueInput::createByReal(0.0)
        );
    if (!widthValueInput) return;
    widthValueInput->isEnabled(false);
}

void BossJoints::CommandCreatedHandler::createStyleGroup(const Ptr<CommandInputs>& inputs)
{
    // create group input for joint style
    Ptr<GroupCommandInput> styleGroupInput = inputs->addGroupCommandInput(
        STYLE_GROUP::ID,
        STYLE_GROUP::NAME
    );
    if (!styleGroupInput) return;
    styleGroupInput->isExpanded(true);
    styleGroupInput->isEnabledCheckBoxDisplayed(false);

    Ptr<CommandInputs> groupChildInputs = styleGroupInput->children();
    if (!groupChildInputs) return;

    // create style input
    Ptr<DropDownCommandInput> styleInput = groupChildInputs->addDropDownCommandInput(
        STYLE_SELECT::ID,
        STYLE_SELECT::NAME,
        DropDownStyles::TextListDropDownStyle
    );
    if (!styleInput) return;

    Ptr<ListItems> styleItems = styleInput->listItems();
    if (!styleItems) return;
    styleItems->add(STYLE_SELECT::OPTION_TOOTH, true, "");
    styleItems->add(STYLE_SELECT::OPTION_GAP, false, "");

    // create tooth count input
    Ptr<IntegerSliderCommandInput> toothCountInput = groupChildInputs->addIntegerSliderCommandInput(
        TOOTH_COUNT::ID,
        TOOTH_COUNT::NAME,
        2,
        20
    );

    // create gap count input
    Ptr<IntegerSliderCommandInput> gapCountInput = groupChildInputs->addIntegerSliderCommandInput(
        GAP_COUNT::ID,
        GAP_COUNT::NAME,
        2,
        20
    );
    gapCountInput->isVisible(false);
    gapCountInput->isEnabled(false);
}

void BossJoints::CommandCreatedHandler::createDimGroup(const Ptr<CommandInputs>& inputs)
{
    // create group input for joint dimensions
    Ptr<GroupCommandInput> dimGroupInput = inputs->addGroupCommandInput(
        DIMENSION_GROUP::ID,
        DIMENSION_GROUP::NAME
    );
    if (!dimGroupInput) return;
    dimGroupInput->isExpanded(true);
    dimGroupInput->isEnabledCheckBoxDisplayed(false);

    Ptr<CommandInputs> groupChildInputs = dimGroupInput->children();
    if (!groupChildInputs) return;

    // create material thickness input
    Ptr<ValueCommandInput> materialThicknessInput = groupChildInputs->addValueInput(
        MATERIAL_THICKNESS::ID,
        MATERIAL_THICKNESS::NAME,
        "in",
        ValueInput::createByString("0.5 in")
    );
    if (!materialThicknessInput) return;

    // create tool diameter input
    Ptr<ValueCommandInput> toolDiameterInput = groupChildInputs->addValueInput(
        TOOL_DIAMETER::ID,
        TOOL_DIAMETER::NAME,
        "in",
        ValueInput::createByString("0.125 in")
    );
    if (!toolDiameterInput) return;

    // create wiggle room input
    Ptr<ValueCommandInput> wiggleRoomInput = groupChildInputs->addValueInput(
        WIGGLE_ROOM::ID,
        WIGGLE_ROOM::NAME,
        "in",
        ValueInput::createByString("0.005 in")
    );
    if (!wiggleRoomInput) return;
}
