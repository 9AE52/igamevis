#ifndef iGameCellLinks_h
#define iGameCellLinks_h

#include "iGameObject.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
class CellLinks : public Object {
public:
	I_OBJECT(CellLinks);
	static Pointer New() { return new CellLinks; }

	struct Link
	{
		uint8_t capcity{0};
		uint8_t size{0};
		igIndex* pointer{nullptr};
	};

	void Allocate(const IGsize numLinks, int ext = 0);
	
	// Free all memory and initialize the array
	void Initialize();

    // Reallocate memory, and the old memory is preserved. The array 
	// size will not change. '_Newsize' is the number of links.
	void Reserve(const IGsize _Newsize);

	// Reallocate memory, and the old memory is preserved. The array 
	// size will change. '_Newsize' is the number of links.
	void Resize(const IGsize _Newsize);

	// Set the number of links. The array size will change. 
	// '_Newsize' is the number of links.
	void SetNumberOfLinks(const IGsize _Newsize);

	// Get the number of links. 
	IGsize GetNumberOfLinks() const noexcept;

	// Reset the array size, and the old memory will not change.
    void Reset();

	// Free up extra memory.
    void Squeeze();

	// Get the neighbor sequence of link at linkId.
	Link& GetLink(const IGsize linkId);
    // Get the neighbor sequence's size of link at linkId.
    int GetLinkSize(const IGsize linkId);
    // Get the neighbor sequence's pointer of link at linkId.
    igIndex* GetLinkPointer(const IGsize linkId);
    const igIndex* GetLinkPointer(const IGsize linkId) const;

	// Add new empty sequence. Will allocate memory according to size.
	IGsize AddLink(uint8_t size = 0);

	// Set the new sequence of link at linkId.
	void SetLink(const IGsize linkId, igIndex* ids, int size);

	// Delete link at linkId.
	void DeleteLink(const IGsize linkId);

	// Replace one index of link at linkId.
	void ReplaceReference(const IGsize linkId, const IGsize from,
                          const IGsize to);

    // Remove one index of link at linkId.
	void RemoveReference(const IGsize linkId, const IGsize cellId);

	// Add one index of link at linkId.
	void AddReference(const IGsize linkId, const IGsize cellId);


	void ResizeLink(const IGsize linkId, uint8_t _Newsize);

	// Increase the seuqence's size of link at linkId.
	void IncrementLinkSize(const IGsize linkId);

	void AllocateLinks(const IGsize n);

protected:
	CellLinks();
	~CellLinks() override = default;

	ElementArray<Link>::Pointer m_Buffer{};
};
IGAME_NAMESPACE_END
#endif