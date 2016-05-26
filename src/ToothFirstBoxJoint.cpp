#include "ToothFirstBoxJoint.h"

ToothFirstBoxJoint::ToothFirstBoxJoint()
{
}


ToothFirstBoxJoint::~ToothFirstBoxJoint()
{
}

bool ToothFirstBoxJoint::sketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();

    m_filletSketch = sketches->add(m_plane);
    isOk = m_filletSketch->name("fillet_profiles");
    if (!isOk) return false;
    m_filletSketch->isComputeDeferred(true);

    m_gapSketch = sketches->add(m_plane);
    isOk = m_gapSketch->name("gap_profiles");
    if (!isOk) return false;
    m_gapSketch->isComputeDeferred(true);

    createBorderSketch();
    createGapSketch(m_gapSketch);
    createFilletSketch(m_filletSketch);

    return true;
}

// create the sketch for all the dogbone fillets
void ToothFirstBoxJoint::createFilletSketch(const Ptr<Sketch>& sketch)
{
    Ptr<Point3D> p3 = m_edgeStartPoint->copy();
    p3->translateBy(m_thicknessVector);

    // calculate wiggle room vectors
    Ptr<Vector3D> halfWiggleRoomVector = m_edgeVector->copy();
    halfWiggleRoomVector->normalize();
    halfWiggleRoomVector->scaleBy(m_wiggleRoom * 0.5);

    Ptr<Vector3D> negHalfWiggleRoomVector = halfWiggleRoomVector->copy();
    negHalfWiggleRoomVector->scaleBy(-1.0);

    // calculate tooth vectors
    Ptr<Vector3D> toothWidthVector = m_edgeVector->copy();
    toothWidthVector->normalize();
    toothWidthVector->scaleBy(m_toothWidth - m_wiggleRoom);

    // calculate gap vectors
    Ptr<Vector3D> gapWidthVector = m_edgeVector->copy();
    gapWidthVector->normalize();
    gapWidthVector->scaleBy(m_gapWidth + m_wiggleRoom);

    XTRACE(L"tooth width vector: (%lf, %lf, %lf) - %lf\n", toothWidthVector->x(), toothWidthVector->y(), toothWidthVector->z(), toothWidthVector->length());
    XTRACE(L"gap width vector: (%lf, %lf, %lf) - %lf\n", gapWidthVector->x(), gapWidthVector->y(), gapWidthVector->z(), gapWidthVector->length());
    XTRACE(L"tooth count : (%i)\n", m_toothCount);
    XTRACE(L"tooth width : (%lf)\n", m_toothWidth);
    XTRACE(L"gap count : (%i)\n", m_gapCount);
    XTRACE(L"gap width : (%lf)\n", m_gapWidth);

    Ptr<Vector3D> negGapWidthVector = gapWidthVector->copy();
    negGapWidthVector->scaleBy(-1.0);

    Ptr<Point3D> refPoint = p3->copy();

    Ptr<Vector3D> negThicknessVector = m_thicknessVector->copy();
    negThicknessVector->scaleBy(-1.0);

    refPoint->translateBy(toothWidthVector);
    refPoint->translateBy(halfWiggleRoomVector);

    CornerFillet refLeftFillet(refPoint, gapWidthVector, negThicknessVector, m_toolDiameter);
    refLeftFillet.drawSketch(sketch);

    refPoint->translateBy(gapWidthVector);

    CornerFillet refRightFillet(refPoint, negGapWidthVector, negThicknessVector, m_toolDiameter);
    refRightFillet.drawSketch(sketch);

    Ptr<Vector3D> slideVector = toothWidthVector->copy();
    slideVector->add(gapWidthVector);
    m_gapSpacing = slideVector->length();
}

void ToothFirstBoxJoint::createGapSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    Ptr<Vector3D> toothVector = m_edgeVector->copy();
    isOk = toothVector->normalize();
    if (!isOk) return;
    isOk = toothVector->scaleBy(m_toothWidth);
    if (!isOk) return;

    Ptr<Vector3D> halfGapVector = m_edgeVector->copy();
    isOk = halfGapVector->normalize();
    if (!isOk) return;
    isOk = halfGapVector->scaleBy(m_gapWidth / 2);
    if (!isOk) return;

    Ptr<Point3D> refPoint = m_edgeStartPoint->copy();
    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;

    isOk = refPoint->translateBy(toothVector);
    if (!isOk) return;

    m_refGap.setRefPoint(refPoint);
    m_refGap.setWiggleRoom(m_wiggleRoom);
    m_refGap.setHalfWidthVector(halfGapVector);
    m_refGap.setThicknessVector(m_thicknessVector);
    m_refGap.sketch(sketch);

    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;

    m_gapSketch = sketch;
}

bool ToothFirstBoxJoint::extrude()
{
    Ptr<Component> comp = m_plane->body()->parentComponent();

    extrudeProfiles(m_gapSketch);
    extrudeProfiles(m_filletSketch);

    Ptr<ObjectCollection> gapFeatures = Util::filterMatchingFeatures(comp, m_refGap.boundingBox());
    createGapPattern(comp, gapFeatures, m_gapCount);

    return true;
}
