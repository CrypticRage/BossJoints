#include "CornerFillet.h"

CornerFillet::CornerFillet(
    const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& widthVector,
    const Ptr<Vector3D>& thicknessVector, const double diameter
) {
    m_refPoint = refPoint->copy();
    m_widthVector = widthVector->copy();
    m_thicknessVector = thicknessVector->copy();
    
    m_filletBase.setDiameter(diameter);

    calcLayoutVectors();
    calcPoints();
}

CornerFillet::CornerFillet()
{
}

CornerFillet::~CornerFillet()
{
}

void CornerFillet::calcPoints()
{
    bool isOk = false;

    // find the front point
    m_frontPoint = m_refPoint->copy();
    isOk = m_frontPoint->translateBy(m_thicknessVector);
    if (!isOk) return;

    // find the back half-gap point
    m_backGapPoint = m_refPoint->copy();
    isOk = m_backGapPoint->translateBy(m_halfWidthVector);
    if (!isOk) return;

    // find the front half-gap point
    m_frontGapPoint = m_frontPoint->copy();
    isOk = m_frontGapPoint->translateBy(m_halfWidthVector);
    if (!isOk) return;

    // find the point to display the angle dimension text
    m_angleTextPoint = m_refPoint->copy();
    isOk = m_angleTextPoint->translateBy(m_quarterWidthPerpVector);
    if (!isOk) return;
    isOk = m_angleTextPoint->translateBy(m_tinyWidthVector);
    if (!isOk) return;
}

void CornerFillet::calcLayoutVectors()
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

void CornerFillet::drawSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    Ptr<Point3D> sk_refPoint = sketch->modelToSketchSpace(m_refPoint);
    Ptr<Point3D> sk_frontPoint = sketch->modelToSketchSpace(m_frontPoint);
    Ptr<Point3D> sk_frontGapPoint = sketch->modelToSketchSpace(m_frontGapPoint);
    Ptr<Point3D> sk_backGapPoint = sketch->modelToSketchSpace(m_backGapPoint);
    Ptr<Point3D> sk_angleTextPoint = sketch->modelToSketchSpace(m_angleTextPoint);

    // draw a line from the reference point to the front point
    Ptr<SketchLine> perpLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(sk_refPoint, sk_frontPoint);
    isOk = perpLine->isConstruction(true);
    if (!isOk) return;
    isOk = perpLine->isFixed(true);
    if (!isOk) return;

    // draw a line from the reference point to the back half-gap point
    Ptr<SketchLine> paraLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(sk_refPoint, sk_backGapPoint);
    isOk = paraLine->isConstruction(true);
    if (!isOk) return;
    isOk = paraLine->isFixed(true);
    if (!isOk) return;

    // draw the half gap line
    Ptr<SketchLine> gapLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(sk_frontGapPoint, sk_backGapPoint);
    isOk = gapLine->isConstruction(true);
    if (!isOk) return;
    isOk = gapLine->isFixed(true);
    if (!isOk) return;

    // draw the diagonal ref line that the fillet will center on
    Ptr<SketchLine> diagLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(sk_refPoint, sk_frontGapPoint);
    isOk = diagLine->isConstruction(true);
    if (!isOk) return;
    isOk = diagLine->startSketchPoint()->isFixed(true);

    // add constraints to the diagonal ref line
    sketch->geometricConstraints()->addCoincident(diagLine->endSketchPoint(), gapLine);
    Ptr<SketchAngularDimension> angularDim = sketch->sketchDimensions()->addAngularDimension(perpLine, diagLine, sk_angleTextPoint);
    Ptr<ModelParameter> angleParam = angularDim->parameter();
    isOk = angleParam->expression("45 deg");
    if (!isOk) return;

    // draw the fillet outline
    Ptr<SketchPoint> centerPoint = sketch->sketchPoints()->add(sk_refPoint);
    sketch->geometricConstraints()->addCoincident(centerPoint, diagLine);

    Ptr<SketchLinearDimension> lengthConstraint;
    lengthConstraint = sketch->sketchDimensions()->addDistanceDimension(
        centerPoint,
        diagLine->endSketchPoint(),
        DimensionOrientations::AlignedDimensionOrientation,
        sk_frontGapPoint
    );

    double constraintLength = diagLine->length() - (m_filletBase.diameter() / 2);
    Ptr<ModelParameter> lengthParam = lengthConstraint->parameter();
    lengthParam->value(constraintLength);

    m_filletBase.setCenterPoint(sketch->sketchToModelSpace(centerPoint->geometry()));
    m_filletBase.drawSketch(sketch);
}
