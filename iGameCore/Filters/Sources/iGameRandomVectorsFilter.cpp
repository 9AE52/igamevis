 /**
 * @class   iGameRandomVectors
 * @brief   Generate a point set whose each point carries a random vector
 *          attribute. The vector values are filled with uniform random numbers.
 */

#include "iGameRandomVectorsFilter.h"

#include <random>

IGAME_NAMESPACE_BEGIN

bool RandomVectorsFilter::Execute() {
    auto output = UnstructuredMesh::New();
    output->SetName("RandomVectors");

    auto vectors = FloatArray::New();
    vectors->SetName("random_vectors");
    vectors->SetDimension(static_cast<int>(m_VectorDimension));
    vectors->Reserve(m_NumberOfPoints);

    std::mt19937 gen(m_Seed);
    std::uniform_real_distribution<float> dist(m_MinValue, m_MaxValue);

    for (int i = 0; i < m_NumberOfPoints; i ++) {
        output->AddPoint(Point(dist(gen), dist(gen), dist(gen)));
        igIndex cell[1] = {static_cast<igIndex>(i)};
        output->AddCell(cell, 1, IG_VERTEX);
        for (unsigned int d = 0; d < m_VectorDimension; ++d) {
            vectors->AddValue(dist(gen));
        }
    }

    output->GetAttributeSet()->AddVector(IG_POINT, vectors);
    SetOutput(output);
    return true;
}

void RandomVectorsFilter::SetNumberOfPoints(unsigned int n) {
    if (n > 0) m_NumberOfPoints = n;
}

void RandomVectorsFilter::SetVectorDimension(unsigned int d) {
    if (d > 0 && d <= 7) m_VectorDimension = d;
}

void RandomVectorsFilter::SetRange(float minValue, float maxValue) {
    if (minValue <= maxValue) {
        m_MinValue = minValue;
        m_MaxValue = maxValue;
    }
}

void RandomVectorsFilter::SetSeed(unsigned int seed) { m_Seed = seed; }

RandomVectorsFilter::RandomVectorsFilter() {
    SetNumberOfInputs(0);
    SetNumberOfOutputs(1);
}

IGAME_NAMESPACE_END
