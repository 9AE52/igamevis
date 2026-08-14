#include "iGameIsoVolumeFilter.h"
#include "iGameThreadPool.h"

IGAME_NAMESPACE_BEGIN

IsoVolumeFilter::IsoVolumeFilter() {
    this->SetNumberOfInputs(1);
    this->SetNumberOfOutputs(1);
}

IsoVolumeFilter::~IsoVolumeFilter() {}

void IsoVolumeFilter::SetIsoScalarData(ArrayObject::Pointer array, double lower, double upper, int dimension) {
    this->m_SelectedScalar = array;
    if (array) {
        this->m_SelectedScalarName = array->GetName();
    }
    this->m_LowerValue = lower;
    this->m_UpperValue = upper;
    this->m_SelectDimension = static_cast<double>(dimension);
}

bool IsoVolumeFilter::Execute() {
    if (m_Inputs->GetNumberOfElements() == 0) { return false; }
    auto input = m_Inputs->GetElement(0);
    if (!input) { return false; }

    switch (input->GetDataObjectType()) {
        case IG_NONE:
            return true;
        case IG_VOLUME_MESH:
            return this->ExecuteWithVolumeMesh(DynamicCast<VolumeMesh>(input));
        case IG_SURFACE_MESH:
            return this->ExecuteWithSurfaceMesh(DynamicCast<SurfaceMesh>(input));
        case IG_UNSTRUCTURED_MESH:
            return this->ExecuteWithUnstructuredMesh(DynamicCast<UnstructuredMesh>(input));
        case IG_STRUCTURED_MESH:
            return this->ExecuteWithVolumeMesh(DynamicCast<VolumeMesh>(input));
        default:
            return false;
    }
}

bool IsoVolumeFilter::ExecuteWithUnstructuredMesh(UnstructuredMesh::Pointer input) {
    if (!input) return false;
    if (!m_SelectedScalar) return false;

    // 第一步：保留标量值 >= LowerValue 的部分
    auto lowerClipped = UnstructuredMesh::New();
    if (!ClipMeshByScalar(input, m_SelectedScalar, m_LowerValue, true, lowerClipped)) {
        return false;
    }

    // 第二步：在第一步结果上保留标量值 <= UpperValue 的部分
    // 需要从中间网格的 AttributeSet 中找到同名标量数组
    ArrayObject::Pointer scalarArray = m_SelectedScalar;
    auto attrSet = lowerClipped->GetAttributeSet();
    if (attrSet && !m_SelectedScalarName.empty()) {
        auto attr = attrSet->GetAttribute(m_SelectedScalarName);
        if (!attr.IsNone() && attr.pointer) {
            scalarArray = attr.pointer;
        }
    }

    auto output = UnstructuredMesh::New();
    if (!ClipMeshByScalar(lowerClipped, scalarArray, m_UpperValue, false, output)) {
        return false;
    }

    this->SetOutput(0, output);
    return true;
}

bool IsoVolumeFilter::ExecuteWithVolumeMesh(VolumeMesh::Pointer vm) {
    if (!vm) return false;
    if (vm->GetIsPolyhedronType()) {
        return this->ExecuteWithVolumeMeshWithPolyhedronType(vm);
    }
    auto um = UnstructuredMesh::New();
    um->GenerateFromVolumeMesh(vm);
    return this->ExecuteWithUnstructuredMesh(um);
}

bool IsoVolumeFilter::ExecuteWithSurfaceMesh(SurfaceMesh::Pointer sm) {
    if (!sm) return false;
    auto um = UnstructuredMesh::New();
    um->GenerateFromSurfaceMesh(sm);
    return this->ExecuteWithUnstructuredMesh(um);
}

bool IsoVolumeFilter::ExecuteWithVolumeMeshWithPolyhedronType(VolumeMesh::Pointer vm) {
    if (!vm || !vm->GetIsPolyhedronType()) { return false; }
    auto um = UnstructuredMesh::New();
    um->GenerateFromVolumeMesh(vm);
    return this->ExecuteWithUnstructuredMesh(um);
}

bool IsoVolumeFilter::ClipMeshByScalar(UnstructuredMesh::Pointer input, ArrayObject::Pointer scalarArray,
                                       double isoValue, bool keepAbove, UnstructuredMesh::Pointer output) {
    if (!input || !output || !scalarArray) return false;

    AttributeSet::Pointer inData = input->GetAttributeSet();
    AttributeSet::Pointer outData = AttributeSet::New();

    CellArray::Pointer OutConn = CellArray::New();
    UnsignedIntArray::Pointer OutType = UnsignedIntArray::New();
    Points::Pointer OutPoints = Points::New();
    std::vector<CellClip::InterpolateEdge> OriginEdge;
    std::vector<igIndex> OriginCell;

    auto inPoints = input->GetPoints();
    auto inPointNum = input->GetNumberOfPoints();
    auto inCells = input->GetCells();
    auto inTypes = input->GetCellTypes();
    igIndex inCellNum = input->GetNumberOfCells();

    DoubleArray::Pointer PointIsoArray = DoubleArray::New();
    CharArray::Pointer CellVisible = CharArray::New();
    ComputePointValueAndCellVisible(inPoints, inCells, PointIsoArray, CellVisible, scalarArray, isoValue, keepAbove);
    auto PointIsoValue = PointIsoArray->RawPointer();
    auto cellVisible = CellVisible->RawPointer();

    // 复制完全在内的单元
    igIndex vcnt = 0;
    igIndex vhs[IGAME_CELL_MAX_SIZE] = {0};
    for (igIndex cellId = 0; cellId < inCellNum; cellId++) {
        if (cellVisible[cellId] == 1) {
            vcnt = inCells->GetCellIds(cellId, vhs);
            OutConn->AddCellIds(vhs, vcnt);
            OutType->AddValue(inTypes->GetValue(cellId));
            OriginCell.emplace_back(cellId);
        }
    }

    // 复制原始点，并建立 OriginEdge 映射
    OutPoints->Resize(inPointNum);
    std::copy(inPoints->RawPointer(), inPoints->RawPointer() + inPointNum * 3, OutPoints->RawPointer());
    OriginEdge.reserve(inPointNum * 2);
    for (int pointId = 0; pointId < inPointNum; pointId++) {
        OriginEdge.emplace_back(CellClip::InterpolateEdge(pointId));
    }

    // 裁剪与边界相交的单元
    igIndex* vhs2 = nullptr;
    igIndex CellId = 0;
    igIndex i = 0;
    Cell::Pointer cell = nullptr;
    double CellIsoValue[IGAME_CELL_MAX_SIZE] = {0};
    for (CellId = 0; CellId < inCellNum; CellId++) {
        if (cellVisible[CellId]) { continue; }
        cell = input->GetCell(CellId);
        vhs2 = cell->m_PointIds->RawPointer();
        vcnt = cell->GetNumberOfPoints();
        for (i = 0; i < vcnt; i++) { CellIsoValue[i] = PointIsoValue[vhs2[i]]; }
        switch (cell->GetCellType()) {
            case IG_TRIANGLE:
                CellClip::Clip(DynamicCast<Triangle>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr,
                               nullptr, CellId, OriginEdge, OriginCell);
                break;
            case IG_QUAD:
                CellClip::Clip(DynamicCast<Quad>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr, nullptr,
                               CellId, OriginEdge, OriginCell);
                break;
            case IG_POLYGON:
                CellClip::Clip(DynamicCast<Polygon>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr, nullptr,
                               CellId, OriginEdge, OriginCell);
                break;
            case IG_TETRA:
                CellClip::Clip(DynamicCast<Tetra>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr, nullptr,
                               CellId, OriginEdge, OriginCell);
                break;
            case IG_PRISM:
            case IG_PYRAMID:
            case IG_HEXAHEDRON:
                // 无对应专用重载，走通用 Volume 裁剪（内部自动四面体化）
                CellClip::Clip(DynamicCast<Volume>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr,
                               nullptr, CellId, OriginEdge, OriginCell, PointIsoValue);
                break;
            case IG_QUADRATIC_TETRA:
                CellClip::Clip(DynamicCast<QuadraticTetra>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr,
                               nullptr, CellId, OriginEdge, OriginCell);
                break;
            case IG_POLYHEDRON:
                CellClip::Clip(DynamicCast<Polyhedron>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr,
                               nullptr, CellId, OriginEdge, OriginCell);
                break;
            default:
                if (Cell::GetCellDimension(cell->GetCellType()) == 3) {
                    CellClip::Clip(DynamicCast<Volume>(cell), CellIsoValue, OutPoints, OutConn, OutType, nullptr,
                                   nullptr, CellId, OriginEdge, OriginCell, PointIsoValue);
                }
                break;
        }
    }

    this->CopyAttributeSetData(OutPoints->GetNumberOfPoints(), OutConn->GetNumberOfCells(), inData, outData,
                               OriginEdge, OriginCell);

    output->SetCells(OutConn, OutType);
    output->SetPoints(OutPoints);
    output->SetAttributeSet(outData);

    std::vector<igIndex>().swap(OriginCell);
    std::vector<CellClip::InterpolateEdge>().swap(OriginEdge);
    return true;
}

void IsoVolumeFilter::ComputePointValueAndCellVisible(Points::Pointer inPoints, CellArray::Pointer inCells,
                                                      DoubleArray::Pointer PointIsoArray, CharArray::Pointer CellVisible,
                                                      ArrayObject::Pointer scalarArray, double isoValue,
                                                      bool keepAbove) {
    igIndex PointId = 0;
    igIndex inPointNum = inPoints->GetNumberOfPoints();
    PointIsoArray->Resize(inPointNum);
    double* PointIsoValue = PointIsoArray->RawPointer();

    int dim = static_cast<int>(m_SelectDimension);
    for (PointId = 0; PointId < inPointNum; PointId++) {
        double s = scalarArray->GetElementValue(PointId, dim);
        if (keepAbove) {
            // s >= isoValue 等价于 isoValue - s <= 0
            PointIsoValue[PointId] = isoValue - s;
        } else {
            // s <= isoValue 等价于 s - isoValue <= 0
            PointIsoValue[PointId] = s - isoValue;
        }
    }

    igIndex CellId = 0;
    IGsize CellNum = inCells->GetNumberOfCells();
    CellVisible->Resize(CellNum);
    auto cellVisible = CellVisible->RawPointer();
    std::fill(cellVisible, cellVisible + CellNum, 0);

    auto func = [&](igIndex start, igIndex end) -> void {
        igIndex cellId = 0;
        igIndex vhs[IGAME_CELL_MAX_SIZE] = {0};
        igIndex vcnt = 0;
        igIndex allIn = 1, allOut = 1;
        double value = 0;
        igIndex i = 0;
        for (cellId = start; cellId < end; cellId++) {
            vcnt = inCells->GetCellIds(cellId, vhs);
            allIn = 1;
            allOut = 1;
            for (i = 0; i < vcnt; i++) {
                value = PointIsoValue[vhs[i]];
                if (value < 0.0) {
                    allOut = 0;
                } else if (value > 0.0) {
                    allIn = 0;
                } else {
                    allIn = 0;
                    allOut = 0;
                }
            }
            if (allIn) {
                cellVisible[cellId] = 1;
            } else if (allOut) {
                cellVisible[cellId] = 2;
            }
        }
    };
    ThreadPool::parallelFor(0, CellNum, func);
    PointIsoValue = nullptr;
}

void IsoVolumeFilter::CopyAttributeSetData(igIndex outPointNum, igIndex outCellNum, AttributeSet::Pointer inData,
                                           AttributeSet::Pointer outData,
                                           std::vector<CellClip::InterpolateEdge> OriginEdge,
                                           std::vector<igIndex> OriginCell) {
    igIndex i = 0, j = 0, k = 0;
    int dimension = 0;
    if (!inData) return;
    auto inAllAttr = inData->GetAllAttributes();
    if (!inAllAttr) return;
    double values[IGAME_CELL_MAX_SIZE] = {0};
    double values_1[IGAME_CELL_MAX_SIZE] = {0};
    double values_2[IGAME_CELL_MAX_SIZE] = {0};
    for (i = 0; i < inAllAttr->GetNumberOfElements(); i++) {
        auto attr = inAllAttr->GetElement(i);
        auto inArray = attr.pointer;
        if (!inArray) continue;
        auto outArray = FloatArray::New();
        outArray->SetName(inArray->GetName());
        outArray->SetDimension(inArray->GetDimension());
        if (attr.attachmentType == IG_CELL) {
            outArray->Resize(outCellNum);
            for (j = 0; j < outCellNum; j++) {
                inArray->GetElement(OriginCell[j], values);
                outArray->SetElement(j, values);
            }
            outData->AddAttribute(attr.type, attr.attachmentType, outArray, attr.GetDataRange());
        } else if (attr.attachmentType == IG_POINT) {
            outArray->Resize(outPointNum);
            dimension = inArray->GetDimension();
            for (j = 0; j < outPointNum; j++) {
                inArray->GetElement(OriginEdge[j].vh1, values_1);
                if (OriginEdge[j].vh2 == -1) {
                    outArray->SetElement(j, values_1);
                } else {
                    inArray->GetElement(OriginEdge[j].vh2, values_2);
                    for (k = 0; k < dimension; k++) {
                        values[k] = values_1[k] + OriginEdge[j].t * (values_2[k] - values_1[k]);
                    }
                    outArray->SetElement(j, values);
                }
            }
            outData->AddAttribute(attr.type, attr.attachmentType, outArray, attr.GetDataRange());
        }
    }
}

IGAME_NAMESPACE_END
