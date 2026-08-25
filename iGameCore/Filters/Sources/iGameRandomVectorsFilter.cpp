/**
 * @class   iGameRandomVectors
 * @brief   Mimic ParaView's "Random Vectors" filter (vtkBrownianPoints).
 */

#include "iGameRandomVectorsFilter.h"

#include "iGameAttributeSet.h"
#include "iGameDrawObject.h"
#include "iGameFlatArray.h"
#include "iGamePointSet.h"

#include <cmath>
#include <random>

namespace {
// 全局随机数流（非确定性），模拟 vtkMath::Random()
std::mt19937& GlobalRandomGen() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}
} // namespace

IGAME_NAMESPACE_BEGIN

bool RandomVectorsFilter::Execute() {
    auto input = GetInput(0);
    if (input == nullptr) return false;

    auto pointSet = DynamicCast<PointSet>(input);
    if (pointSet == nullptr) return false;

    const IGsize numPoints = pointSet->GetNumberOfPoints();
    if (numPoints <= 0) return false;

    auto vectors = FloatArray::New();
    vectors->SetName("BrownianVectors");
    vectors->SetDimension(3);
    vectors->Reserve(numPoints);

    auto& gen = GlobalRandomGen();
    std::uniform_real_distribution<double> dirDist(-1.0, 1.0);
    std::uniform_real_distribution<double> speedDist(m_MinimumSpeed, m_MaximumSpeed);

    for (IGsize i = 0; i < numPoints; ++i) {
        double x = dirDist(gen);
        double y = dirDist(gen);
        double z = dirDist(gen);
        const double length = std::sqrt(x * x + y * y + z * z);
        if (length > 0.0) {
            x /= length;
            y /= length;
            z /= length;
        }
        const double speed = speedDist(gen);
        vectors->AddElement3(x * speed, y * speed, z * speed);
    }

    auto attrSet = pointSet->GetAttributeSet();
    attrSet->AddVector(IG_POINT, vectors);
    attrSet->ForceReConvertToDrawableData();

    SetOutput(input);
    return true;
}

void RandomVectorsFilter::SetMinimumSpeed(double speed) { m_MinimumSpeed = speed; }

void RandomVectorsFilter::SetMaximumSpeed(double speed) { m_MaximumSpeed = speed; }

double RandomVectorsFilter::GetMinimumSpeed() const { return m_MinimumSpeed; }

double RandomVectorsFilter::GetMaximumSpeed() const { return m_MaximumSpeed; }

RandomVectorsFilter::RandomVectorsFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}

IGAME_NAMESPACE_END
