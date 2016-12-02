#include <windows.h>
#include "slc_util.h"

#include "thirdparty/ladspa.h"

extern "C"
{

const LADSPA_Descriptor* ladspa_descriptor(unsigned long Index)
{
	return (LADSPA_Descriptor*)0;
}

} // Extern "C"