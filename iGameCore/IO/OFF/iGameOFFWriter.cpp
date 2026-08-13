#include "iGameOFFWriter.h"

IGAME_NAMESPACE_BEGIN

bool OFFWriter::GenerateBuffers()
{
	switch (m_DataObject->GetDataObjectType())
	{
	case IG_SURFACE_MESH:
		m_SurfaceMesh = DynamicCast<SurfaceMesh>(m_DataObject);
		WriteWithSurfaceMeshType();
		break;
	case IG_UNSTRUCTURED_MESH:
		m_SurfaceMesh = DynamicCast<UnstructuredMesh>(m_DataObject)->TransferToSurfaceMesh();
		WriteWithSurfaceMeshType();
		break;
	default:
		return false;
	}
	return true;
}
const void OFFWriter::WriteWithSurfaceMeshType()
{
	m_Buffers.resize(20, nullptr);
	WriteHeaderToBuffer(m_Buffers[0]);
	WritePointsToBuffer(m_SurfaceMesh->GetPoints(), m_Buffers[1]);
	WriteFacesToBuffer(m_SurfaceMesh->GetFaces(), m_Buffers[2]);
}
const void OFFWriter::WriteHeaderToBuffer(CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int VertexNum = m_SurfaceMesh->GetNumberOfPoints();
	int FaceNum = m_SurfaceMesh->GetNumberOfFaces();
	int EdgeNum = 0;
	std::string data = "OFF\n" + std::to_string(VertexNum) + ' '
		+ std::to_string(FaceNum) + ' '
		+ std::to_string(EdgeNum) + '\n';
	AddStringToBuffer(data, buffer);
}
const void OFFWriter::WritePointsToBuffer(Points::Pointer Points, CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int VertexNum = Points ? Points->GetNumberOfPoints() : 0;
	if (!VertexNum) {
		return;
	}
	std::string data;
	Point p;
	for (int i = 0; i < VertexNum; i++) {
		p = Points->GetPoint(i);
		for (int j = 0; j < 3; j++) {
			if (j)buffer->AddValue(' ');
			data = std::to_string(p[j]);
			AddStringToBuffer(data, buffer);
		}
		buffer->AddValue('\n');
	}
}
const void OFFWriter::WriteFacesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int FaceNum = Cells ? Cells->GetNumberOfCells() : 0;
	if (!FaceNum)return;
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE];
	std::string data;
	for (int i = 0; i < FaceNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		data = std::to_string(vcnt);
		AddStringToBuffer(data, buffer);
		for (int j = 0; j < vcnt; j++) {
			buffer->AddValue(' ');
			data = std::to_string(vhs[j]);
			AddStringToBuffer(data, buffer);
		}
		buffer->AddValue('\n');
	}
}


IGAME_NAMESPACE_END