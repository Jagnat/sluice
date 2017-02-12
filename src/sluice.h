#pragma once
#ifndef _SLUICE_H_
#define _SLUICE_H_

#include "thirdparty/ladspa.h"

typedef struct _SluiceData
{
	float *cutoff;
	float *resonance;
	float *input;
	float *output;
	unsigned int sampleRate;
} SluiceData;

void slcInit();
const LADSPA_Descriptor* slcDesc(unsigned long index);
LADSPA_Handle slcInstantiate(const LADSPA_Descriptor *desc, unsigned long sampleRate);
void slcActivate(LADSPA_Handle instance);
void slcConnectPort(LADSPA_Handle instance, unsigned long port, float *dataLoc);
void slcRun(LADSPA_Handle instance, unsigned long sampleCount);
void slcCleanup(LADSPA_Handle instance);

#endif // _SLUICE_H_

