#include "vas_osc.h"

vas_osc *vas_osc_new(int tableSize, float master_frequency)
{
    vas_osc *x = (vas_osc *)malloc(sizeof(vas_osc));

    x->tableSize = tableSize;
    x->lookupTable = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->currentIndex = 0;

    x->frequency = master_frequency;
    x->amp = 1;
    x->frequency_factor = 1;

    float stepSize = (M_PI*2) / (float)tableSize;
    float currentX = 0;
    
    for(int i = 0; i < x->tableSize; i++)
    {
        x->lookupTable[i] = sinf(currentX);
        currentX += stepSize;
    }
 
    return x;
}

void vas_osc_free(vas_osc *x)
{
    vas_mem_free(x->lookupTable);
    free(x);
}

void vas_osc_process(vas_osc *x, float *in, float *out, int vectorSize, int mode)
{
    int i = vectorSize;
    float currentValue;
    
    while(i--)
    {
        
        int intIndex = floor(x->currentIndex);
        currentValue = x->lookupTable[intIndex];

        switch(mode) {

	    case MODE_MOD_NO_INPUT: 

            x->currentIndex += x->frequency;
            break;

	    case MODE_MOD_WITH_INPUT:

            x->currentIndex += (1 + *in++) * x->frequency;
            break;

	    case MODE_CARRIER_NO_INPUT:

            x->currentIndex += x->frequency;
            break;

        case MODE_CARRIER_WITH_INPUT:

            x->currentIndex += (1 + *in++) * x->frequency;
            break;

        case MODE_SUM_WITH_IN:

            currentValue = (currentValue + *in++) * 0.5;
            x->currentIndex += x->frequency;
            break;

	    default: printf("fehler"); break;
        }

        x->adsrIndex++;
        *out++ = currentValue * x->amp;

        if(x->currentIndex >= x->tableSize)
         x->currentIndex -= x->tableSize;

        if(x->adsrIndex >= x->tableSizeADSR)
         x->adsrIndex -= x->tableSizeADSR;
    }
}

void vas_osc_setFrequency(vas_osc *x,float master_frequency, float frequency_factor)
{
    if(frequency_factor > 0){
        x->frequency_factor = frequency_factor;
        x->frequency = frequency_factor*master_frequency;
    }
}

void vas_osc_setAmp(vas_osc *x,float master_amp, float amp_factor)
{
    if(amp_factor >= 0 && amp_factor <= 1){
        x->amp_factor = amp_factor;
        x->amp = amp_factor*master_amp;
    }
}