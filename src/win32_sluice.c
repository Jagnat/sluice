#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "thirdparty/ladspa.h"

#ifdef _WIN32
#define WIN32_DLL_EXPORT __declspec(dllexport)
#else
#define WIN32_DLL_EXPORT
#endif

// Cutoff, resonance, input, output
#define NUM_PORTS 4
#define PORT_CUTOFF 0
#define PORT_RESO 1
#define PORT_IN 2
#define PORT_OUT 3

LADSPA_Descriptor* desc;

typedef struct _SluiceData
{
	float *cutoff;
	float *resonance;
	float *input;
	float *output;
} SluiceData;

LADSPA_Handle slcInstantiate(const LADSPA_Descriptor *desc, unsigned long sampleRate)
{
	SluiceData *data = (SluiceData*)calloc(1, sizeof(SluiceData));
	return (void*)data;
}

void slcConnectPort(LADSPA_Handle instance, unsigned long port, float *dataLoc)
{
	SluiceData *data = (SluiceData*)instance;
	switch (port)
	{
		case PORT_CUTOFF:
		data->cutoff = dataLoc;
		break;

		case PORT_RESO:
		data->resonance = dataLoc;
		break;

		case PORT_IN:
		data->input = dataLoc;
		break;

		case PORT_OUT:
		data->output = dataLoc;
		break;
	}
}

void slcRun(LADSPA_Handle instance, unsigned long sampleCount)
{
	SluiceData *data = (SluiceData*)instance;

	float reso = *data->resonance;

	float* input = data->input;
	float* output = data->output;

	for (int i = 0; i < sampleCount; ++i)
	{
		float postMult = input[i] < 0 ? -1 : 1;
		if (fabs(input[i]) > reso)
		{
			output[i] = reso * postMult;
		}
		else
		{
			output[i] = input[i];
		}
	}
}

void slcCleanup(LADSPA_Handle instance)
{
	free(instance);
}

WIN32_DLL_EXPORT
const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
	// Only allow one plugin index
	if (index != 0)
		return NULL;

	desc = (LADSPA_Descriptor*)calloc(1, sizeof(LADSPA_Descriptor));

	// Properties
	desc->UniqueID = 11435;
	desc->Label = "sluice_plugin";
	desc->Name = "Sluice Filter";
	desc->Maker = "Jagannath Natarajan";
	desc->Copyright = "None";
	desc->PortCount = NUM_PORTS;

	// Function pointers - everything not specified has been 0-cleared
	desc->instantiate = slcInstantiate;
	desc->connect_port = slcConnectPort;
	desc->run = slcRun;
	desc->cleanup = slcCleanup;

	// Allocate port descriptors, names, and range hints
	LADSPA_PortDescriptor* portDescriptors =
		(LADSPA_PortDescriptor*)calloc(NUM_PORTS, sizeof(LADSPA_PortDescriptor));

	char** portNames = (char**)calloc(NUM_PORTS, sizeof(char*));

	LADSPA_PortRangeHint* portRangeHints =
		(LADSPA_PortRangeHint*)calloc(NUM_PORTS, sizeof(LADSPA_PortRangeHint));

	desc->PortDescriptors = portDescriptors;
	desc->PortNames = portNames;
	desc->PortRangeHints = portRangeHints;

	// Cutoff port - logarithmic, range of half sample rate - usually 0 - 22050
	portDescriptors[PORT_CUTOFF] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	portNames[PORT_CUTOFF] = "Cutoff";
	portRangeHints[PORT_CUTOFF].HintDescriptor =
		LADSPA_HINT_BOUNDED_BELOW |
		LADSPA_HINT_BOUNDED_ABOVE |
		LADSPA_HINT_SAMPLE_RATE |
		LADSPA_HINT_LOGARITHMIC |
		LADSPA_HINT_DEFAULT_MAXIMUM;
	portRangeHints[PORT_CUTOFF].LowerBound = 0.0;
	portRangeHints[PORT_CUTOFF].UpperBound = 0.5;

	// Resonance port - 0 to 1
	portDescriptors[PORT_RESO] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
	portNames[PORT_RESO] = "Resonance";
	portRangeHints[PORT_RESO].HintDescriptor = 
		LADSPA_HINT_BOUNDED_BELOW |
		LADSPA_HINT_BOUNDED_ABOVE |
		LADSPA_HINT_DEFAULT_0;
	portRangeHints[PORT_RESO].LowerBound = 0.0;
	portRangeHints[PORT_RESO].UpperBound = 1.0;

	// Input signal port
	portDescriptors[PORT_IN] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
	portNames[PORT_IN] = "Input";
	portRangeHints[PORT_IN].HintDescriptor = 0;

	// Output signal port
	portDescriptors[PORT_OUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
	portNames[PORT_OUT] = "Output";
	portRangeHints[PORT_OUT].HintDescriptor = 0;

	return desc;
}
