#ifndef iGameSTLWriter_h
#define iGameSTLWriter_h

#include "iGameFileWriter.h"

IGAME_NAMESPACE_BEGIN

class STLWriter : public FileWriter {
public:
	I_OBJECT(STLWriter);
	static Pointer New() { return new STLWriter; }

	bool GenerateBuffers() override;
	
	void SetBinary(bool is_binary) { m_Binary = is_binary; }

	bool WriteBinaryToBuffer(CharArray::Pointer& buffer);
	bool WriteAsciiToBuffer(CharArray::Pointer& buffer);

protected:
	STLWriter() = default;
	~STLWriter() override = default;
	SurfaceMesh::Pointer m_SurfaceMesh{ nullptr };
	bool m_Binary{ true };
};

IGAME_NAMESPACE_END
#endif