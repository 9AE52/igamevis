#ifndef iGameOFFReader_h
#define iGameOFFReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class OFFReader : public FileReader {
public:
	I_OBJECT(OFFReader);
	static Pointer New() { return new OFFReader; }

	bool Parsing() override;

	const char* ReadPoints(const char* left, int num);
	const char* ReadFaces(const char* left, int num);
	const char* ReadLines(const char* left, int num, CellArray::Pointer data);
	const char* ReadNums(const char* left, int&, int&, int&);

protected:
	OFFReader() = default;
	~OFFReader() override = default;
};

IGAME_NAMESPACE_END
#endif