#ifndef iGameEmptyCell_h
#define iGameEmptyCell_h

#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class EmptyCell : public Cell {
public:
	I_OBJECT(EmptyCell);
	static Pointer New() {	return new EmptyCell; }

	IGenum GetCellType() const noexcept override { return IG_EMPTY_CELL; }
    int GetCellSize() const noexcept override { return 0; }
	int GetNumberOfEdges() override { return 0; }
	int GetNumberOfFaces() override { return 0; }
	Cell* GetEdge(const int) override { return nullptr; }
    Cell* GetFace(const int) override { return nullptr; }

protected:
	EmptyCell() = default;
	~EmptyCell() override = default;
};
IGAME_NAMESPACE_END
#endif