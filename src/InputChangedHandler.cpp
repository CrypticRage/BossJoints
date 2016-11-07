#include "InputChangedHandler.h"

using namespace BossJoints;

BossJoints::InputChangedHandler::InputChangedHandler()
{
}

BossJoints::InputChangedHandler::~InputChangedHandler()
{
}

void BossJoints::InputChangedHandler::printInputs(const Ptr<CommandInputs>& inputs)
{
    XTRACE(L"==================\n");
    XTRACE(L"input count: %i\n", inputs->count());
    XTRACE(L"==================\n");
    
    for (int i = 0; i < inputs->count(); ++i)
    {
        Ptr<CommandInput> input = inputs->item(i);
        XTRACE(L"input %i: %ls\n", i, (LPCTSTR)input->id().c_str());
    }
}

void BossJoints::InputChangedHandler::notify(const Ptr<InputChangedEventArgs>& eventArgs)
{
    if (m_inputs == nullptr)
    {
        return;
    }

    printInputs(m_inputs);
    Ptr<CommandInput> input = eventArgs->input();

    // code to react to the event.
    if (input->id() == STYLE_SELECT::ID)
    {
        processStyleChange();
    }
    else if (input->id() == EDGE_SELECT::ID)
    {
        processEdgeChange();
    }
    else if (input->id() == CENTER_OFFSET_SELECT::ID)
    {
        updateCenterOffset();
    }
    else if (input->id() == EDGE_USE_SELECT::ID)
    {
        processEdgeUseSelectChange();
    }
}

void BossJoints::InputChangedHandler::processEdgeChange()
{
    XTRACE(L"Edge changed!");
    Ptr<SelectionCommandInput> edgeSelectInput = m_inputs->itemById(EDGE_SELECT::ID);
    Ptr<DistanceValueCommandInput> centerOffsetValueInput = m_inputs->itemById(CENTER_OFFSET_VALUE::ID);
    Ptr<BoolValueCommandInput> centerOffsetSelectInput = m_inputs->itemById(CENTER_OFFSET_SELECT::ID);
    Ptr<DistanceValueCommandInput> widthValueInput = m_inputs->itemById(WIDTH_VALUE::ID);

    if (edgeSelectInput->selectionCount() == 0)
    {
        setPositionGroupEnable(false, false);
    }

    else if (edgeSelectInput->selectionCount() > 1)
    {
        return;
    }

    else
    {
        m_edge = edgeSelectInput->selection(0)->entity();
        setPositionGroupEnable(true, true);
    }
}

void BossJoints::InputChangedHandler::processStyleChange()
{
    XTRACE(L"Style changed!");
    Ptr<DropDownCommandInput> styleSelectInput = m_inputs->itemById(STYLE_SELECT::ID);
    Ptr<IntegerSliderCommandInput> toothCountInput = m_inputs->itemById(TOOTH_COUNT::ID);
    Ptr<IntegerSliderCommandInput> gapCountInput = m_inputs->itemById(GAP_COUNT::ID);

    if (styleSelectInput->selectedItem()->name() == STYLE_SELECT::OPTION_TOOTH)
    {
        toothCountInput->isEnabled(true);
        gapCountInput->isEnabled(false);

        toothCountInput->isVisible(true);
        gapCountInput->isVisible(false);
    }
    else if (styleSelectInput->selectedItem()->name() == STYLE_SELECT::OPTION_GAP)
    {
        toothCountInput->isEnabled(false);
        gapCountInput->isEnabled(true);

        toothCountInput->isVisible(false);
        gapCountInput->isVisible(true);
    }
}

void BossJoints::InputChangedHandler::processEdgeUseSelectChange()
{
    Ptr<BoolValueCommandInput> edgeUseSelectInput = m_inputs->itemById(EDGE_USE_SELECT::ID);
    bool useEntireEdge = edgeUseSelectInput->value();

    setPositionGroupEnable(!useEntireEdge, useEntireEdge);
}


void BossJoints::InputChangedHandler::updateCenterOffset()
{
    Ptr<DistanceValueCommandInput> centerOffsetValueInput = m_inputs->itemById(CENTER_OFFSET_VALUE::ID);
    Ptr<BoolValueCommandInput> centerOffsetSelectInput = m_inputs->itemById(CENTER_OFFSET_SELECT::ID);

    Ptr<Point3D> edgeStartPoint = m_edge->startVertex()->geometry();
    Ptr<Point3D> edgeEndPoint = m_edge->endVertex()->geometry();
    Ptr<Vector3D> edgeVector = edgeStartPoint->vectorTo(edgeEndPoint);
    double edgeLength = edgeVector->length();

    if (centerOffsetSelectInput->value())
    {
        centerOffsetValueInput->value(edgeLength / 2);
        centerOffsetValueInput->isEnabled(false);
    }
    else
    {
        centerOffsetValueInput->isEnabled(true);
        centerOffsetValueInput->setManipulator(edgeStartPoint, edgeVector);
        centerOffsetValueInput->minimumValue(0.0);
        centerOffsetValueInput->maximumValue(edgeLength);
        centerOffsetValueInput->value(0.0);
    }
}

void BossJoints::InputChangedHandler::setPositionGroupEnable(bool enable, bool allowEdgeSelect)
{
    Ptr<BoolValueCommandInput> edgeUseSelectInput = m_inputs->itemById(EDGE_USE_SELECT::ID);
    Ptr<BoolValueCommandInput> centerOffsetSelectInput = m_inputs->itemById(CENTER_OFFSET_SELECT::ID);
    Ptr<DistanceValueCommandInput> centerOffsetValueInput = m_inputs->itemById(CENTER_OFFSET_VALUE::ID);
    Ptr<DistanceValueCommandInput> widthValueInput = m_inputs->itemById(WIDTH_VALUE::ID);

    if (!enable)
    {
        edgeUseSelectInput->isEnabled(allowEdgeSelect);

        centerOffsetSelectInput->isEnabled(false);
        centerOffsetSelectInput->value(false);

        centerOffsetValueInput->value(0.0);
        centerOffsetValueInput->isEnabled(false);

        widthValueInput->isEnabled(false);
        widthValueInput->value(0.0);
    }
    else
    {
        edgeUseSelectInput->isEnabled(true);
        edgeUseSelectInput->value(false);

        centerOffsetSelectInput->isEnabled(true);
        centerOffsetSelectInput->value(false);

        centerOffsetValueInput->value(0.0);
        centerOffsetValueInput->isEnabled(false);

        updateCenterOffset();

        widthValueInput->isEnabled(true);
        widthValueInput->value(0.0);
    }
}
