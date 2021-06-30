#include "vas_osc_carry.h"

vas_osc_carry *vas_osc_carry_new(int tableSize)
{
    vas_osc_carry *x = (vas_osc_carry *)malloc(sizeof(vas_osc_carry));
    x->tableSize = tableSize;
    x->tableSizeADSR =  tableSize;

    x->indexMod = 0;

    x->lookupTableMod = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->lookupTableResult = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));

    x->adsrCarry.att = 0.5;
    x->adsrCarry.dec = 0.5;
    x->adsrCarry.sus = 0.5;
    x->adsrCarry.rel = 0.5;

    x->lookupTableCarryADSR = (float *) vas_mem_alloc(x-> tableSizeADSR * sizeof(float));

    for(int i = 0;  i < MAXNUMBEROFVOICES; i++)
    {
        x->voice[i].frequency = 440;
        x->voice[i].indexResult = 0;
        x->voice[i].envelopeIndex = 0;
        x->voice[i].velocity = 0.7;
        x->voice[i].occupied = 0;
    }
    x->modAmp = 0;
    x->modFreq = 100;
    x->resultFreq = 100;
    
    float stepSizeMod = (M_PI*2) / (float)tableSize;
    float stepSizeResult = (M_PI*2) / (float)tableSize;

    float currentXMod = 0;
    float currentXResult = 0;
    
    for(int i = 0; i < x->tableSize; i++)
    {
        x->lookupTableMod[i] = sinf(currentXMod);
        x->lookupTableResult[i] = sinf(currentXResult);

        currentXMod += stepSizeMod;
        currentXResult += stepSizeResult;
    }
 
    return x;
}

void vas_osc_carry_noteOn(vas_osc_carry *x, float frequency, float velocity)
{
    for(int i = 0; i < MAXNUMBEROFVOICES; i++)
    {
       
        if(!x->voice[i].occupied)
        {
            x->voice[i].occupied = 1;
            x->voice[i].frequency = frequency;
            x->voice[i].velocity = velocity;
            x->voice[i].envelopeIndex = 0;
            x->voice[i].indexResult = 0;
            
            break;
        }
    } 
}

void vas_osc_carry_free(vas_osc_carry *x)
{
    vas_mem_free(x->lookupTableMod);
    vas_mem_free(x->lookupTableResult);
    vas_mem_free(x->lookupTableCarryADSR);
    free(x);
}

void vas_osc_carry_process(vas_osc_carry *x, float *in, float *out, int vectorSize)
{
    int i = vectorSize;
    float voiceSample = 0;
    
    while(i--)
    {
        *out=0;
        for(int j = 0; j < MAXNUMBEROFVOICES; j++)
        {

            if(x->voice[j].occupied)
            {
                voiceSample = 0;
                int intIndexMod = floor(x->indexMod);
                int intIndexResult = floor(x->voice[j].indexResult);

                voiceSample += x->lookupTableResult[intIndexResult];
                x->resultFreq = (1 + (x->modAmp * x->lookupTableMod[intIndexMod])) * x->voice[j].frequency;
                x->voice[j].indexResult += x->resultFreq;

                x->indexMod += x->modFreq;

                if(x->indexMod >= x->tableSize)
                    x->indexMod -= x->tableSize;
                
                if(x->voice[j].indexResult >= x->tableSize)
                    x->voice[j].indexResult -= x->tableSize;
                

                voiceSample *= x->voice[j].velocity;
                voiceSample *= x->lookupTableCarryADSR[x->voice[j].envelopeIndex++];
                
                if(x->voice[j].envelopeIndex >= x->tableSizeADSR)
                    x->voice[j].occupied = 0;
                
                 *out += voiceSample;
            }
                
               // *out++ =  x->lookupTableResult[intIndexResult];
                //x->currentIndex += x->frequency;
               // x->resultFrequency = (1 + (x->modAmp * x->lookupTableMod[intIndexMod])) * x->frequency;
        }
        out++;  
    }
}

void vas_osc_carry_setModAmp(vas_osc_carry *x, float amplitude)
{
    if(amplitude > 0 && amplitude < 1)
        x->modAmp = amplitude;
}

void vas_osc_carry_setModFreq(vas_osc_carry *x, float freq)
{
            x->modFreq = freq;
}

float vas_osc_carry_calc_stepSize_Att(int tableSize, float attVal, float sumVal)
{
    return 1 / ((tableSize/sumVal) * attVal);
}

float vas_osc_carry_calc_stepSize_Dec(int tableSize, float decVal, float sumVal, float susVal)
{
    return (susVal-1) / ((tableSize/sumVal) * decVal);
}

float vas_osc_carry_calc_stepSize_Rel(int tableSize, float relVal, float sumVal, float susVal)
{
    return (-susVal) / ((tableSize/sumVal) * relVal);
}

void vas_osc_carry_updateADSR(vas_osc_carry *x, float a, float d, float s , float r)
{   
    if (a<=1 && a>0)
    x->adsrCarry.att = a;

    if (d<=1 && d>0)
    x->adsrCarry.dec = d;

    if (s<=1 && s>=0)
    x->adsrCarry.sus = s;

    if (r<=1 && r>0)
    x->adsrCarry.rel = r;
    

    float sumVal = x->adsrCarry.att + x->adsrCarry.dec + x->adsrCarry.rel;

    float stepSizeAttack = vas_osc_carry_calc_stepSize_Att(x->tableSizeADSR, x->adsrCarry.att, sumVal);
    float stepSizeDecay = vas_osc_carry_calc_stepSize_Dec(x->tableSizeADSR, x->adsrCarry.dec, sumVal, x->adsrCarry.sus);
    float stepSizeRelease = vas_osc_carry_calc_stepSize_Rel(x->tableSizeADSR, x->adsrCarry.rel, sumVal, x->adsrCarry.sus);

    float releaseSamples = (x->tableSizeADSR/sumVal) * x->adsrCarry.rel;

    float currentX = 0;
    float currentY = 0;
    int stage = 0;
    
    for(int i = 0; i < x->tableSizeADSR; i++)
    {   
        if(currentY>1) currentY = 1;
        if(currentY<0) currentY = 0;
        x->lookupTableCarryADSR[i] = currentY;

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
            if (currentY >= x->adsrCarry.sus)
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





