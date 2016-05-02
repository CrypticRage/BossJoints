#include "FilletBase.h"

FilletBase::FilletBase(
    const double diameter, const Ptr<Point3D>& centerPoint
) {
    m_diameter = diameter;
    m_centerPoint = centerPoint->copy();
}

FilletBase::FilletBase()
{
}

FilletBase::~FilletBase()
{
}

void FilletBase::drawSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    if (!sketch || !m_centerPoint)
        return;

    if (!sketch->isValid() || !m_centerPoint->isValid())
        return;

    if (m_diameter <= 0.0)
        return;

    Ptr<SketchCircle> outline;
    outline = sketch->sketchCurves()->sketchCircles()->addByCenterRadius(
        sketch->modelToSketchSpace(m_centerPoint),
        m_diameter / 2
    );
    isOk = outline->isFixed(true);
    if (!isOk) return;
}
