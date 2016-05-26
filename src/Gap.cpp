#include "Gap.h"

Gap::Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& halfWidthVector, const Ptr<Vector3D>& thicknessVector, double wiggleRoom)
{
    setRefPoint(refPoint);
    setWiggleRoom(wiggleRoom);
    setHalfWidthVector(halfWidthVector);
    setThicknessVector(thicknessVector);
}

Gap::Gap()
{
}

Gap::~Gap()
{
}

Ptr<BoundingBox3D> Gap::boundingBox()
{
    return m_boundingBox->copy();;
}

void Gap::expandBoundingBox()
{
    Ptr<Point3D> p1 = m_refPoint->copy();
    p1->translateBy(m_negHalfWidthVector);
    p1->translateBy(m_negWiggleRoomVector);

    Ptr<Point3D> p2 = m_refPoint->copy();
    p2->translateBy(m_halfWidthVector);
    p2->translateBy(m_wiggleRoomVector);

    Ptr<Point3D> p3 = p1->copy();
    p3->translateBy(m_thicknessVector);

    Ptr<Point3D> p4 = p2->copy();
    p4->translateBy(m_thicknessVector);

    m_boundingBox->expand(p1);
    m_boundingBox->expand(p2);
    m_boundingBox->expand(p3);
    m_boundingBox->expand(p4);

    Ptr<Vector3D> crossThickVector = m_thicknessVector->crossProduct(m_halfWidthVector);
    crossThickVector->normalize();
    crossThickVector->scaleBy(m_thicknessVector->length());

    p1->translateBy(crossThickVector);
    p2->translateBy(crossThickVector);
    p3->translateBy(crossThickVector);
    p4->translateBy(crossThickVector);

    m_boundingBox->expand(p1);
    m_boundingBox->expand(p2);
    m_boundingBox->expand(p3);
    m_boundingBox->expand(p4);

    Ptr<Vector3D> maxExpVector = m_boundingBox->minPoint()->vectorTo(m_boundingBox->maxPoint());
    maxExpVector->scaleBy(0.02);

    Ptr<Vector3D> minExpVector = maxExpVector->copy();
    minExpVector->scaleBy(-1.0);

    Ptr<Point3D> minExpPoint = m_boundingBox->minPoint();
    minExpPoint->translateBy(minExpVector);

    Ptr<Point3D> maxExpPoint = m_boundingBox->maxPoint();
    maxExpPoint->translateBy(maxExpVector);

    m_boundingBox->expand(minExpPoint);
    m_boundingBox->expand(maxExpPoint);
}

// create the profile of all the gaps in the joint
void Gap::sketch(const Ptr<Sketch>& sketch)
{
    Ptr<Point3D> p1 = m_refPoint->copy();
    p1->translateBy(m_negHalfWidthVector);

    Ptr<Point3D> p2 = m_refPoint->copy();
    p2->translateBy(m_halfWidthVector);

    Ptr<Point3D> p3 = m_refPoint->copy();
    p3->translateBy(m_thicknessVector);

    Ptr<SketchLineList> baseLines = sketch->sketchCurves()->sketchLines()->addThreePointRectangle(
        sketch->modelToSketchSpace(p1),
        sketch->modelToSketchSpace(p2),
        sketch->modelToSketchSpace(p3)
    );

    for (unsigned int i = 0; i < baseLines->count(); i++)
    {
        Ptr<SketchLine> line = baseLines->item(i);
        line->isConstruction(true);
    }

    p1->translateBy(m_negWiggleRoomVector);
    p2->translateBy(m_wiggleRoomVector);

    expandBoundingBox();
    Ptr<SketchLine> boxLine = sketch->sketchCurves()->sketchLines()->addByTwoPoints(
        sketch->modelToSketchSpace(m_boundingBox->minPoint()),
        sketch->modelToSketchSpace(m_boundingBox->maxPoint())
    );
    boxLine->isConstruction(false);

    sketch->sketchCurves()->sketchLines()->addThreePointRectangle(
        sketch->modelToSketchSpace(p1),
        sketch->modelToSketchSpace(p2),
        sketch->modelToSketchSpace(p3)
    );
}
