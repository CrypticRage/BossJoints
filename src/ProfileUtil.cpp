#include "ProfileUtil.h"

void ProfileUtil::printProfile(const Ptr<Profile> &profile)
{
    Ptr<Point3D> maxPoint = profile->boundingBox()->maxPoint();
    Ptr<Point3D> minPoint = profile->boundingBox()->minPoint();
    XTRACE(L"profile : (%p)\n", profile.get());
    XTRACE(L"--------------------\n");
    XTRACE(L"max point : (%lf, %lf, %lf)\n", maxPoint->x(), maxPoint->y(), maxPoint->z());
    XTRACE(L"min point : (%lf, %lf, %lf)\n", minPoint->x(), minPoint->y(), minPoint->z());
}

void ProfileUtil::printProfiles(const Ptr<Sketch>& sketch)
{
    Ptr<Profiles> profiles = sketch->profiles();
    Ptr<Profile> profile;

    for (unsigned int i = 0; i < profiles->count(); i++)
    {
        profile = profiles->item(i);
        printProfile(profile);
    }
}

// compare the bounding boxes of two profiles and see if they are equal
bool ProfileUtil::profileBoxesEqual(const Ptr<Profile> &p1, const Ptr<Profile> &p2)
{
    Ptr<Point3D> p1MinPoint = p1->boundingBox()->minPoint();
    Ptr<Point3D> p1MaxPoint = p1->boundingBox()->maxPoint();

    Ptr<Point3D> p2MinPoint = p2->boundingBox()->minPoint();
    Ptr<Point3D> p2MaxPoint = p2->boundingBox()->maxPoint();

    bool retVal = p1MinPoint->isEqualTo(p2MinPoint) && p1MaxPoint->isEqualTo(p2MaxPoint);
    return retVal;
}

// compare each profile in a sketch and try to match against
// the provided border profile
Ptr<ObjectCollection> ProfileUtil::stripBorderProfile(const Ptr<Sketch>& sketch, const Ptr<Profile> &borderProfile)
{
    Ptr<ObjectCollection> profileCollection = ObjectCollection::create();
    Ptr<Profiles> profiles = sketch->profiles();
    Ptr<Profile> profile;

    for (unsigned int i = 0; i < profiles->count(); i++)
    {
        profile = profiles->item(i);
        printProfile(profile);

        if (!profileBoxesEqual(profile, borderProfile))
        {
            profileCollection->add(profile);
        }
    }

    return profileCollection;
}