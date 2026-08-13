#include "iGameMESHWriter.h"

IGAME_NAMESPACE_BEGIN



bool MESHWriter::GenerateBuffers()
{
	switch (m_DataObject->GetDataObjectType())
	{
	case IG_SURFACE_MESH:
		m_SurfaceMesh = DynamicCast<SurfaceMesh>(m_DataObject);
		if (m_SurfaceMesh) {
			WriteWithSurfaceMeshType();
		}
		break;
	case IG_VOLUME_MESH:
		m_VolumeMesh = DynamicCast<VolumeMesh>(m_DataObject);
		if (m_VolumeMesh) {
			WriteWithVolumeMeshType();
		}
		break;
	case IG_UNSTRUCTURED_MESH:
		m_VolumeMesh = DynamicCast<UnstructuredMesh>(m_DataObject)->ExtractVolumeMesh();
		if (m_VolumeMesh && m_VolumeMesh->GetNumberOfVolumes()) {
			WriteWithVolumeMeshType();
		}
		else {
			m_SurfaceMesh = DynamicCast<UnstructuredMesh>(m_DataObject)->ExtractSurfaceMesh();
			if (m_SurfaceMesh && m_SurfaceMesh->GetNumberOfFaces()) {
				WriteWithSurfaceMeshType();
			}
		}

		break;
	default:
		return false;
	}
	return true;
}
const void MESHWriter::WriteHeaderToBuffer(CharArray::Pointer& buffer)
{
	if(buffer == nullptr) {
		buffer = CharArray::New();
	}
	const std::string header = "MeshVersionFormatted 1\n\nDimension 3\n\n";
	AddStringToBuffer(header, buffer);
}
const void MESHWriter::WriteWithSurfaceMeshType()
{
	m_Buffers.resize(3, nullptr);
	WriteHeaderToBuffer(m_Buffers[0]);
	WritePointsToBuffer(m_SurfaceMesh->GetPoints(), m_Buffers[1]);
	WriteFacesToBuffer(m_SurfaceMesh->GetFaces(), m_Buffers[2]);
}
const void MESHWriter::WriteWithVolumeMeshType()
{
	m_Buffers.resize(3, nullptr);
	WriteHeaderToBuffer(m_Buffers[0]);
	WritePointsToBuffer(m_VolumeMesh->GetPoints(), m_Buffers[1]);
	WriteVolumesToBuffer(m_VolumeMesh->GetVolumes(), m_Buffers[2]);
}
const void MESHWriter::WritePointsToBuffer(Points::Pointer Points, CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int VertexNum = Points ? Points->GetNumberOfPoints() : 0;
	if (!VertexNum) {
		return;
	}
	std::string data = "Vertices\n" + std::to_string(VertexNum) + '\n';
	AddStringToBuffer(data, buffer);
	Point p;
	for (int i = 0; i < VertexNum; i++) {
		p = Points->GetPoint(i);
		for (int j = 0; j < 3; j++) {
			if (j)buffer->AddValue(' ');
			data = std::to_string(p[j]);
			AddStringToBuffer(data, buffer);
		}
		buffer->AddValue(' ');
		buffer->AddValue('-');
		buffer->AddValue('1');
		buffer->AddValue('\n');
	}
}
const void MESHWriter::WriteFacesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int FaceNum = Cells ? Cells->GetNumberOfCells() : 0;
	if (!FaceNum)return;

	std::string data;
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE];

	auto Triangles = IntArray::New();
	Triangles->SetDimension(3);
	for (igIndex i = 0; i < FaceNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 3) {
			Triangles->AddElement(vhs);
		}
	}
	int TriangleNum = Triangles->GetNumberOfElements();
	if (TriangleNum) {
		data = "Triangles\n" + std::to_string(TriangleNum) + '\n';
		AddStringToBuffer(data, buffer);
	}
	auto triangles = Triangles->RawPointer();
	for (int i = 0; i < TriangleNum; i++) {
		for (int j = 0; j < 3; j++) {
			if (j)buffer->AddValue(' ');
			data = std::to_string(1 + (*triangles++));
			AddStringToBuffer(data, buffer);
		}
		buffer->AddValue(' ');
		buffer->AddValue('1');
		buffer->AddValue('\n');
	}

	auto Quads = IntArray::New();
	Quads->SetDimension(4);
	for (igIndex i = 0; i < FaceNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 4) {
			Quads->AddElement(vhs);
		}
	}
	int QuadNum = Quads->GetNumberOfElements();
	if (QuadNum) {
		data = "Quadrilaterals\n" + std::to_string(QuadNum) + '\n';
		AddStringToBuffer(data, buffer);
	}
	auto quads = Quads->RawPointer();
	for (int i = 0; i < QuadNum; i++) {
		for (int j = 0; j < 4; j++) {
			if (j)buffer->AddValue(' ');
			data = std::to_string(1 + (*quads++));
			AddStringToBuffer(data, buffer);
		}
		buffer->AddValue(' ');
		buffer->AddValue('1');
		buffer->AddValue('\n');
	}
}

const void MESHWriter::WriteVolumesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer)
{
	if (buffer == nullptr) {
		buffer = CharArray::New();
	}
	int VolumeNum = Cells ? Cells->GetNumberOfCells() : 0;
	if (!VolumeNum)return;

	std::string data;
	igIndex vcnt = 0;
	igIndex vhs[IGAME_CELL_MAX_SIZE];

	auto Tetras = IntArray::New();
	Tetras->SetDimension(4);
	for (igIndex i = 0; i < VolumeNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 4) {
			Tetras->AddElement(vhs);
		}
	}
	int TetraNum = Tetras->GetNumberOfElements();
	if (TetraNum) {
		data = "Tetrahedra\n" + std::to_string(TetraNum) + '\n';
		AddStringToBuffer(data, buffer);
		auto cells = Tetras->RawPointer();
		for (int i = 0; i < TetraNum; i++) {
			for (int j = 0; j < 4; j++) {
				if (j)buffer->AddValue(' ');
				data = std::to_string(1 + (*cells++));
				AddStringToBuffer(data, buffer);
			}
			buffer->AddValue(' ');
			buffer->AddValue('1');
			buffer->AddValue('\n');
		}
	}


	auto Hexahedrons = IntArray::New();
	Hexahedrons->SetDimension(8);
	for (igIndex i = 0; i < VolumeNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 8) {
			Hexahedrons->AddElement(vhs);
		}
	}
	int HexahedronNum = Hexahedrons->GetNumberOfElements();
	if (HexahedronNum) {
		data = "Hexahedra\n" + std::to_string(HexahedronNum) + '\n';
		AddStringToBuffer(data, buffer);
		auto cells = Hexahedrons->RawPointer();
		for (int i = 0; i < HexahedronNum; i++) {
			for (int j = 0; j < 8; j++) {
				if (j)buffer->AddValue(' ');
				data = std::to_string(1 + (*cells++));
				AddStringToBuffer(data, buffer);
			}
			buffer->AddValue(' ');
			buffer->AddValue('1');
			buffer->AddValue('\n');
		}
	}

	auto Prisms = IntArray::New();
	Prisms->SetDimension(6);
	for (igIndex i = 0; i < VolumeNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 6) {
			Prisms->AddElement(vhs);
		}
	}
	int PrismNum = Prisms->GetNumberOfElements();
	if (PrismNum) {
		data = "Prisms\n" + std::to_string(PrismNum) + '\n';
		AddStringToBuffer(data, buffer);
		auto cells = Prisms->RawPointer();
		for (int i = 0; i < PrismNum; i++) {
			for (int j = 0; j < 6; j++) {
				if (j)buffer->AddValue(' ');
				data = std::to_string(1 + (*cells++));
				AddStringToBuffer(data, buffer);
			}
			buffer->AddValue(' ');
			buffer->AddValue('1');
			buffer->AddValue('\n');
		}
	}

	auto Pyramids = IntArray::New();
	Pyramids->SetDimension(5);
	for (igIndex i = 0; i < VolumeNum; i++) {
		vcnt = Cells->GetCellIds(i, vhs);
		if (vcnt == 5) {
			Pyramids->AddElement(vhs);
		}
	}
	int PyramidNum = Pyramids->GetNumberOfElements();
	if (PyramidNum) {
		data = "Pyramids\n" + std::to_string(PyramidNum) + '\n';
		AddStringToBuffer(data, buffer);
		auto cells = Pyramids->RawPointer();
		for (int i = 0; i < PyramidNum; i++) {
			for (int j = 0; j < 5; j++) {
				if (j)buffer->AddValue(' ');
				data = std::to_string(1 + (*cells++));
				AddStringToBuffer(data, buffer);
			}
			buffer->AddValue(' ');
			buffer->AddValue('1');
			buffer->AddValue('\n');
		}
	}

}

IGAME_NAMESPACE_END