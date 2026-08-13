#include "iGameSTLReader.h"
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_map>

/**
 * https://en.wikipedia.org/wiki/STL_(file_format)
 */

IGAME_NAMESPACE_BEGIN
bool STLReader::Parsing()
{
	bool binary = true;
	if (strncmp(this->IS, "solid ", 6) == 0) {
		binary = false;
	}

	return binary ? ParseBinaryFile() : ParseAsciiFile();
}
bool STLReader::ParseBinaryFile()
{
	Points::Pointer Points = m_Data.GetPoints();
	CellArray::Pointer Faces = m_Data.GetFaces();
	std::string mode_name;

	auto& left = this->IS;
	auto right = this->FILEEND;

	unsigned char buffer[256];

	auto CopyToBuffer = [&](int size) -> bool {
		if (left + size < right) {
			memcpy(buffer, left, size);
			left += size;
			return true;
		}
		return false;
		};

	auto ReadUInt16 = [&]() -> uint16_t {
		uint16_t number = *(uint16_t*)left;
		left += 2;
		return number;
		};

	auto ReadUInt32 = [&]() -> uint32_t {
		uint32_t number = *(uint32_t*)left;
		left += 4;
		return number;
		};

	auto ReadFloat = [&]() -> float {
		float number = *(float*)left;
		left += 4;
		return number;
		};

	if (!CopyToBuffer(80)) {
		return false;
	};
	buffer[80] = '\0';
	mode_name = std::string((char*)buffer);

	// read number of triangles
	uint32_t size = ReadUInt32();

	// read triangles
	float normal[3];
	float point[3];
	igIndex vhs[3];
	std::unordered_map<float, std::unordered_map<float,
		std::unordered_map<float, igIndex>>> mp; // map same coordinate
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < 3; ++j) {
			normal[j] = ReadFloat();
		}
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				point[k] = ReadFloat();
			}
			if (mp.count(point[0])
				&& mp[point[0]].count(point[1])
				&& mp[point[0]][point[1]].count(point[2])) {
				vhs[j] = mp[point[0]][point[1]][point[2]];
			}
			else {
				vhs[j] = Points->AddPoint(point);
				mp[point[0]][point[1]][point[2]] = vhs[j];
			}
		}
		Faces->AddCellIds(vhs, 3);

		ReadUInt16(); // skip
	}

	if (left > right) return false;

	left = right;

	return true;
}
bool STLReader::ParseAsciiFile()
{
	Points::Pointer Points = m_Data.GetPoints();
	CellArray::Pointer Faces = m_Data.GetFaces();
	std::string model_name;

	auto& left = this->IS;
	auto right = this->FILEEND;
	const char* begin = nullptr, * end = nullptr;

	auto NextString = [&](const char** begin_, const char** end_) -> bool {
		while (left < right && isspace (*left))left++;
		*begin_ = left;
		while (left < right && !isspace(*left))left++;
		*end_ = left;
		return *begin_ < right;
		};

	auto SkipOneWord = [&]() -> bool {
		return NextString(&begin, &end);
		};

	auto ReadDouble = [&]() -> double {
		NextString(&begin, &end);
		char* endChar;
		return strtod(begin, &endChar);
		};

	auto ReadModelName = [&]() -> std::string {
		NextString(&begin, &end); // skip keyword 'solid'
		while (*left == ' ')left++;
		const char* lineEnd = strchr(left, '\n');
		if (!lineEnd) {
			lineEnd = right;
		}
		std::string model_name(left, lineEnd);
		left = lineEnd + 1;
		return model_name;
		};


	// read model name
	model_name = ReadModelName();

	// read triangles
	double normal[3];
	double point[3];
	igIndex vhs[3];
	std::unordered_map<double, std::unordered_map<double, 
		std::unordered_map<double, igIndex>>> mp; // map same coordinate
	while (left < right) {
		if (!NextString(&begin, &end)) {
			return false;
		}
		if (strncmp("facet", begin, 5) != 0) {
			break;
		}
		SkipOneWord(); // "normal"
		normal[0] = ReadDouble();
		normal[1] = ReadDouble();
		normal[2] = ReadDouble();
		SkipOneWord(); // "outer"
		SkipOneWord(); // "loop"
		
		for (int i = 0; i < 3; ++i) {
			SkipOneWord(); // "vertex"
			point[0] = ReadDouble();
			point[1] = ReadDouble();
			point[2] = ReadDouble();
			if (mp.count(point[0])
				&& mp[point[0]].count(point[1])
				&& mp[point[0]][point[1]].count(point[2])) {
				vhs[i] = mp[point[0]][point[1]][point[2]];
			}
			else {
				vhs[i] = Points->AddPoint(point);
				mp[point[0]][point[1]][point[2]] = vhs[i];
			}
		}
		Faces->AddCellIds(vhs, 3);

		SkipOneWord(); // "endloop"
		SkipOneWord(); // "endfacet"
	}

	if (left > right) return false;

	left = right;

	return true;
}
IGAME_NAMESPACE_END