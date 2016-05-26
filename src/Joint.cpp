#include "Joint.h"

#include "ToothFirstBoxJoint.h"
#include "GapFirstBoxJoint.h"

Joint::Joint()
{
}


Joint::~Joint()
{
}

Joint *Joint::create(const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge, const std::string& style)
{
    // check to make sure the selected edge borders the selected face
    bool foundEdge = false;

    for (Ptr<BRepEdge> search_edge : plane->edges())
    {
        if (edge == search_edge)
            foundEdge = true;
    }

    if (!foundEdge)
        return NULL;

    Joint *ptr = NULL;

    if (style == "Start With Tooth")
    {
        ptr = new ToothFirstBoxJoint();
    }
    else if (style == "Start With Gap")
    {
        ptr = new GapFirstBoxJoint();
    }

    if (ptr != NULL)
    {
        ptr->setPlane(plane);
        ptr->setEdge(edge);
    }

    return ptr;
}

void Joint::createBorderSketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    Ptr<Sketch> sketch = sketches->add(m_plane);
    sketch->isComputeDeferred(true);

    Ptr<Profiles> profiles = sketch->profiles();
    m_borderProfile = profiles->item(0);

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

    if (!Util::isPointOnSurface(midTestPoint, m_plane))
    {
        isOk = m_thicknessVector->scaleBy(-1.0);
        XTRACE(L"isPointOnSurface() - point NOT on surface!");

        midTestPoint = midLinePoint->copy();
        isOk = midTestPoint->translateBy(m_thicknessVector);
    }

    m_midLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(
        sketch->modelToSketchSpace(midLinePoint),
        sketch->modelToSketchSpace(midTestPoint)
    );
    isOk = m_midLine->isConstruction(true);

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

bool Joint::extrudeProfiles(const Ptr<Sketch>& sketch)
{
    Ptr<ObjectCollection> profileCollection = Util::stripBorderProfile(sketch, m_borderProfile);

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

bool Joint::createGapPattern(const Ptr<Component>& comp, const Ptr<ObjectCollection>& gapFeatures, Ptr<Vector3D> dirVector, int count)
{
    bool isOk = false;
    Ptr<Features> features = comp->features();

    // create the input for rectangular pattern
    Ptr<RectangularPatternFeatures> rectPatterns = features->rectangularPatternFeatures();
    if (!rectPatterns)
        return false;

    Ptr<RectangularPatternFeatureInput> rectPatternInput = rectPatterns->createInput(
        gapFeatures,
        m_edge,
        ValueInput::createByReal(count),
        ValueInput::createByReal(m_gapSpacing),
        PatternDistanceType::SpacingPatternDistanceType
    );
    if (!rectPatternInput)
        return false;

    // check the direction
    Ptr<Vector3D> patternDirVector = rectPatternInput->directionOne();
    isOk = patternDirVector->normalize();
    if (!isOk) return false;

    isOk = dirVector->normalize();
    if (!isOk) return false;

    if (!dirVector->isEqualTo(patternDirVector))
    {
        Ptr<ValueInput> currentDistance = rectPatternInput->distanceOne();
        Ptr<ValueInput> newDistance = ValueInput::createByReal(-currentDistance->realValue());
        isOk = rectPatternInput->distanceOne(newDistance);
    }

    // create the rectangular pattern
    Ptr<RectangularPatternFeature> rectangularFeature = rectPatterns->add(rectPatternInput);
    if (!rectangularFeature)
        return false;

    return true;
}
