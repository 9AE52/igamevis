#ifndef iGameMESHWriter_h
#define iGameMESHWriter_h

#include "iGameFileWriter.h"

IGAME_NAMESPACE_BEGIN

class MESHWriter : public FileWriter {
public:
	I_OBJECT(MESHWriter);
	static Pointer New() { return new MESHWriter; }

	bool GenerateBuffers() override;

	// Write the head of type 'mesh' to the buffer.
	const void WriteHeaderToBuffer(CharArray::Pointer& buffer);

	// Write the point data to the buffer for output.
	const void WritePointsToBuffer(Points::Pointer, CharArray::Pointer& buffer);

	// Write the face (element connectivity) data to the buffer.
	const void WriteFacesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer);

	// Write the volume (element connectivity) data to the buffer.
	const void WriteVolumesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer);

	// Write data in a structured mesh format to the file.
	const void WriteWithVolumeMeshType();

	// Write data in a surface mesh format to the file.
	const void WriteWithSurfaceMeshType();
protected:
	MESHWriter() = default;
	~MESHWriter() override = default;
	SurfaceMesh::Pointer m_SurfaceMesh{ nullptr };
	VolumeMesh::Pointer m_VolumeMesh{ nullptr };
};

IGAME_NAMESPACE_END
#endif