#include "vas_osc.h"

vas_osc *vas_osc_new(int tableSize)
{
    vas_osc *x = (vas_osc *)malloc(sizeof(vas_osc));

    x->tableSize = tableSize;
    x->lookupTable = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->currentIndex = 0;

    x->frequency = 440;
    x->amp = 1;
    
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
    }
}

void vas_osc_setFrequency(vas_osc *x, float frequency)
{
    if(frequency > 0)
        x->frequency = frequency;
}

void vas_osc_setAmp(vas_osc *x, float amp)
{
    if(amp >= 0 && amp <= 1)
        x->amp = amp;
}






