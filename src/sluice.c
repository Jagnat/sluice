#include "sluice.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "thirdparty/log.h"

// Cutoff, resonance, input, output
#define NUM_PORTS 4
#define PORT_CUTOFF 0
#define PORT_RESO 1
#define PORT_IN 2
#define PORT_OUT 3

LADSPA_Descriptor* desc;

const LADSPA_Descriptor * slcDesc(unsigned long index)
{
	log_init("sluice_log.txt");

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

LADSPA_Handle slcInstantiate(const LADSPA_Descriptor *desc, unsigned long sampleRate)
{
	SluiceData *data = (SluiceData*)calloc(1, sizeof(SluiceData));
	data->sampleRate = sampleRate;
	log_info("sampleRate: %d", sampleRate);
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

	float cutoff = *data->cutoff;
	float resonance = *data->resonance;

	float* input = data->input;
	float* output = data->output;

	unsigned int sampleRate = data->sampleRate;

	// Moog filter from:
	// http://musicdsp.org/showArchiveComment.php?ArchiveID=25 

	float f = 0, p = 0, q = 0; //filter coefficients 
	float b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0; //filter buffers (beware denormals!) 
	float t1 = 0, t2 = 0; //temporary buffers
	// Set coefficients given frequency & resonance [0.0...1.0]
	float frequency = (2 * cutoff) / (float)sampleRate;
	log_info("freq:%f", frequency);
	q = 1.0f - frequency;
	p = frequency + 0.8f * frequency * q;
	f = p + p - 1.0f;
	q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
	for (int i = 0; i < sampleCount; i++)
	{
		// Filter (in [-1.0...+1.0])
		float in = input[i];
		in -= q * b4; //feedback
		t1 = b1;
		b1 = (in + b0) * p - b1 * f;

		t2 = b2;
		b2 = (b1 + t1) * p - b2 * f;

		t1 = b3;
		b3 = (b2 + t2) * p - b3 * f;

		b4 = (b3 + t1) * p - b4 * f;
		b4 = b4 - b4 * b4 * b4 * 0.166667f; //clipping
		b0 = in;

		output[i] = b4;
		// Lowpass  output:  b4;
		// Highpass output:  in - b4;
		// Bandpass output:  3.0f * (b3 - b4);
	}
}

void slcCleanup(LADSPA_Handle instance)
{
	free(instance);
}

