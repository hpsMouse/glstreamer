#ifndef _CC0CB4B2_3173_11E2_AAFB_206A8A22A96A
#define _CC0CB4B2_3173_11E2_AAFB_206A8A22A96A

#include "types.h"

glstreamer::Processor* makeProvider();
glstreamer::Processor* makeP();
glstreamer::Processor* makePrinter();

glstreamer::Link* makeInternalSingleLink(glstreamer::OutputSlot const& src, glstreamer::InputSlot const& dst);
glstreamer::Link* makeThreadedLink(glstreamer::OutputSlot const& src, glstreamer::InputSlot const& dst);

#endif
