#pragma once

#include "Joint.h"
#include "CornerFillet.h"
#include "Gap.h"

using namespace BossJoints;

namespace BossJoints
{
    class ToothFirstBoxJoint : public Joint
    {
    private:
        Gap m_refGap;
        Ptr<Sketch> m_gapSketch;
        Ptr<Sketch> m_filletSketch;

        void createGapSketch(const Ptr<Sketch>& sketch);
        void createFilletSketch(const Ptr<Sketch>& sketch);

    public:
        ToothFirstBoxJoint();
        ~ToothFirstBoxJoint();

        virtual bool sketch();
        virtual bool extrude();

        virtual void setGapCount(unsigned int gapCount);
        virtual void setToothCount(unsigned int toothCount);
    };
}

inline void BossJoints::ToothFirstBoxJoint::setGapCount(unsigned int gapCount)
{
    if (gapCount < 1)
        return;

    m_gapCount = gapCount;
    m_toothCount = m_gapCount + 1;

    updateToothWidth();
    updateGapWidth();
}

inline void BossJoints::ToothFirstBoxJoint::setToothCount(unsigned int toothCount)
{
    if (toothCount < 2)
        return;

    m_toothCount = toothCount;
    m_gapCount = m_toothCount - 1;

    updateToothWidth();
    updateGapWidth();
}
