#ifndef iGameQuadraticVolume_h
#define iGameQuadraticVolume_h

#include "iGameCell.h"
#include "iGameQuadraticLine.h"

IGAME_NAMESPACE_BEGIN
class QuadraticVolume : public Cell {
public:
	I_OBJECT(QuadraticVolume);

	virtual int GetEdgePointIds(const int edgeId, const igIndex*& ptIds) = 0;
	virtual int GetFacePointIds(const int faceId, const igIndex*& ptIds) = 0;
	virtual int GetFaceEdgeIds(const int faceId, const igIndex*& edgeIds) = 0;
	virtual int GetPointToOneRingPoints(const int ptId, const igIndex*& ptIds) = 0;
	virtual int GetPointToNeighborEdges(const int ptId, const igIndex*& edgeIds) = 0;
	virtual int GetPointToNeighborFaces(const int ptId, const igIndex*& faceIds) = 0;
	virtual int GetEdgeToNeighborFaces(const int edgeId, const igIndex*& faceIds) = 0;
	virtual int GetFaceToNeighborFaces(const int faceId, const igIndex*& faceIds) = 0;

		igIndex GetEdgeId(const int id) { return this->EdgeIds->GetId(id); }
	igIndex GetFaceId(const int id) { return this->FaceIds->GetId(id); }
	IdArray::Pointer EdgeIds{};
	IdArray::Pointer FaceIds{};
protected:
	QuadraticVolume()
	{
		this->EdgeIds = IdArray::New();
		this->FaceIds = IdArray::New();
	}
	~QuadraticVolume() override = default;

};
IGAME_NAMESPACE_END
#endif