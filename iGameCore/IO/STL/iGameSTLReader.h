#ifndef iGameSTLReader_h
#define iGameSTLReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class STLReader : public FileReader {
public:
	I_OBJECT(STLReader);
	static Pointer New() { return new STLReader; }

	bool Parsing() override;

	bool ParseBinaryFile();
	bool ParseAsciiFile();

protected:
	STLReader() = default;
	~STLReader() override = default;
};

IGAME_NAMESPACE_END
#endif