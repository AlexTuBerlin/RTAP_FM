#include "vas_osc.h"

vas_osc *vas_osc_new(int tableSize)
{
    vas_osc *x = (vas_osc *)malloc(sizeof(vas_osc));

    x->tableSize = tableSize;
    x->tableSizeADSR = tableSize;

    x->lookupTable = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->lookupTableADSR = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->currentIndex = 0;
    x->adsrIndex = 0;

    x->frequency = 440;
    x->amp = 1;
    
    float stepSize = (M_PI*2) / (float)tableSize;
    float currentX = 0;
    
    for(int i = 0; i < x->tableSize; i++)
    {
        x->lookupTable[i] = sinf(currentX);
        currentX += stepSize;
    }
    vas_osc_updateADSR(x,0.5,0.5,0.5,0.5);
 
    return x;
}

void vas_osc_free(vas_osc *x)
{
    vas_mem_free(x->lookupTable);
    vas_mem_free(x->lookupTableADSR);
    free(x);
}

void vas_osc_process(vas_osc *x, float *in, float *out, int vectorSize, int mode)
{
    int i = vectorSize;
    float currentValue;
    
    while(i--)
    {
        
        int intIndex = floor(x->currentIndex);
        int adsrIndex = floor(x->adsrIndex);
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
        *out++ = currentValue * x->amp *x->lookupTableADSR[adsrIndex];

        if(x->currentIndex >= x->tableSize)
         x->currentIndex -= x->tableSize;

        if(x->adsrIndex >= x->tableSizeADSR)
         x->adsrIndex -= x->tableSizeADSR;
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

void vas_osc_updateADSR(vas_osc *x, float a, float d, float s , float r)
{   
    if (a<=1 && a>0)
    x->adsr.att = a;

    if (d<=1 && d>0)
    x->adsr.dec = d;

    if (s<=1 && s>=0)
    x->adsr.sus = s;

    if (r<=1 && r>0)
    x->adsr.rel = r;
    

    float sumVal = x->adsr.att + x->adsr.dec + x->adsr.rel;

    float stepSizeAttack = vas_osc_calc_stepSize_Att(x->tableSizeADSR, x->adsr.att, sumVal);
    float stepSizeDecay = vas_osc_calc_stepSize_Dec(x->tableSizeADSR, x->adsr.dec, sumVal, x->adsr.sus);
    float stepSizeRelease = vas_osc_calc_stepSize_Rel(x->tableSizeADSR, x->adsr.rel, sumVal, x->adsr.sus);

    float releaseSamples = (x->tableSizeADSR/sumVal) * x->adsr.rel;

    float currentX = 0;
    float currentY = 0;
    int stage = 0;
    
    for(int i = 0; i < x->tableSizeADSR; i++)
    {   
        if(currentY>1) currentY = 1;
        if(currentY<0) currentY = 0;
        x->lookupTableADSR[i] = currentY;

        if (stage == 0) //attack
        {
            if (currentY<1)
            {  
               currentY += stepSizeAttack;
            } else {
               stage ++;
            }
        }

        if (stage == 1) // decay
        {
            if (currentY >= x->adsr.sus)
            {  
               currentY += stepSizeDecay;
            } else {
               stage ++;
            }
        } 

        if (stage == 2) // sustain
        {
            if (currentX > (x->tableSizeADSR - releaseSamples))
            {
               stage ++;
            }
        }

        if (stage == 3) // release
        {
            currentY += stepSizeRelease;
        }
        currentX++;
    }
}

float vas_osc_calc_stepSize_Att(int tableSize, float attVal, float sumVal)
{
    return 1 / ((tableSize/sumVal) * attVal);
}

float vas_osc_calc_stepSize_Dec(int tableSize, float decVal, float sumVal, float susVal)
{
    return (susVal-1) / ((tableSize/sumVal) * decVal);
}

float vas_osc_calc_stepSize_Rel(int tableSize, float relVal, float sumVal, float susVal)
{
    return (-susVal) / ((tableSize/sumVal) * relVal);
}






