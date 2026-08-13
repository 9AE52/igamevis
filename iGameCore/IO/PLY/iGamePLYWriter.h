#ifndef iGamePLYWriter_h
#define iGamePLYWriter_h

#include "iGameFileWriter.h"

IGAME_NAMESPACE_BEGIN

class PLYWriter : public FileWriter {
public:
	I_OBJECT(PLYWriter);
	static Pointer New() { return new PLYWriter; }

	bool GenerateBuffers() override;

	//Write the VTK header information to the buffer.
	const void WriteHeaderToBuffer(CharArray::Pointer& buffer);

	// Write the point data to the buffer for output.
	const void WritePointsToBuffer(Points::Pointer, CharArray::Pointer& buffer);

	// Write the face (element connectivity) data to the buffer.
	const void WriteFacesToBuffer(CellArray::Pointer Cells, CharArray::Pointer& buffer);
	
	// Write data in a surface mesh format to the file.
	const void WriteWithSurfaceMeshType();
protected:
	PLYWriter() = default;
	~PLYWriter() override = default;
	SurfaceMesh::Pointer m_SurfaceMesh{ nullptr };
};

IGAME_NAMESPACE_END
#endif