#ifndef iGamePicker_h
#define iGamePicker_h

#include "iGameFilter.h"

IGAME_NAMESPACE_BEGIN
class Picker : public Filter{
public:
	I_OBJECT(Picker);
	static Pointer New() { return new Picker; }


protected:
	Picker() {}
	~Picker() override {}

};
IGAME_NAMESPACE_END
#endif