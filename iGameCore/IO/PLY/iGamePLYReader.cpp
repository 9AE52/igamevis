#include "iGamePLYReader.h"

IGAME_NAMESPACE_BEGIN
bool PLYReader::Parsing()
{
	int VertexNum = 0;
	int FaceNum = 0;
	int EdgeNum = 0;

	auto& left = this->IS;
	auto right = this->FILEEND;

	while (left < right) {
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
		left = lineEnd + 1;
		//std::cout << name << std::endl;
		if (name == "element") {
			const char* nextSpacePos = strchr(spacePos + 1, ' ');
			name = std::string(spacePos + 1, nextSpacePos);
			if (name == "vertex") {
				VertexNum = mAtoi(nextSpacePos + 1);
			}
			else if (name == "face") {
				FaceNum = mAtoi(nextSpacePos + 1);
			}
			else if (name == "edge") {
				EdgeNum = mAtoi(nextSpacePos + 1);
			}
		}
		else if (name == "end_header" || name == "end_header\r") {
			left = ReadPoints(left, VertexNum);
			this->UpdateReadProgress();
			left = ReadFaces(left, FaceNum);
			this->UpdateReadProgress();
			left = ReadEdges(left, EdgeNum);
			this->UpdateReadProgress();
			break;
		}
	}

	return true;
}

const char* PLYReader::ReadPoints(const char* left, int VertexNum)
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
			left = mAtof(left, p[j]) ;
		}
		lineEnd = strchr(left, '\n');
		left = lineEnd + 1;
		Points->AddPoint(p);
	}
	return left;
}
const char* PLYReader::ReadFaces(const char* left, int FaceNum)
{
	CellArray::Pointer Faces = m_Data.GetFaces();
	const char* lineEnd;
	int vhs[16] = { 0 };
	for (int i = 0; i < FaceNum; i++)
	{
		while (*left == ' ')left++;
		lineEnd = strchr(left, '\n');
		if (*left == '\r' || *left == '\n') {
			i--;
			left = lineEnd + 1;
			continue;
		}
		int Component;
		left = mAtoi(left, Component) + 1;
		for (int j = 0; j < Component; j++) {
			left = mAtoi(left, vhs[j]) + 1;
		}
		left = lineEnd + 1;
		Faces->AddCellIds(vhs, Component);
	}
	return left;
}
const char* PLYReader::ReadEdges(const char* left, int num)
{
	return left;
}

IGAME_NAMESPACE_END