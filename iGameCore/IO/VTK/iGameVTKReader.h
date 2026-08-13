#ifndef iGameVTKReader_h
#define iGameVTKReader_h

#include "iGameVTKAbstractReader.h"

IGAME_NAMESPACE_BEGIN

class VTKReader : public VTKAbstractReader {
public:
	I_OBJECT(VTKReader);
	static Pointer New() { return new VTKReader; }

	bool Parsing() override;
	bool CreateDataObject() override;
	DataObject::Pointer GetOutput() override;
protected:
	VTKReader() = default;
	~VTKReader() override = default;

};

IGAME_NAMESPACE_END
#endif