#include "vas_adsr.h"
#include <math.h>

vas_adsr *vas_adsr_new(int tableSize)
{
    vas_adsr *x = (vas_adsr *)malloc(sizeof(vas_adsr));

    x->tableSize = tableSize;
    x->lookupTable_attack = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->lookupTable_decay = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->lookupTable_release = (float *) vas_mem_alloc(x-> tableSize * sizeof(float));
    x->currentIndex = 0;

    //defaulat linear slope
    x->att_q = 1;
    x->dec_q = 1;
    x->rel_q = 1;

    x->att_t = 0.5;
    x->dec_t = 0.5;
    x->sus_v = 0.5;
    x->rel_t = 0.5;

    // silent time 
    x->silent_time = 0.5;
    x->sustain_time = 0.5;

    x->resultvolume = 0.0F;
    x->currentStage = STAGE_SILENT;
    x->currentMode = MODE_LFO;

    x->is_note_on = 0;

    vas_adsr_updateADSR(x); //initTables
    return x;
}

void vas_adsr_free(vas_adsr *x)
{
    vas_mem_free(x->lookupTable_attack);
    vas_mem_free(x->lookupTable_decay);
    vas_mem_free(x->lookupTable_release);
    free(x);
}

void vas_adsr_noteOn(vas_adsr *x, float velocity)
{
    x->is_note_on = 1;
    x->currentStage = STAGE_ATTACK;
    x->resultvolume = x->sus_v * velocity/VELOCITY_MAX;   
}

//methode noteOff
void vas_adsr_noteOff(vas_adsr *x)
{
x->is_note_on = 0;
   switch((int)x->currentMode){
        case MODE_LFO:
            if(x->currentStage != STAGE_SILENT){
                x->currentStage = STAGE_RELEASE;
            }
        break;
        
        case MODE_TRIGGER:
            x->currentStage = STAGE_RELEASE;
        break;
        default: printf("fehler"); break;
    }
}

//methode wechsel zwischen loop modus und sustain mode
void vas_adsr_modeswitch(vas_adsr *x, float mode)
{
     switch((int)mode){

         case MODE_LFO:
            x->currentMode = MODE_LFO;
         break;

         case MODE_TRIGGER:
            x->currentMode = MODE_TRIGGER;
         break;

         default: printf("fehler"); break;
     }
}

//process methode läuft in loop
void vas_adsr_process(vas_adsr *x, float *in, float *out, int vectorSize)
{
    int i = vectorSize;
    float currentValue;
    
    while(i--)
    {
        //currentvalue y-achse zwischen 0 und 1
        currentValue = vas_adsr_get_current_value(x);
        currentValue *= *in++;

        switch((int)x->currentMode) {

	    case MODE_LFO: 

            x->currentIndex += vas_adsr_get_stepSize(x);
            *out++ = currentValue;

             if(x->currentIndex >= x->tableSize){
                 x->currentIndex -= x->tableSize;
                vas_adsr_next_stage(x,x->currentMode);
            }   
            break;

	    case MODE_TRIGGER:

            //currentIndex x-Achse zwischen 0 und tablesize (44100) 
            if(x->currentStage != STAGE_SILENT || x->currentStage != STAGE_SUSTAIN){
            x->currentIndex += vas_adsr_get_stepSize(x);
            } 
            *out++ = currentValue;

            if(x->currentIndex >= x->tableSize){
                x->currentIndex -= x->tableSize;
                //noteOn soll in sustain bleiben bis noteoff Befehl für Release gibt
                if(x->currentStage != STAGE_SUSTAIN) {
                    vas_adsr_next_stage(x,x->currentMode);
                }
            }
            break;

	    default: printf("fehler"); break;
        }
    }
}

float vas_adsr_get_stepSize(vas_adsr *x)
{
    if(x->currentStage == STAGE_ATTACK){
        return (ADSR_MAX - x->att_t)/SCALE_ATTACK;
    }  

    else if(x->currentStage == STAGE_DECAY){
        return (ADSR_MAX - x->dec_t)/SCALE_DECAY;
    }

    else if(x->currentStage == STAGE_SUSTAIN){
        return (ADSR_MAX - x->sustain_time)/SCALE_SUSTAIN;
    }   

    else if(x->currentStage == STAGE_RELEASE){
        return (ADSR_MAX - x->rel_t)/SCALE_RELEASE;
    } 

    else if(x->currentStage == STAGE_SILENT){
        return (ADSR_MAX - x->silent_time)/SCALE_SILENT;
    } 
    else {return 1;}
}

//loop oder sustain einstellen , neuer Input für Modus (neue Variable)
//#define MODE_LFO 0 #define MODE_TRIGGER 1 
void vas_adsr_next_stage(vas_adsr *x, int mode)
{
  switch(mode) {

	    case MODE_LFO: 
            x->currentStage++;
            if(x->currentStage>4){
                if(x->is_note_on==1){
                  x->currentStage=0;  
                }
            }
            break;

	    case MODE_TRIGGER:
            if(x->currentStage != STAGE_SILENT){
                x->currentStage++;
                }
            break;

	    default: printf("fehler"); break;
        }

}

void vas_adsr_updateADSR(vas_adsr *x)
{   
    float x_val;
    for(int i = 0; i < x->tableSize; i++){
        x_val = (float)i / (float)x->tableSize;
        x->lookupTable_attack[i] = vas_adsr_func_slope_up(x_val,x->att_q);
        x->lookupTable_decay[i] = vas_adsr_func_slope_down(x_val,x->dec_q);
        x->lookupTable_release[i] = vas_adsr_func_slope_down(x_val,x->rel_q); 
    } 
}

float vas_adsr_get_current_value(vas_adsr *x)
{
    float sustain = x->resultvolume;
    float decayQuality = x->dec_q;
    float x_normalized = x->currentIndex/(float)x->tableSize;
    int intIndex = floor(x->currentIndex);

    if(x->currentStage == STAGE_ATTACK){
        return x->lookupTable_attack[intIndex];
    }  
    else if(x->currentStage == STAGE_DECAY){
        return x->lookupTable_decay[intIndex] + (sustain * powf(x_normalized,decayQuality));
    }
    else if(x->currentStage == STAGE_SUSTAIN){
        return sustain;
    }   
    else if(x->currentStage == STAGE_RELEASE){
        return x->lookupTable_release[intIndex] * sustain;
    }
     else if(x->currentStage == STAGE_SILENT){
        return 0;
    }    
    else return 0;
        
}

float vas_adsr_func_slope_up(float x,float q)
{   
  float y = 0;
  if(x<0.0F){y=0.0F;}
  else if(x>1.0F){y=1.0F;}
  else{y=powf(x,q);}
  return y;
}

float vas_adsr_func_slope_down(float x,float q)
{   
    float y = 0;
    if(x<0.0F){y=1.0F;}
    else if(x>1.0F){y=0.0F;}
    else{y = 1.0F - powf(x,q);}
    return y;
}

void vas_adsr_setADSR_values(vas_adsr *x, float a, float d, float s, float r)
{
    if(a>0 && a!=x->att_t){x->att_t = a;}
    if(d>0 && d!=x->dec_t){x->dec_t = d;}
    if(s<=1 && s!=x->sus_v){x->sus_v = s;}
    if(r>0 && r!=x->rel_t){x->rel_t = r;}
}

void vas_adsr_set_Silent_time(vas_adsr *x, float st, float sus_t)
{
    if(st>0 && st!=x->silent_time){x->silent_time = st;}
    if(sus_t>0 && sus_t!=x->sustain_time){x->sustain_time = sus_t;}
}


void vas_adsr_setQ(vas_adsr *x, float qa, float qd, float qr){
    if(qa>0){x->att_q = qa;}
    if(qd>0){x->dec_q = qd;}
    if(qr>0){x->rel_q = qr;}
    vas_adsr_updateADSR(x);
}







