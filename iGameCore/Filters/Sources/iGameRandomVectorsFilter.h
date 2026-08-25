/**
 * @class   iGameRandomVectors
 * @brief   Generate a point set whose each point carries a random vector
 *          attribute. The vector values are filled with uniform random numbers.
 */

#pragma once

#include "iGameFilter.h"
#include "iGamePointSet.h"
#include "iGamePoints.h"
#include "iGameUnstructuredMesh.h"

IGAME_NAMESPACE_BEGIN


class RandomVectorsFilter : public Filter {
public:
    I_OBJECT(RandomVectorsFilter)
    static Pointer New() { return new RandomVectorsFilter; }

    bool Execute() override;

    void SetNumberOfPoints(unsigned int n);
    void SetVectorDimension(unsigned int d);
    void SetRange(float minValue, float maxValue);
    void SetSeed(unsigned int seed);

protected:
    RandomVectorsFilter();
    ~RandomVectorsFilter() override = default;

protected:
    unsigned int m_NumberOfPoints{1000};
    unsigned int m_VectorDimension{3};
    float m_MinValue{0.f};
    float m_MaxValue{1.f};
    unsigned int m_Seed{42};
};

IGAME_NAMESPACE_END
