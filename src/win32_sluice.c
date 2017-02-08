#include "thirdparty/ladspa.h"
#include "sluice.h"

// Wrap function since we need to export on windows
__declspec(dllexport)
const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
	return slcDesc(index);
}

#include "sluice.c"

