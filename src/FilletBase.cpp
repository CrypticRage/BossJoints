#include "FilletBase.h"

#include <cmath>

#include "Debug.h"
#include "Util.h"

#define PI 3.14159265

using namespace BossJoints;

BossJoints::FilletBase::FilletBase(
    const double diameter, const Ptr<Point3D>& centerPoint
) {
    m_diameter = diameter;
    m_centerPoint = centerPoint->copy();
}

BossJoints::FilletBase::FilletBase()
{
}

BossJoints::FilletBase::~FilletBase()
{
}

bool BossJoints::FilletBase::onSurface(const Ptr<BRepFace>& face, const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    Ptr<Surface> surface = face->geometry();
    if (!surface) return isOk;

    Ptr<Plane> plane;
    if (surface->surfaceType() == PlaneSurfaceType)
    {
        plane = surface;
    }
    if (!plane) return isOk;

    Ptr<SurfaceEvaluator> surfaceEval = face->evaluator();
    Ptr<Point3D> facePoint = face->pointOnFace();

    Ptr<Vector3D> xAxis = plane->uDirection();
    Ptr<Vector3D> yAxis = plane->vDirection();
    Ptr<Vector3D> zAxis;

    isOk = surfaceEval->getNormalAtPoint(facePoint, zAxis);
    if (!isOk) return isOk;

    Ptr<Matrix3D> coordinateMatrix = Matrix3D::create();
    coordinateMatrix->setWithCoordinateSystem(m_centerPoint, xAxis, yAxis, zAxis);

    Ptr<Point3D> origin = Point3D::create();
    bool onSurface = true;
    int div = 16;

    origin->set(0.0, 0.0, 0.0);

    for (int i = 0; i < div; i++)
    {
        Ptr<Point3D> point = Point3D::create();
        Ptr<Point3D> checkPoint = Point3D::create();
        Ptr<Vector3D> vector = Vector3D::create();
        double radian = (((360.0 / div) * i) * PI) / 180.0;

        point->set(std::cos(radian), std::sin(radian), 0.0);
        vector = origin->vectorTo(point);
        isOk = vector->scaleBy(m_diameter / 2.0);
        checkPoint = vector->asPoint();
        // XTRACE(L"unit circle point before (%i) : (%lf, %lf, %lf)\n", i, checkPoint->x(), checkPoint->y(), checkPoint->z());
        checkPoint->transformBy(coordinateMatrix);
        // XTRACE(L"unit circle point after (%i) : (%lf, %lf, %lf)\n", i, checkPoint->x(), checkPoint->y(), checkPoint->z());

        // sketch->sketchPoints()->add(checkPoint);
        isOk = Util::isPointOnSurface(checkPoint, face);
        if (!isOk)
        {
            onSurface = false;
        }
    }

    if (onSurface)
        XTRACE(L"fillet IS on surface\n");
    else
        XTRACE(L"fillet IS NOT on surface\n");

    return onSurface;
}

void BossJoints::FilletBase::drawSketch(const Ptr<Sketch>& sketch)
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
