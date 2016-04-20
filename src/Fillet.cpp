#include "Fillet.h"

Fillet::Fillet(
    const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& widthVector,
    const Ptr<Vector3D>& thicknessVector, const double toolDiameter
) {
    m_refPoint = refPoint;
    m_widthVector = widthVector;
    m_thicknessVector = thicknessVector;
    m_toolDiameter = toolDiameter;

    m_centerPoint = Point3D::create(0.0, 0.0, 0.0);

    updateSubVectors();
}

Fillet::Fillet()
{
}

Fillet::~Fillet()
{
}

void Fillet::updateSubVectors()
{
    bool isOk = false;
    
    m_halfWidthVector = m_widthVector->copy();
    isOk = m_halfWidthVector->scaleBy(0.5);
    if (!isOk) return;

    m_tinyWidthVector = m_widthVector->copy();
    isOk = m_tinyWidthVector->scaleBy(0.02);
    if (!isOk) return;

    m_quarterWidthPerpVector = m_thicknessVector->copy();
    isOk = m_quarterWidthPerpVector->normalize();
    if (!isOk) return;
    isOk = m_quarterWidthPerpVector->scaleBy(m_widthVector->length() / 4);
    if (!isOk) return;
}

void Fillet::drawSketch(const Ptr<Sketch>& sketch, const Ptr<Point3D>& centerPoint)
{
    bool isOk = false;

    if (!sketch || !centerPoint)
        return;

    if (!sketch->isValid() || !centerPoint->isValid())
        return;

    if (m_toolDiameter <= 0.0)
        return;

    m_centerPoint = centerPoint;

    Ptr<SketchCircle> outline;
    outline = sketch->sketchCurves()->sketchCircles()->addByCenterRadius(centerPoint, m_toolDiameter / 2);
    isOk = outline->isFixed(true);
    if (!isOk) return;
}

void Fillet::drawRefSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    // find the front point
    Ptr<Point3D> frontPoint = m_refPoint->copy();
    isOk = frontPoint->translateBy(m_thicknessVector);
    if (!isOk) return;

    // draw a line from the reference point to the front point
    Ptr<SketchLine> perpLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(m_refPoint, frontPoint);
    isOk = perpLine->isConstruction(true);
    if (!isOk) return;
    isOk = perpLine->isFixed(true);
    if (!isOk) return;

    // find the back half-gap point
    Ptr<Point3D> backGapPoint = m_refPoint->copy();
    isOk = backGapPoint->translateBy(m_halfWidthVector);
    if (!isOk) return;

    // draw a line from the reference point to the back half-gap point
    Ptr<SketchLine> paraLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(m_refPoint, backGapPoint);
    isOk = paraLine->isConstruction(true);
    if (!isOk) return;
    isOk = paraLine->isFixed(true);
    if (!isOk) return;

    // find the back half-gap point
    Ptr<Point3D> frontGapPoint = frontPoint->copy();
    isOk = frontGapPoint->translateBy(m_halfWidthVector);
    if (!isOk) return;

    // draw the half gap line
    Ptr<SketchLine> gapLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(frontGapPoint, backGapPoint);
    isOk = gapLine->isConstruction(true);
    if (!isOk) return;
    isOk = gapLine->isFixed(true);
    if (!isOk) return;

    // draw the diagonal line that the fillet will center on
    Ptr<SketchLine> diagLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(m_refPoint, frontGapPoint);
    isOk = diagLine->isConstruction(true);
    if (!isOk) return;
    isOk = diagLine->startSketchPoint()->isFixed(true);

    // find the point to display the angle dimension text
    Ptr<Point3D> angleTextPoint = m_refPoint->copy();
    isOk = angleTextPoint->translateBy(m_quarterWidthPerpVector);
    if (!isOk) return;
    isOk = angleTextPoint->translateBy(m_tinyWidthVector);
    if (!isOk) return;

    // add constraints to the diagonal reference line
    sketch->geometricConstraints()->addCoincident(diagLine->endSketchPoint(), gapLine);
    Ptr<SketchAngularDimension> angularDim = sketch->sketchDimensions()->addAngularDimension(perpLine, diagLine, angleTextPoint);
    Ptr<ModelParameter> angleParam = angularDim->parameter();
    isOk = angleParam->expression("45 deg");
    if (!isOk) return;

    // draw the fillet outline
    Ptr<SketchPoint> centerPoint = sketch->sketchPoints()->add(m_refPoint);
    sketch->geometricConstraints()->addCoincident(centerPoint, diagLine);

    Ptr<SketchLinearDimension> lengthConstraint;
    lengthConstraint = sketch->sketchDimensions()->addDistanceDimension(
        centerPoint,
        diagLine->endSketchPoint(),
        DimensionOrientations::AlignedDimensionOrientation,
        frontGapPoint
    );

    double constraintLength = diagLine->length() - (m_toolDiameter / 2);
    Ptr<ModelParameter> lengthParam = lengthConstraint->parameter();
    lengthParam->value(constraintLength);

    drawSketch(sketch, centerPoint->geometry());
}
