#include "iGameCellLinks.h"

IGAME_NAMESPACE_BEGIN

void CellLinks::Allocate(const IGsize numLinks, int ext) {
    this->Initialize();
    this->Resize(numLinks);
}

void CellLinks::Initialize() { m_Buffer->Initialize(); }

void CellLinks::Reserve(const IGsize _Newsize) { m_Buffer->Reserve(_Newsize); }

void CellLinks::Resize(const IGsize _Newsize) { m_Buffer->Resize(_Newsize); }

void CellLinks::SetNumberOfLinks(const IGsize _Newsize) {
    this->Resize(_Newsize);
}

IGsize CellLinks::GetNumberOfLinks() const noexcept {
    return m_Buffer->GetNumberOfElements();
}

void CellLinks::Squeeze() { m_Buffer->Squeeze(); }

void CellLinks::Reset() { m_Buffer->Reset(); }

CellLinks::Link& CellLinks::GetLink(const IGsize linkId) {
    return m_Buffer->ElementAt(linkId);
}
int CellLinks::GetLinkSize(const IGsize linkId) {
    return m_Buffer->ElementAt(linkId).size;
}
igIndex* CellLinks::GetLinkPointer(const IGsize linkId) {
    return m_Buffer->ElementAt(linkId).pointer;
}
const igIndex* CellLinks::GetLinkPointer(const IGsize linkId) const {
    return m_Buffer->ElementAt(linkId).pointer;
}

IGsize CellLinks::AddLink(uint8_t size) {
    m_Buffer->AddElement(Link{0, 0, nullptr});
    if (size > 0) { this->ResizeLink(this->GetNumberOfLinks() - 1, size); }
    return this->GetNumberOfLinks() - 1;
}

void CellLinks::SetLink(const IGsize linkId, igIndex* ids, int size) {
    if (linkId >= GetNumberOfLinks()) { Resize(linkId + 1); }
    Link& link = GetLink(linkId);
    link.size = link.capcity = size;
    link.pointer = new igIndex[link.size];
    memcpy(link.pointer, ids, static_cast<size_t>(size * sizeof(igIndex)));
}

void CellLinks::DeleteLink(const IGsize linkId) {
    Link& link = this->GetLink(linkId);
    link.size = link.capcity = 0;
    if (link.pointer) { delete[] link.pointer; }
    link.pointer = nullptr;
}

void CellLinks::ReplaceReference(const IGsize linkId, const IGsize from,
                                 const IGsize to) {
    Link& link = this->GetLink(linkId);

    for (int i = 0; i < link.size; i++) {
        if (link.pointer[i] == from) { link.pointer[i] = to; }
    }
}

void CellLinks::RemoveReference(const IGsize linkId, const IGsize cellId) {
    Link& link = this->GetLink(linkId);

    for (int i = 0; i < link.size; i++) {
        if (link.pointer[i] == cellId) {
            for (int j = i; j < (link.size - 1); j++) {
                link.pointer[j] = link.pointer[j + 1];
            }
            link.size--;
            break;
        }
    }
}

void CellLinks::AddReference(const IGsize linkId, const IGsize cellId) {
    Link& link = this->GetLink(linkId);
    if (link.size >= link.capcity) {
        this->ResizeLink(linkId, 2 * link.size + 1);
    }
    link.pointer[link.size++] = cellId;
}

void CellLinks::ResizeLink(const IGsize linkId, uint8_t _Newsize) {
    Link& link = this->GetLink(linkId);
    igIndex* cells = nullptr;

    if ((cells = new igIndex[_Newsize]) == nullptr) { return; }

    memcpy(cells, link.pointer,
           static_cast<size_t>(_Newsize < link.size ? _Newsize : link.size) *
                   sizeof(igIndex));

    if (link.pointer) { delete[] link.pointer; }

    link.pointer = cells;
    link.capcity = _Newsize;
    if (link.size > link.capcity) { link.size = link.capcity; }
}

void CellLinks::IncrementLinkSize(const IGsize linkId) {
    this->GetLink(linkId).capcity++;
}

void CellLinks::AllocateLinks(const IGsize n) {
    for (int i = 0; i < n; ++i) {
        Link& link = GetLink(i);
        link.pointer = new igIndex[link.capcity];
    }
}

CellLinks::CellLinks() 
{
    m_Buffer = ElementArray<Link>::New(); 
};

IGAME_NAMESPACE_END