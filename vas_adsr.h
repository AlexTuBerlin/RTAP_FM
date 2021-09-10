/**
 * @file vas_adsr.h
 * @author Alexander Wessel and Gideon Krumbach<br>
 * Object for implementing ADSR and Q-ADR Manipulation <br>
 * <br>
 * @brief Audio Object for manipulating ADSR and Q-ADR for different oscillators <br>
 * <br>
 * vas_adsr allows for manipulating any binded oscillator. <br>
 * <br>
 */
#ifndef vas_adsr_h
#define vas_adsr_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vas_mem.h"
#include "vas_util.h"

#define MODE_LFO 0
#define MODE_TRIGGER 1 

#define STAGE_ATTACK 0 
#define STAGE_DECAY 1
#define STAGE_SUSTAIN 2
#define STAGE_RELEASE 3
#define STAGE_SILENT 4

#define SCALE_ATTACK 10
#define SCALE_DECAY 10
#define SCALE_SUSTAIN 10
#define SCALE_RELEASE 10
#define SCALE_SILENT 10

#define ADSR_MAX 100.0F
#define VELOCITY_MAX 128.0F


#ifdef __cplusplus
extern "C" {
#endif
   
/**
 * @struct vas_adsr
 * @brief A structure for adsr object <br>
 * @var vas_adsr::tableSize The parameter for the tablesize of vas_adsr object<br>
 * @var vas_adsr::*lookupTable_attack The pointer to lookupTable_attack  <br>
 * @var vas_adsr::*lookupTable_decay The pointer to lookupTable_decay<br>
 * @var vas_adsr::*lookupTable_release The pointer to lookupTable_attack  <br>
 * @var vas_adsr::currentIndex The parameter for current Index from tablesize <br>
 * @var vas_adsr::att_t The parameter value for adjusting the attack duration <br>
 * @var vas_adsr::dec_t The parameter value for adjusting the decay duration<br>
 * @var vas_adsr::sus_v The parameter value for adjusting the sustain volume<br>
 * @var vas_adsr::rel_t The parameter value for adjusting the realease duration <br>
 * @var vas_adsr::silent_time The parameter value for adjusting the silent_time between Loops <br>  
 * @var vas_adsr::sustain_time The parameter value for adjusting the sustain_time between Loops <br>
 * @var vas_adsr::att_q The parameter value for adjusting the attack q-factor <br>
 * @var vas_adsr::dec_q The parameter value for adjusting the decay q-factor <br>
 * @var vas_adsr::rel_q The parameter value for adjusting the release q-factor <br>  
 * @var vas_adsr::resultvolume The parameter value for adjusting the volume <br>
 * @var vas_adsr::currentStage The parameter value for adjusting the current Stage <br>
 * @var vas_adsr::currentMode The parameter value for switching between LOOP <br>
 * and TRIGGER mode
 * @var vas_adsr::is_note_on The parameter value for switching between note_on<br>
 * and note_off  
 */
typedef struct vas_adsr
{
    int tableSize;
    float *lookupTable_attack;
    float *lookupTable_decay;
    float *lookupTable_release;
    float currentIndex;

    float att_t;
    float dec_t;
    float sus_v;
    float rel_t;

    float silent_time;
    float sustain_time;

    float att_q;
    float dec_q;
    float rel_q;
    float resultvolume;

    int currentStage;
    int currentMode;
    int is_note_on;

} vas_adsr;

/**
 * @related vas_adsr
 * @brief Creates a new adsr object<br>
 * @param tablesize of adsr object <br>
 * The function sets the adsr parameter of the adsr class <br>
 * @return a pointer to the newly created adsr object <br>
 */
vas_adsr *vas_adsr_new(int tableSize);

/**
 * @related vas_adsr
 * @brief Frees a adsr object<br>
 * @param x My adsr object <br>
 * The function frees the allocated memory<br>
 * of a adsr object.
 */
void vas_adsr_free(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Performs the adsr in realtime. <br>
 * @param x My adsr object <br>
 * @param in The input vector <br>
 * @param out The output vector <br>
 * @param vector_size The size of the i/o vectors <br>
 * The function vas_adsr_process appiels the adsr of an <br>
 * incoming signal and copies the result to the output vector <br>
 */
void vas_adsr_process(vas_adsr *x, float *in, float *out, int vector_size);

/**
 * @related vas_adsr
 * @brief Updates lookuptable parameters. <br>
 * @param x My adsr object <br>
 * Updates lookuptable parameters of adsr object with new q-values. <br>
 */
void vas_adsr_updateADSR(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Sets ADSR Parameters. <br>
 * @param x My adsr object <br>
 * @param a parameter for attack time<br>
 * @param d parameter for decay time<br>
 * @param s parameter for sustian volume<br>
 * @param r parameter for release time<br>
 * Sets ADSR parameters of adsr object. <br>
 */
void vas_adsr_setADSR_values(vas_adsr *x, float a, float d, float s, float r);

/**
 * @related vas_adsr
 * @brief Sets Q-ADR Parameters. <br>
 * @param x My adsr object <br>
 * @param qa parameter for Q-attack<br>
 * @param qd parameter for Q-decay<br>
 * @param qr parameter for Q-release<br>
 * Sets Q-ADR parameters of adsr object. <br>
 */
void vas_adsr_setQ(vas_adsr *x, float qa, float qd, float qr);

/**
 * @related vas_adsr
 * @brief Triggers a note_on in both TRIGGER and LOOP Mode. <br>
 * @param x My adsr object <br>
 * @param velocity sound level in Terms of MIDI  <br>
 * Triggers a note on in both ADSR Modes. <br>
 */
void vas_adsr_noteOn(vas_adsr *x, float velocity);

/**
 * @related vas_adsr
 * @brief Triggers a note_off in both TRIGGER and LOOP Mode. <br>
 * @param x My adsr object <br>
 * Triggers a note off in both ADSR Modes. <br>
 */
void vas_adsr_noteOff(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Switches between TRIGGER and LOOP Mode of ADSR. <br>
 * @param x My adsr object <br>
 * @param mode the parameter that adjusts the ADSR mode<br>
 * Switches between TRIGGER and LOOP Mode of ADSR object. <br>
 */
void vas_adsr_modeswitch(vas_adsr *x, float mode);

/**
 * @related vas_adsr
 * @brief Sets the silent time and sustain time in LOOP Mode. <br>
 * @param x My adsr object <br>
 * @param st the parameter for relative silent time <br>
 * @param sus_time the parameter relative sustain time <br>
 Sets the silent time and sustain time in LOOP Mode.
 */
void vas_adsr_set_Silent_time(vas_adsr *x,float st, float sus_time);

/**
 * @related vas_adsr
 * @brief Calculates stepsize of current Stage. <br>
 * @param x My adsr object <br>
 * @return current stepsize for current Stage <br>
 * Calculates stepsize of current Stage and returns it. <br>
 */
float vas_adsr_get_stepSize(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Triggers the next ADSR Stage. <br>
 * @param x My adsr object <br>
 * @param mode the parameter that adjusts the ADSR mode<br>
 * Triggers the next ADSR Stage depending on ADSR mode.<br>
 */
void vas_adsr_next_stage(vas_adsr *x,int mode);

/**
 * @related vas_adsr
 * @brief Sets current value depending on ADSR stage. <br>
 * @param x My adsr object <br>
 * @return current value for lookuptable <br>
 * Sets current value depending on ADSR stage. <br>
 */
float vas_adsr_get_current_value(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Calculates new slope up. <br>
 * @param x current index value <br>
 * @param q the parameter Q<br>
 * @return new value for lookuptable <br>
Calculates new slope up for Q-attack.
 */
float vas_adsr_func_slope_up(float x,float q);

/**
 * @related vas_adsr
 * @brief Calculates new slope up. <br>
 * @param x current index value <br>
 * @param q the parameter Q<br>
 * @return new value for lookuptable <br>
Calculates new slope down for Q-decay and Q-release.
 */
float vas_adsr_func_slope_down(float x,float q);

#ifdef __cplusplus
}
#endif

#endif /* vas_adsr_h */
