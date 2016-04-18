#include "BoxJoint.h"

BoxJoint::BoxJoint()
{
}


BoxJoint::~BoxJoint()
{
}

BoxJoint *BoxJoint::create(
    const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge,
    unsigned int toothCount, double matThickness
) {
    // check to make sure the selected edge borders the selected face
    bool foundEdge = false;

    for (Ptr<BRepEdge> search_edge : plane->edges())
    {
        if (edge == search_edge)
            foundEdge = true;
    }

    if (!foundEdge)
        return NULL;

    BoxJoint *ptr = new BoxJoint();
    ptr->setPlane(plane);
    ptr->setEdge(edge);
    ptr->setToothCount(toothCount);
    ptr->setMatThickness(matThickness);

    // Ptr<BoxJoint> smart_ptr;
    // smart_ptr.reset(ptr, true);

    return ptr;
}

// find the adjustment vector
Ptr<Vector3D> BoxJoint::findAdjustmentVector(const Ptr<SketchLine>& line, double length)
{
    Ptr<Point3D> sp = line->startSketchPoint()->geometry();
    Ptr<Point3D> ep = line->endSketchPoint()->geometry();

    Ptr<Vector3D> v1 = sp->vectorTo(ep);
    v1->normalize();
    v1->scaleBy(length);

    return v1;
}

// extrude the gaps of the joint
bool BoxJoint::extrudeGaps()
{
    Ptr<ObjectCollection> profileCollection = ProfileUtil::stripBorderProfile(m_profileSketch, m_borderProfile);

    // create an extrusion input to be able to define the input needed for an extrusion
    // while specifying the profile and that a new component is to be created
    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Features> feats = comp->features();
    if (!feats)
        return false;
    Ptr<ExtrudeFeatures> extrudes = feats->extrudeFeatures();
    if (!extrudes)
        return false;
    Ptr<ExtrudeFeatureInput> extInput = extrudes->createInput(
        profileCollection,
        FeatureOperations::CutFeatureOperation
        );
    if (!extInput)
        return false;

    // define the extent as the given material thickness
    Ptr<ValueInput> distance = ValueInput::createByReal(-m_matThickess);
    if (!distance)
        return false;
    extInput->setDistanceExtent(false, distance);

    // create the extrusion
    Ptr<ExtrudeFeature> ext = extrudes->add(extInput);
    if (!ext)
        return false;

    return true;
}

Ptr<SketchCircle> BoxJoint::drawFilletOutline(Ptr<Sketch>& sketch, const Ptr<Point3D>& centerPoint, const double radius)
{
    if (!sketch || !centerPoint)
        return NULL;

    if (!sketch->isValid() || !centerPoint->isValid())
        return NULL;

    if (radius <= 0.0)
        return NULL;

    Ptr<SketchCircle> outline;
    outline = sketch->sketchCurves()->sketchCircles()->addByCenterRadius(centerPoint, radius);

    return outline;
}

// create the sketch for all the dogbone fillets
void BoxJoint::createFilletSketch(const double toolDiameter)
{
    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    sketch->name("fillet_profiles");

    Ptr<Point3D> p1 = sketch->modelToSketchSpace(m_edge->startVertex()->geometry());
    Ptr<Point3D> p2 = sketch->modelToSketchSpace(m_edge->endVertex()->geometry());
    Ptr<Point3D> p3 = sketch->modelToSketchSpace(m_backPoint);
    double distance = p1->distanceTo(p2);

    unsigned int gapCount = m_toothCount - 1;
    double toothWidth = distance / (double)(m_toothCount + gapCount);
    double gapWidth = toothWidth;

    Ptr<Vector3D> fullToothVector = p1->vectorTo(p2);

    bool isOk = fullToothVector->normalize();
    if (!isOk)
        return;

    isOk = fullToothVector->scaleBy(toothWidth);
    if (!isOk)
        return;

    Ptr<Vector3D> halfToothVector = fullToothVector->copy();
    isOk = halfToothVector->scaleBy(0.5);
    if (!isOk)
        return;

    Ptr<Vector3D> fullGapVector = fullToothVector->copy();

    Ptr<Vector3D> halfGapVector = fullGapVector->copy();
    isOk = halfGapVector->scaleBy(0.5);
    if (!isOk)
        return;

    Ptr<Vector3D> tinyGapVector = fullGapVector->copy();
    isOk = tinyGapVector->scaleBy(0.01);
    if (!isOk)
        return;

    Ptr<Vector3D> negTinyGapVector = tinyGapVector->copy();
    isOk = negTinyGapVector->scaleBy(-1.0);
    if (!isOk)
        return;

    Ptr<Vector3D> fullThicknessVector = p3->vectorTo(p1);

    Ptr<Vector3D> quarterGapPerpVector = fullThicknessVector->copy();
    isOk = quarterGapPerpVector->normalize();
    if (!isOk)
        return;
    isOk = quarterGapPerpVector->scaleBy(0.25 * gapWidth);
    if (!isOk)
        return;

    Ptr<Vector3D> quarterThicknessVector = fullThicknessVector->copy();
    isOk = quarterThicknessVector->scaleBy(0.25);
    if (!isOk)
        return;

    Ptr<Vector3D> halfThicknessVector = fullThicknessVector->copy();
    isOk = halfThicknessVector->scaleBy(0.5);
    if (!isOk)
        return;

    Ptr<Point3D> nearPoint;
    Ptr<Point3D> midPoint;
    Ptr<Point3D> farPoint;
    Ptr<Point3D> nearAngleTextPoint;
    Ptr<Point3D> farAngleTextPoint;

    Ptr<SketchLine> nearLine;
    Ptr<SketchLine> nearDiagLine;
    Ptr<SketchLine> midLine;
    Ptr<SketchLine> farLine;
    Ptr<SketchLine> farDiagLine;

    for (unsigned int i = 0; i < gapCount; i++)
    {
        // we have a tooth at the end, so we need to move past it
        p1->translateBy(fullToothVector);
        p3->translateBy(fullToothVector);

        // save the current position of p3 as the near corner point
        nearPoint = p3->copy();

        // draw the first straight reference line for the fillet on the
        // nearest edge of the gap
        nearLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p1, nearPoint);
        nearLine->isConstruction(true);
        nearLine->isFixed(true);

        // create the anchor point for the near 45 degree angle constraint text
        nearAngleTextPoint = p3->copy();
        nearAngleTextPoint->translateBy(quarterGapPerpVector);
        nearAngleTextPoint->translateBy(tinyGapVector);

        // move to the midpoint of the gap
        p1->translateBy(halfGapVector);
        p3->translateBy(halfGapVector);

        // draw a line through the middle of the gap
        midPoint = p3->copy();
        isOk = midPoint->translateBy(halfThicknessVector);
        if (!isOk)
            return;

        midLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p1, p3);
        midLine->isConstruction(true);
        midLine->isFixed(true);

        // draw the diagonal line we'll use for the near fillet
        nearDiagLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(nearPoint, midPoint);
        nearDiagLine->isConstruction(true);
        nearDiagLine->startSketchPoint()->isFixed(true);
        sketch->geometricConstraints()->addCoincident(nearDiagLine->endSketchPoint(), midLine);

        // create the near angle constraint
        Ptr<SketchAngularDimension> nearAngularDim = sketch->sketchDimensions()->addAngularDimension(nearLine, nearDiagLine, nearAngleTextPoint);
        Ptr<ModelParameter> nearAngleParam = nearAngularDim->parameter();
        nearAngleParam->expression("45 deg");

        // move to the farthest side of the gap
        p1->translateBy(halfGapVector);
        p3->translateBy(halfGapVector);

        // save the current position of p3 as the far corner point
        farPoint = p3->copy();

        double constraintLength = nearDiagLine->length() - (toolDiameter / 2);

        // draw the near fillet outline
        Ptr<Point3D> diagEndPoint = nearDiagLine->endSketchPoint()->geometry();
        Ptr<Vector3D> centerVector = diagEndPoint->vectorTo(nearPoint);
        centerVector->normalize();
        centerVector->scaleBy(constraintLength);
        Ptr<SketchPoint> nearFilletCenter = sketch->sketchPoints()->add(diagEndPoint);
        nearFilletCenter->move(centerVector);
        sketch->geometricConstraints()->addCoincident(nearFilletCenter, nearDiagLine);

        // add the near distance constraint
        Ptr<Vector3D> nearLengthConstraintLabelVector = farPoint->vectorTo(diagEndPoint);
        nearLengthConstraintLabelVector->normalize();
        nearLengthConstraintLabelVector->scaleBy(gapWidth / 8);
        Ptr<Point3D> nearLengthConstraintLabelPoint = diagEndPoint->copy();
        nearLengthConstraintLabelPoint->translateBy(nearLengthConstraintLabelVector);

        Ptr<SketchLinearDimension> nearLengthConstraint;
        nearLengthConstraint = sketch->sketchDimensions()->addDistanceDimension(
            nearFilletCenter,
            nearDiagLine->endSketchPoint(),
            DimensionOrientations::AlignedDimensionOrientation,
            nearLengthConstraintLabelPoint
            );

        Ptr<ModelParameter> nearLengthParam = nearLengthConstraint->parameter();
        nearLengthParam->value(constraintLength);

        drawFilletOutline(sketch, nearFilletCenter->geometry(), toolDiameter / 2);

        // create the anchor point for the far 45 degree angle constraint text
        farAngleTextPoint = p3->copy();
        farAngleTextPoint->translateBy(quarterGapPerpVector);
        farAngleTextPoint->translateBy(negTinyGapVector);

        // draw the second straight reference line for the fillet on the
        // farthest edge of the gap
        farLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p1, farPoint);
        farLine->isConstruction(true);
        farLine->isFixed(true);

        // draw the diagnoal line we'll use for the far fillet
        farDiagLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(farPoint, midPoint);
        farDiagLine->isConstruction(true);
        farDiagLine->startSketchPoint()->isFixed(true);
        sketch->geometricConstraints()->addCoincident(farDiagLine->endSketchPoint(), midLine);

        // create the far angle constraint
        Ptr<SketchAngularDimension> farAngularDim = sketch->sketchDimensions()->addAngularDimension(farLine, farDiagLine, farAngleTextPoint);
        Ptr<ModelParameter> farAngleParam = farAngularDim->parameter();
        farAngleParam->expression("45 deg");

        // draw the far fillet outline
        diagEndPoint = farDiagLine->endSketchPoint()->geometry();
        Ptr<Vector3D> centerVector2 = diagEndPoint->vectorTo(farPoint);
        centerVector2->normalize();
        centerVector2->scaleBy(constraintLength);
        Ptr<SketchPoint> farFilletCenter = sketch->sketchPoints()->add(diagEndPoint);
        farFilletCenter->move(centerVector2);
        sketch->geometricConstraints()->addCoincident(farFilletCenter, farDiagLine);

        // add the far distance constraint
        Ptr<Vector3D> farLengthConstraintLabelVector = nearPoint->vectorTo(diagEndPoint);
        farLengthConstraintLabelVector->normalize();
        farLengthConstraintLabelVector->scaleBy(gapWidth / 8);
        Ptr<Point3D> farLengthConstraintLabelPoint = diagEndPoint->copy();
        farLengthConstraintLabelPoint->translateBy(farLengthConstraintLabelVector);

        Ptr<SketchLinearDimension> farLengthConstraint;
        farLengthConstraint = sketch->sketchDimensions()->addDistanceDimension(
            farFilletCenter,
            farDiagLine->endSketchPoint(),
            DimensionOrientations::AlignedDimensionOrientation,
            farLengthConstraintLabelPoint
        );

        Ptr<ModelParameter> farLengthParam = farLengthConstraint->parameter();
        farLengthParam->value(constraintLength);

        drawFilletOutline(sketch, farFilletCenter->geometry(), toolDiameter / 2);
    }
}

// create the profile of all the gaps in the joint
void BoxJoint::createGapSketch()
{
    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    sketch->name("gap_profiles");

    Ptr<Point3D> p1 = sketch->modelToSketchSpace(m_edge->startVertex()->geometry());
    Ptr<Point3D> p2 = sketch->modelToSketchSpace(m_edge->endVertex()->geometry());
    Ptr<Point3D> p3 = sketch->modelToSketchSpace(m_backPoint);
    double distance = p1->distanceTo(p2);

    Ptr<Point3D> sp1 = p1->copy();
    Ptr<Point3D> sp3 = p3->copy();

    unsigned int gapCount = m_toothCount - 1;
    double toothWidth = distance / (double)(m_toothCount + gapCount);

    Ptr<Vector3D> v1 = p1->vectorTo(p2);
    bool isOk = v1->normalize();

    if (!isOk)
        return;

    isOk = v1->scaleBy(toothWidth);

    if (!isOk)
        return;

    Ptr<Profiles> profiles = sketch->profiles();
    m_borderProfile = profiles->item(0);

    for (unsigned int i = 0; i < gapCount; i++)
    {
        sp3->translateBy(v1);
        sp3->translateBy(v1);

        sp1->translateBy(v1);
        if (i > 0)
        {
            sp1->translateBy(v1);
        }

        sketch->sketchCurves()->sketchLines()->addTwoPointRectangle(sp1, sp3);
    }

    sketch->isVisible(false);
    m_profileSketch = sketch;
}

// create the border box
void BoxJoint::createBorderSketch()
{
    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    sketch->name("border_profile");

    Ptr<Point3D> p1 = sketch->modelToSketchSpace(m_edge->startVertex()->geometry());
    Ptr<Point3D> p2 = sketch->modelToSketchSpace(m_edge->endVertex()->geometry());
    Ptr<Point3D> facePoint = sketch->modelToSketchSpace(m_plane->pointOnFace());

    double distance = p1->distanceTo(p2);

    Ptr<SketchLine> se1 = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p1, p2);
    Ptr<SketchPoint> sp1 = se1->startSketchPoint();
    sp1->isFixed(true);

    Ptr<SketchLine> se2 = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p1, facePoint);
    Ptr<PerpendicularConstraint> perpConstraint = sketch->geometricConstraints()->addPerpendicular(se1, se2);

    Ptr<Vector3D> adjustVector = findAdjustmentVector(se2, m_matThickess);
    perpConstraint->deleteMe();
    se2->deleteMe();

    Ptr<Point3D> p3 = p1->copy();
    p3->translateBy(adjustVector);
    Ptr<Point3D> p4 = p2->copy();
    p4->translateBy(adjustVector);

    Ptr<SketchLine> line = sketch->sketchCurves()->sketchLines()->addByTwoPoints(p3, p4);
    line->isConstruction(true);

    sketch->isVisible(false);
    m_backPoint = sketch->sketchToModelSpace(p3);
}
