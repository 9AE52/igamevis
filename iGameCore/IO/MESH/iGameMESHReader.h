#ifndef iGameMESHReader_h
#define iGameMESHReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class MESHReader : public FileReader {
public:
	I_OBJECT(MESHReader);
	static Pointer New() { return new MESHReader; }

	bool Parsing() override;

	const char* ReadPoints(const char* left, int num);
	const char* ReadCells(const char* left, int num, int Component, CellArray::Pointer data);

protected:
	MESHReader() = default;
	~MESHReader() override = default;
};

IGAME_NAMESPACE_END
#endif