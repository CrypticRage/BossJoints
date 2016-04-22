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
Ptr<Vector3D> BoxJoint::findScaleVector(const Ptr<SketchLine>& line, double length)
{
    Ptr<Point3D> sp = line->startSketchPoint()->geometry();
    Ptr<Point3D> ep = line->endSketchPoint()->geometry();

    Ptr<Vector3D> v1 = sp->vectorTo(ep);
    v1->normalize();
    v1->scaleBy(length);

    return v1;
}

bool BoxJoint::extrudeGaps()
{
    return extrudeProfiles(m_gapSketch);
}

bool BoxJoint::extrudeFillets()
{
    return extrudeProfiles(m_filletSketch);
}

// extrude the profiles from the given sketch
bool BoxJoint::extrudeProfiles(const Ptr<Sketch>& sketch)
{
    Ptr<ObjectCollection> profileCollection = ProfileUtil::stripBorderProfile(sketch, m_borderProfile);

    if (profileCollection->count() == 0)
        return false;

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

    Ptr<Vector3D> toothWidthVector = p1->vectorTo(p2);
    toothWidthVector->normalize();
    toothWidthVector->scaleBy(toothWidth);

    Ptr<Vector3D> gapWidthVector = p1->vectorTo(p2);
    gapWidthVector->normalize();
    gapWidthVector->scaleBy(gapWidth);

    Ptr<Vector3D> negGapWidthVector = p2->vectorTo(p1);
    negGapWidthVector->normalize();
    negGapWidthVector->scaleBy(gapWidth);

    Ptr<Vector3D> thicknessVector = p3->vectorTo(p1);
    thicknessVector->normalize();
    thicknessVector->scaleBy(m_matThickess);

    Ptr<Point3D> refPoint = p3->copy();

    refPoint->translateBy(toothWidthVector);
    Fillet refLeftFillet(refPoint, gapWidthVector, thicknessVector, toolDiameter);
    refLeftFillet.drawRefSketch(sketch);

    refPoint->translateBy(gapWidthVector);
    Fillet refRightFillet(refPoint, negGapWidthVector, thicknessVector, toolDiameter);
    refRightFillet.drawRefSketch(sketch);

    Ptr<Vector3D> slideVector = toothWidthVector->copy();
    slideVector->add(gapWidthVector);

    for (unsigned int i = 1; i < gapCount; i++)
    {
        refPoint = refLeftFillet.centerPoint();
        refPoint->translateBy(slideVector);

        Fillet leftFillet(refPoint, negGapWidthVector, thicknessVector, toolDiameter);
        leftFillet.drawSketch(sketch, refPoint);

        refPoint = refRightFillet.centerPoint();
        refPoint->translateBy(slideVector);

        Fillet rightFillet(refPoint, negGapWidthVector, thicknessVector, toolDiameter);
        rightFillet.drawSketch(sketch, refPoint);
    }

    m_filletSketch = sketch;
}

// create the profile of all the gaps in the joint
void BoxJoint::createGapSketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    
    isOk = sketch->name("gap_profiles");
    if (!isOk)
        return;

    Ptr<Point3D> startPoint = sketch->modelToSketchSpace(m_edge->startVertex()->geometry());
    Ptr<Point3D> endPoint = sketch->modelToSketchSpace(m_edge->endVertex()->geometry());
    Ptr<Point3D> thicknessPoint = sketch->modelToSketchSpace(m_backPoint);

    Ptr<Vector3D> widthVector = startPoint->vectorTo(endPoint);

    double length = widthVector->length();
    unsigned int gapCount = m_toothCount - 1;
    double toothWidth = length / (double)(m_toothCount + gapCount);

    isOk = widthVector->normalize();
    if (!isOk)
        return;

    isOk = widthVector->scaleBy(toothWidth);
    if (!isOk)
        return;

    Ptr<Point3D> refPoint = startPoint->copy();
    Ptr<Vector3D> thicknessVector = startPoint->vectorTo(thicknessPoint);

    isOk = thicknessVector->normalize();
    if (!isOk)
        return;

    isOk = thicknessVector->scaleBy(m_matThickess);
    if (!isOk)
        return;

    Ptr<Profiles> profiles = sketch->profiles();
    m_borderProfile = profiles->item(0);

    for (unsigned int i = 0; i < gapCount; i++)
    {
        refPoint->translateBy(widthVector);
        if (!isOk)
            return;
        
        Gap gap(refPoint, widthVector, thicknessVector);
        gap.sketch(sketch);
        
        refPoint->translateBy(widthVector);
        if (!isOk)
            return;
    }

    sketch->isVisible(false);
    m_gapSketch = sketch;
}

void BoxJoint::checkSurfacePoint(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    Ptr<Vector3D> faceNormal;
    Ptr<Vector3D> perpVector;

    Ptr<Point3D> startPoint = m_edge->startVertex()->geometry();
    Ptr<Point3D> endPoint = m_edge->endVertex()->geometry();
    Ptr<Point3D> facePoint = sketch->modelToSketchSpace(m_plane->pointOnFace());

    XTRACE(L"start : (%lf, %lf, %lf)\n", startPoint->x(), startPoint->y(), startPoint->z());
    XTRACE(L"end : (%lf, %lf, %lf)\n", endPoint->x(), endPoint->y(), endPoint->z());

    Ptr<Vector3D> vector = startPoint->vectorTo(endPoint);
    isOk = vector->scaleBy(0.5);

    Ptr<SurfaceEvaluator> surfaceEval = m_plane->evaluator();
    isOk = surfaceEval->getNormalAtPoint(facePoint, faceNormal);

    perpVector = faceNormal->crossProduct(vector);
    isOk = perpVector->normalize();
    isOk = perpVector->scaleBy(m_matThickess);

    XTRACE(L"vector : (%lf, %lf, %lf)\n", vector->x(), vector->y(), vector->z());
    XTRACE(L"normal : (%lf, %lf, %lf)\n", faceNormal->x(), faceNormal->y(), faceNormal->z());
    XTRACE(L"cross : (%lf, %lf, %lf)\n", perpVector->x(), perpVector->y(), perpVector->z());

    Ptr<SketchPoint> skFacePoint = sketch->sketchPoints()->add(facePoint);
    isOk = skFacePoint->isFixed(true);

    Ptr<Point3D> midLinePoint = startPoint->copy();
    isOk = midLinePoint->translateBy(vector);

    Ptr<Point3D> midTestPoint = midLinePoint->copy();
    isOk = midTestPoint->translateBy(perpVector);

    Ptr<SketchLine> testLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(
        sketch->modelToSketchSpace(midLinePoint),
        sketch->modelToSketchSpace(midTestPoint)
    );
    isOk = testLine->isConstruction(true);

#if 0
    Ptr<Point2D> midTestSurfacePoint;
    Ptr<Point2D> midTestSurfaceConvert;

    isOk = surfaceEval->getParameterAtPoint(midTestPoint, midTestSurfacePoint);
    isOk = surfaceEval->getPointAtParameter(midTestSurfaceConvert, midTestPoint);

    skFacePoint = sketch->sketchPoints()->add(midTestPoint);
    isOk = skFacePoint->isFixed(true);

    skFacePoint = sketch->sketchPoints()->add(midTestSurfaceConvert);
    isOk = skFacePoint->isFixed(true);

    XTRACE(L"surface param : (%lf, %lf)\n", midTestSurfacePoint->x(), midTestSurfacePoint->y());
#endif
}

#if 0
void BoxJoint::isPointOnSurface(const Ptr<BRepFace>& face, const Ptr<Point3D>& point)
{

}

Ptr<Vector3D> BoxJoint::getSurfaceVector()
{

}
#endif

// create the border box
void BoxJoint::createBorderSketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    isOk = sketch->name("border_profile");
    if (!isOk) return;

    checkSurfacePoint(sketch);

    Ptr<Point3D> startPoint = sketch->modelToSketchSpace(m_edge->startVertex()->geometry());
    Ptr<Point3D> endPoint = sketch->modelToSketchSpace(m_edge->endVertex()->geometry());
    Ptr<Point3D> facePoint = sketch->modelToSketchSpace(m_plane->pointOnFace());

    Ptr<SketchLine> primaryEdge = sketch->sketchCurves()->sketchLines()->addByTwoPoints(startPoint, endPoint);
    Ptr<SketchLine> perpEdge = sketch->sketchCurves()->sketchLines()->addByTwoPoints(startPoint, facePoint);
    Ptr<PerpendicularConstraint> perpConstraint = sketch->geometricConstraints()->addPerpendicular(primaryEdge, perpEdge);

    Ptr<Vector3D> scaleVector = findScaleVector(perpEdge, m_matThickess);
    perpConstraint->deleteMe();
    perpEdge->deleteMe();

    Ptr<Point3D> backStartPoint = startPoint->copy();
    backStartPoint->translateBy(scaleVector);
    Ptr<Point3D> backEndPoint = endPoint->copy();
    backEndPoint->translateBy(scaleVector);

    Ptr<SketchLine> line = sketch->sketchCurves()->sketchLines()->addByTwoPoints(backStartPoint, backEndPoint);
    line->isConstruction(true);

    // sketch->isVisible(false);
    m_backPoint = sketch->sketchToModelSpace(backStartPoint);
}
