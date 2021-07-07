#include "vas_adsr.h"
#include <math.h>

vas_adsr *vas_adsr_new(int tableSize)
{
    vas_adsr *x = (vas_adsr *)malloc(sizeof(vas_adsr));

    x->tableSize = 5 * tableSize;
    x->lookupTable = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->currentIndex = 0;
    x->sus_t = 0.5;

    //defaulat linear slope
    x->att_q = 1;
    x->dec_q = 1;
    x->rel_q = 1;

    vas_adsr_updateADSR(x,0.5,0.5,0.5,0.5);
    
    return x;
}

void vas_adsr_free(vas_adsr *x)
{
    vas_mem_free(x->lookupTable);
    free(x);
}

void vas_adsr_process(vas_adsr *x, float *in, float *out, int vectorSize, int mode)
{
    int i = vectorSize;
    float currentValue;
    
    while(i--)
    {
        
        int intIndex = floor(x->currentIndex);
        currentValue = x->lookupTable[intIndex];
        currentValue *= *in++;
        x->currentIndex++;
        *out++ = currentValue;

        if(x->currentIndex >= x->tableSize)
         x->currentIndex -= x->tableSize;
    }
}

void vas_adsr_updateADSR(vas_adsr *x, float a, float d, float s , float r)
{   
    if (a<=1 && a>0)
        x->att_t = a;

    if (d<=1 && d>0)
        x->dec_t = d;

    if (s<=1 && s>=0)    
        x->sus_v = s;

    if (r<=1 && r>0)
        x->rel_t = 3 * r;
    
    float sum_t = x->att_t + x->dec_t + x->rel_t + x->sus_t;
    int attackSamples = floor((x->tableSize/sum_t) * x->att_t);
    int decaySamples = floor((x->tableSize/sum_t) * x->dec_t);
    int sustainSamples = floor((x->tableSize/sum_t) * x->sus_t);
    int releaseSamples = x->tableSize - attackSamples - decaySamples - sustainSamples;

    float currentY = 0;
    int stage = 0;
    
    for(int i = 0; i < x->tableSize; i++)
    {   
        if(currentY>1) currentY = 1;
        if(currentY<0) currentY = 0;

        x->lookupTable[i] = currentY;
        float stepSize = (float)i/(float)x->tableSize;

        if (stage == 0) //attack
        {
            if (i<attackSamples)
            {  
                stepSize = (float)i/(float)attackSamples;
                currentY = vas_adsr_func_attack_normalized(stepSize,x->att_q);
            } else {
                stage=1;
            }
        }

        else if (stage == 1) // decay
        {
            if (i<(attackSamples+decaySamples))
            {  
                stepSize = (float)i/(float)decaySamples;
                currentY = vas_adsr_func_decay_normalized(stepSize,x->dec_q,x->sus_v);
            } else {
                stage=2;
            }
        } 

       else if (stage == 2) // sustain
        {
            if (i > (x->tableSize - releaseSamples))
            {
               stage=3;
            }
        }

       else // release
        {
            stepSize = (float)i/(float)releaseSamples;
            currentY = vas_adsr_func_release_normalized(stepSize,x->rel_q,x->sus_v);
        }
            
    } 

}

float vas_adsr_func_attack_normalized(float x,float q)
{   
  float y = 0;
  if(x<0){y=0;}
  else if(x>1){y=1;}
  else{y=pow(x,q);}
  return y;
}

float vas_adsr_func_decay_normalized(float x,float q,float s)
{   
    float y = 0;
    if(x<0){y=1;}
    else if(x>1){y=s;}
    else{y = 1.0F - powf(x*powf(1.0F-s,1.0F/q),q);}
    return y;
}

float vas_adsr_func_sustain_normalized(float x, float s)
{   
    float y = 0;
    y = s;
    return y;
}

float vas_adsr_func_release_normalized(float x,float q,float s)
{   
    float y = 0;
    if(x<0){y=1;}
    else if(x>1){y=0;}
    else{y = s - powf(x*powf(s,1.0F/q),q);}
    return y;
}

void vas_adsr_setQ(vas_adsr *x, float qa, float qd, float qr){
    if(qa>0)x->att_q = qa;
    if(qd>0)x->dec_q = qd;
    if(qr>0)x->rel_q = qr;
    vas_adsr_updateADSR(x,x->att_t,x->dec_t,x->sus_v,x->rel_t);
}






