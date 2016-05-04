#include "BoxJoint.h"

BoxJoint::BoxJoint()
{
}

BoxJoint::~BoxJoint()
{
}

BoxJoint *BoxJoint::create(
    const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge, double matThickness
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
    ptr->setMatThickness(matThickness);
    ptr->setStyle("Start With Tooth");
    ptr->setToothCount(5);

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

    Ptr<Point3D> p3 = m_edgeStartPoint->copy();
    p3->translateBy(m_thicknessVector);

    double toothWidth = m_edgeVector->length() / (double)(m_toothCount + m_gapCount);
    double gapWidth = toothWidth;

    // calculate the wiggle room vector
    Ptr<Vector3D> halfWiggleRoomVector = m_edgeVector->copy();
    halfWiggleRoomVector->normalize();
    halfWiggleRoomVector->scaleBy(m_wiggleRoom * 0.5);

    Ptr<Vector3D> negHalfWiggleRoomVector = halfWiggleRoomVector->copy();
    negHalfWiggleRoomVector->scaleBy(-1.0);

    // calculate the tooth width
    Ptr<Vector3D> toothWidthVector = m_edgeVector->copy();
    toothWidthVector->normalize();
    toothWidthVector->scaleBy(toothWidth - m_wiggleRoom);

    // calculate the gap width
    Ptr<Vector3D> gapWidthVector = m_edgeVector->copy();
    gapWidthVector->normalize();
    gapWidthVector->scaleBy(gapWidth + m_wiggleRoom);

    // XTRACE(L"wiggle vector: (%lf, %lf, %lf) - %lf\n", wiggleRoomVector->x(), wiggleRoomVector->y(), wiggleRoomVector->z(), wiggleRoomVector->length());
    XTRACE(L"tooth width vector: (%lf, %lf, %lf) - %lf\n", toothWidthVector->x(), toothWidthVector->y(), toothWidthVector->z(), toothWidthVector->length());
    XTRACE(L"gap width vector: (%lf, %lf, %lf) - %lf\n", gapWidthVector->x(), gapWidthVector->y(), gapWidthVector->z(), gapWidthVector->length());
    XTRACE(L"tooth count : (%i)\n", m_toothCount);
    XTRACE(L"tooth width : (%lf)\n", toothWidth);
    XTRACE(L"gap count : (%i)\n", m_gapCount);
    XTRACE(L"gap width : (%lf)\n", gapWidth);

    Ptr<Vector3D> negGapWidthVector = gapWidthVector->copy();
    negGapWidthVector->scaleBy(-1.0);

    Ptr<Point3D> refPoint = p3->copy();

    Ptr<Vector3D> negThicknessVector = m_thicknessVector->copy();
    negThicknessVector->scaleBy(-1.0);

    // create tooth first fillets
    if (m_style == Style::StartWithTooth)
    {
        refPoint->translateBy(toothWidthVector);
        refPoint->translateBy(halfWiggleRoomVector);

        CornerFillet refLeftFillet(refPoint, gapWidthVector, negThicknessVector, toolDiameter);
        refLeftFillet.drawSketch(sketch);

        refPoint->translateBy(gapWidthVector);

        CornerFillet refRightFillet(refPoint, negGapWidthVector, negThicknessVector, toolDiameter);
        refRightFillet.drawSketch(sketch);

        Ptr<Vector3D> slideVector = toothWidthVector->copy();
        slideVector->add(gapWidthVector);

        Ptr<Point3D> leftRefPoint = refLeftFillet.centerPoint();
        Ptr<Point3D> rightRefPoint = refRightFillet.centerPoint();
        FilletBase leftFillet(toolDiameter, leftRefPoint);
        FilletBase rightFillet(toolDiameter, rightRefPoint);
        for (unsigned int i = 1; i < m_gapCount; i++)
        {
            leftRefPoint->translateBy(slideVector);
            leftFillet.setCenterPoint(leftRefPoint);
            leftFillet.drawSketch(sketch);

            rightRefPoint->translateBy(slideVector);
            rightFillet.setCenterPoint(rightRefPoint);
            rightFillet.drawSketch(sketch);
        }
    }

    // create gap first fillets
    else if (m_style == Style::StartWithGap)
    {
        refPoint->translateBy(gapWidthVector);
        refPoint->translateBy(negHalfWiggleRoomVector);

        CornerFillet refLeftFillet(refPoint, negGapWidthVector, negThicknessVector, toolDiameter);
        refLeftFillet.drawSketch(sketch);

        refPoint->translateBy(toothWidthVector);

        CornerFillet refRightFillet(refPoint, gapWidthVector, negThicknessVector, toolDiameter);
        refRightFillet.drawSketch(sketch);

        Ptr<Vector3D> slideVector = toothWidthVector->copy();
        slideVector->add(gapWidthVector);

        Ptr<Point3D> leftRefPoint = refLeftFillet.centerPoint();
        Ptr<Point3D> rightRefPoint = refRightFillet.centerPoint();
        FilletBase leftFillet(toolDiameter, leftRefPoint);
        FilletBase rightFillet(toolDiameter, rightRefPoint);
        for (unsigned int i = 1; i < m_toothCount; i++)
        {
            leftRefPoint->translateBy(slideVector);
            leftFillet.setCenterPoint(leftRefPoint);
            leftFillet.drawSketch(sketch);

            rightRefPoint->translateBy(slideVector);
            rightFillet.setCenterPoint(rightRefPoint);
            rightFillet.drawSketch(sketch);
        }
    }

    m_filletSketch = sketch;
}

// create the profile of all the gaps in the joint in one sketch
void BoxJoint::createGapSketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    
    Ptr<Profiles> profiles = sketch->profiles();
    m_borderProfile = profiles->item(0);

    isOk = sketch->name("gap_profiles");
    if (!isOk) return;

    double toothWidth = m_edgeVector->length() / (double)(m_toothCount + m_gapCount);
    double gapWidth = toothWidth;

    Ptr<Vector3D> toothVector = m_edgeVector->copy();
    isOk = toothVector->normalize();
    if (!isOk) return;
    isOk = toothVector->scaleBy(toothWidth);
    if (!isOk) return;

    Ptr<Vector3D> halfGapVector = m_edgeVector->copy();
    isOk = halfGapVector->normalize();
    if (!isOk) return;
    isOk = halfGapVector->scaleBy(gapWidth / 2);
    if (!isOk) return;

    Ptr<Point3D> refPoint = m_edgeStartPoint->copy();
    for (unsigned int i = 0; i < m_gapCount; i++)
    {
        isOk = refPoint->translateBy(halfGapVector);
        if (!isOk) return;

        if (m_style == Style::StartWithTooth)
        {
            isOk = refPoint->translateBy(toothVector);
            if (!isOk) return;
        }

        Gap gap(refPoint, halfGapVector, m_thicknessVector, m_wiggleRoom);
        gap.sketch(sketch);

        if (m_style == Style::StartWithGap)
        {
            isOk = refPoint->translateBy(halfGapVector);
            if (!isOk) return;
            isOk = refPoint->translateBy(halfGapVector);
            if (!isOk) return;
        }

        isOk = refPoint->translateBy(halfGapVector);
        if (!isOk) return;
    }

    m_gapSketch = sketch;
}

// check the test point to see if it's on the face
// https://forums.autodesk.com/t5/api-and-scripts/checking-whether-a-2d-point-lies-within-a-flat-surface-profile/m-p/6304081
bool BoxJoint::isPointOnSurface(const Ptr<Point3D>& testPoint, const Ptr<BRepFace>& plane)
{
    bool isOk = false;
    Ptr<Point3D> planePoint;

    Ptr<SurfaceEvaluator> surfaceEval = plane->evaluator();
    Ptr<Point2D> param;

    isOk = surfaceEval->getParameterAtPoint(testPoint, param);
    if (!isOk) return false;

    isOk = surfaceEval->getPointAtParameter(param, planePoint);
    if (!isOk) return false;

    XTRACE(L"test point : (%lf, %lf, %lf)\n", testPoint->x(), testPoint->y(), testPoint->z());
    XTRACE(L"plane point : (%lf, %lf, %lf)\n", planePoint->x(), planePoint->y(), planePoint->z());

    if (testPoint->isEqualTo(planePoint))
    {
        if (surfaceEval->isParameterOnFace(param))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

// create the border sketch
void BoxJoint::createBorderSketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    isOk = sketch->name("border_profile");
    if (!isOk) return;

    Ptr<Vector3D> faceNormal;
    Ptr<Point3D> facePoint = m_plane->pointOnFace();

    XTRACE(L"edge start : (%lf, %lf, %lf)\n", m_edgeStartPoint->x(), m_edgeStartPoint->y(), m_edgeStartPoint->z());
    XTRACE(L"edge end : (%lf, %lf, %lf)\n", m_edgeEndPoint->x(), m_edgeEndPoint->y(), m_edgeEndPoint->z());

    Ptr<Vector3D> scaleVector = m_edgeVector->copy();
    scaleVector->scaleBy(0.5);

    Ptr<SurfaceEvaluator> surfaceEval = m_plane->evaluator();
    isOk = surfaceEval->getNormalAtPoint(facePoint, faceNormal);

    m_thicknessVector = faceNormal->crossProduct(scaleVector);
    isOk = m_thicknessVector->normalize();
    isOk = m_thicknessVector->scaleBy(m_matThickess);

    XTRACE(L"half edge vector: (%lf, %lf, %lf)\n", scaleVector->x(), scaleVector->y(), scaleVector->z());
    XTRACE(L"face normal vector : (%lf, %lf, %lf)\n", faceNormal->x(), faceNormal->y(), faceNormal->z());
    XTRACE(L"thickness vector : (%lf, %lf, %lf)\n", m_thicknessVector->x(), m_thicknessVector->y(), m_thicknessVector->z());

    Ptr<Point3D> midLinePoint = m_edgeStartPoint->copy();
    isOk = midLinePoint->translateBy(scaleVector);

    Ptr<Point3D> midTestPoint = midLinePoint->copy();
    isOk = midTestPoint->translateBy(m_thicknessVector);

    if (!isPointOnSurface(midTestPoint, m_plane))
    {
        isOk = m_thicknessVector->scaleBy(-1.0);
        XTRACE(L"isPointOnSurface() - point NOT on surface!");

        midTestPoint = midLinePoint->copy();
        isOk = midTestPoint->translateBy(m_thicknessVector);
    }

    Ptr<SketchLine> testLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(
        sketch->modelToSketchSpace(midLinePoint),
        sketch->modelToSketchSpace(midTestPoint)
    );
    isOk = testLine->isConstruction(true);

    Ptr<Point3D> backStartPoint = m_edgeStartPoint->copy();
    backStartPoint->translateBy(m_thicknessVector);
    Ptr<Point3D> backEndPoint = m_edgeEndPoint->copy();
    backEndPoint->translateBy(m_thicknessVector);

    Ptr<SketchLineList> lines = sketch->sketchCurves()->sketchLines()->addThreePointRectangle(
        sketch->modelToSketchSpace(m_edgeStartPoint),
        sketch->modelToSketchSpace(m_edgeEndPoint),
        sketch->modelToSketchSpace(backEndPoint)
    );

    for (unsigned int i = 0; i < lines->count(); i++)
    {
        Ptr<SketchLine> line = lines->item(i);
        line->isConstruction(true);
    }
}
