#include "iGameCommand.h"

IGAME_NAMESPACE_BEGIN

const char* Command::GetStringFromEventId(unsigned long event)
{
    switch (event)
    {
#define add_event(Enum) \
  case Enum:            \
    return #Enum;

        iGameAllEventsMacro()
#undef add_event
    default: break;
    }

    return "NoEvent";
}

unsigned long Command::GetEventIdFromString(const char* event)
{
    if (event)
    {
#define add_event(Enum)            \
if (strcmp(event, #Enum) == 0) {   \
    return Enum;                   \
}
        iGameAllEventsMacro()
#undef add_event
    }

    return Command::NoEvent;
}

IGAME_NAMESPACE_END