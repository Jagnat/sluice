
#include <ladspa.h>
#include "sluice.h"

const LADSPA_Descriptor* ladspa_descriptor(unsigned long index)
{
	return slcDesc(index);
}

#include "sluice.c"

