#ifndef _0F9B6552_429F_11E2_A744_206A8A22A96A
#define _0F9B6552_429F_11E2_A744_206A8A22A96A

#include "../types.h"

using glstreamer::Processor;

void registerTypes();

Processor* makeReader();
Processor* makeCaster();
Processor* makeProcessor();
Processor* makePrinter();

#endif
