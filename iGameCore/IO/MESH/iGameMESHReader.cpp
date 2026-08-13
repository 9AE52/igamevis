#include "iGameMESHReader.h"

IGAME_NAMESPACE_BEGIN
bool MESHReader::Parsing()
{
	auto& left = this->IS;
	auto right = this->FILEEND;

	CellArray::Pointer Lines = m_Data.GetLines();
	CellArray::Pointer Faces = m_Data.GetFaces();
	CellArray::Pointer Volumes = m_Data.GetVolumes();

	while (left < right)
	{
		while (*left == ' ')left++;
		const char* lineEnd = strchr(left, '\n');
		if (!lineEnd) {
			lineEnd = right;
		}
		if (*left == '\r' || *left == '\n') {
			left = lineEnd + 1;
			continue;
		}
		const char* spacePos = strchr(left, ' ');
		if (!spacePos || spacePos > lineEnd) {
			spacePos = lineEnd;
		}
		std::string  name(left, spacePos);
		// 更新指针位置
		left = lineEnd + 1;
		//std::cout << name << std::endl;
		if (name == "Vertices" || name == "Vertices\r") {
			left = spacePos + 1;
			int VertexNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadPoints(left, VertexNum);
		}
		else if (name == "Edges" || name == "Edges\r") {
			left = spacePos + 1;
			int EdgeNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, EdgeNum,2, Lines);
		}
		else if (name == "Triangles" || name == "Triangles\r") {
			left = spacePos + 1;
			int FaceNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, FaceNum, 3, Faces);
		}
		else if (name == "Quadrilaterals" || name == "Quadrilaterals\r") {
			left = spacePos + 1;
			int FaceNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, FaceNum, 4, Faces);
		}
		else if (name == "Tetrahedra" || name == "Tetrahedra\r") {
			left = spacePos + 1;
			int VolumeNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, VolumeNum, 4, Volumes);
		}
		else if (name == "Hexahedra" || name == "Hexahedra\r") {
			left = spacePos + 1;
			int VolumeNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, VolumeNum, 8, Volumes);
		}
		else if (name == "Prisms" || name == "Prisms\r") {
			left = spacePos + 1;
			int VolumeNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, VolumeNum, 6, Volumes);
		}
		else if (name == "Pyramids" || name == "Pyramids\r") {
			left = spacePos + 1;
			int VolumeNum = mAtoi(left);
			left = strchr(left, '\n') + 1;
			left = ReadCells(left, VolumeNum, 5, Volumes);
		}
		this->UpdateReadProgress();
	}

	return true;
}

const char* MESHReader::ReadPoints(const char* left, int VertexNum)
{
	Points::Pointer Points = m_Data.GetPoints();
	Points->Reserve(VertexNum);
	float p[3] = { 0 };
	const char* lineEnd;
	for (int i = 0; i < VertexNum; i++)
	{
		while (*left == ' ')left++;
		lineEnd = strchr(left, '\n');
		if (*left == '\r' || *left == '\n') {
			i--;
			left = lineEnd + 1;
			continue;
		}
		for (int j = 0; j < 3; j++) {
			left = mAtof(left, p[j]) + 1;
		}
		left = lineEnd + 1;
		Points->AddPoint(p);
	}
	return left;
}
const char* MESHReader::ReadCells(const char* left, int CellNum, int Component, CellArray::Pointer Cells)
{
	const char* lineEnd;
	std::vector<igIndex> cell(Component);
	for (int i = 0; i < CellNum; i++)
	{
		while (*left == ' ')left++;
		lineEnd = strchr(left, '\n');
		if (*left == '\r' || *left == '\n') {
			i--;
			left = lineEnd + 1;
			continue;
		}
		for (int j = 0; j < Component; j++) {
			left = mAtoi(left, cell[j]) + 1;
			cell[j]--;
			//std::cout << cell[j] << ' ';

		}
		//std::cout << '\n';
		Cells->AddCellIds(cell.data(), Component);
		left = lineEnd + 1;
	}
	return left;
}

IGAME_NAMESPACE_END