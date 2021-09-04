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
        currentValue = (1-(x->amp))*(*in)+x->lookupTable[intIndex]*x->amp;

        switch(mode) {

	    case MODE_MOD_NO_INPUT: 

            x->currentIndex += x->frequency;
            currentValue = x->lookupTable[intIndex]*x->amp;
            break;

	    case MODE_MOD_WITH_INPUT:

            x->currentIndex += (1 + *in++) * x->frequency;
            break;

	    case MODE_CARRIER_NO_INPUT:
            
            x->currentIndex += x->frequency;
            currentValue = x->lookupTable[intIndex]*x->amp;
            break;

        case MODE_CARRIER_WITH_INPUT:
        
            x->currentIndex += (1 + *in++) * x->frequency;
            break;

        case MODE_SUM_WITH_IN:

            currentValue = ((x->amp)*currentValue + *in++)*(1-((x->amp)/2));
            x->currentIndex += x->frequency;
            break;

	    default: printf("fehler"); break;
        }

        x->adsrIndex++;
        *out++ = currentValue;

        if(x->currentIndex >= x->tableSize)
         x->currentIndex -= x->tableSize;

        if(x->adsrIndex >= x->tableSizeADSR)
         x->adsrIndex -= x->tableSizeADSR;
    }
}

void vas_osc_set_frequency_factor(vas_osc *x,float master_frequency, float frequency_factor)
{
    if(frequency_factor > 0){
        x->frequency_factor = frequency_factor;
        x->frequency = (x->frequency_factor)*master_frequency;
    }
}

void vas_osc_set_master_frequency(vas_osc *x,float master_frequency)
{
    if(master_frequency > 0){
        x->frequency = (x->frequency_factor)*master_frequency;
    }
}

void vas_osc_setAmp(vas_osc *x, float amp_factor)
{
    if(amp_factor >= 0 && amp_factor <= 1){
        x->amp = amp_factor;
    }
}