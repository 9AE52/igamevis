#ifndef iGamePLYReader_h
#define iGamePLYReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class PLYReader : public FileReader {
public:
	I_OBJECT(PLYReader);
	static Pointer New() { return new PLYReader; }

	bool Parsing() override;

	const char* ReadPoints(const char* left, int num);
	const char* ReadFaces(const char* left, int num);
	const char* ReadEdges(const char* left, int num);

protected:
	PLYReader() = default;
	~PLYReader() override = default;
};

IGAME_NAMESPACE_END
#endif