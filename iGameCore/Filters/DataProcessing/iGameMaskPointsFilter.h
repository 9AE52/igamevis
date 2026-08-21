#pragma once

#include "iGameFilter.h"
#include "iGameUnstructuredMesh.h"

IGAME_NAMESPACE_BEGIN

/**
 * @class MaskPointsFilter
 * @brief Selects a subset of input points and outputs them as vertex cells.
 *
 * The filter supports fixed-stride sampling and random sampling.
 * Point attributes associated with selected points are preserved.
 */
class MaskPointsFilter : public Filter {
public:
    I_OBJECT(MaskPointsFilter);
    static Pointer New() { return new MaskPointsFilter; }

    /**
     * @brief Executes the point masking operation.
     * @return true if execution succeeds; otherwise false.
     */
    bool Execute() override;

    /**
     * @brief Sets the stride used in fixed-stride sampling.
     *
     * For example, OnRatio = 2 selects every second point.
     */
    void SetOnRatio(int ratio);

    /**
     * @brief Returns the current sampling stride.
     */
    int GetOnRatio() const;

    /**
     * @brief Sets the maximum number of output points.
     *
     * A value of 0 means that no additional maximum limit is applied.
     */
    void SetMaximumNumberOfPoints(IGsize maxPoints);

    /**
     * @brief Returns the maximum number of output points.
     */
    IGsize GetMaximumNumberOfPoints() const;

    /**
     * @brief Sets the first input point index considered for sampling.
     */
    void SetOffset(IGsize offset);

    /**
     * @brief Returns the current sampling offset.
     */
    IGsize GetOffset() const;

    /**
     * @brief Enables or disables random point sampling.
     */
    void SetRandomMode(bool enabled);

    /**
     * @brief Returns whether random sampling is enabled.
     */
    bool GetRandomMode() const;

    /**
     * @brief Sets the seed used by the random number generator.
     */
    void SetRandomSeed(unsigned int seed);

    /**
     * @brief Returns the current random seed.
     */
    unsigned int GetRandomSeed() const;

protected:
    MaskPointsFilter();
    ~MaskPointsFilter() override = default;

private:
    int m_OnRatio{2};
    IGsize m_MaximumNumberOfPoints{0};
    IGsize m_Offset{0};
    bool m_RandomMode{false};
    unsigned int m_RandomSeed{1};
};

IGAME_NAMESPACE_END