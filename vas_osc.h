/**
 * @file vas_osc.h
 * @author Based on C.Jaedicke, A.Monciero, P.Schuhladen, F.Müller <br>
 * Edit for rtap_fmMultiOsc~ by Alexander Wessel and Gideon Krumbach<br>
 * <br>
 * @brief Audio Object for adding an oscillator.<br>
 * <br>
 * vas_osc allows to add an oscillator to rtap_fmMultiOsc.
 * <br>
 */

#ifndef vas_osc_h
#define vas_osc_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vas_mem.h"
#include "vas_util.h"

#define MODE_MOD_WITH_INPUT 0 
#define MODE_CARRIER_NO_INPUT 1
#define MODE_SUM_WITH_IN 2

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @struct vas_osc
 * @brief A structure for vas_osc object. <br>
 */
typedef struct vas_osc
{
    int tableSize;          /**< tablesize of vas_osc object*/
    float currentIndex;     /**< current Index from tablesize*/
    float frequency;        /**< frequency of osc*/
    float amp;              /**< amplitude of osc*/
    float *lookupTable;     /**< the pointer to the lookupTable*/
    float frequency_factor; /**< frequency factor of osc*/

} vas_osc;

/**
 * @related vas_osc
 * @brief Creates a new osc object<br>
 * The function sets the osc parameter of the osc class <br>
 * @param tableSize tablesize of osc object <br>
 * @param master_frequency master frequency of rtap_fmMultiOsc object<br>
 * @return a pointer to the newly created osc object <br>
 */
vas_osc *vas_osc_new(int tableSize, float master_frequency);

/**
 * @related vas_osc
 * @brief Frees a osc object<br>
 * @param x My osc object <br>
 * The function frees the allocated memory<br>
 * of an osc object.
 */
void vas_osc_free(vas_osc *x);

/**
 * @related vas_osc
 * @brief Performs the osc in realtime. <br>
 * @param x My osc object <br>
 * @param in The input vector <br>
 * @param out The output vector <br>
 * @param vector_size The size of the i/o vectors <br>
 * The function vas_osc_process processes a oscillator depending on OSC Mode. <br>
 */
void vas_osc_process(vas_osc *x, float *in, float *out, int vector_size, int mode);

/**
 * @related vas_osc
 * @brief Sets frequency factor of oscillator. <br>
 * @param x My osc object <br>
 * @param frequency_factor frequency factor of osc<br>
 * @param master_frequency master_frequency of rtap_fmMultiOsc object<br>
 * Sets frequency factor of oscillator. <br>
 */
void vas_osc_set_frequency_factor(vas_osc *x,float master_frequency, float frequency_factor);

/**
 * @related vas_osc
 * @brief Sets frequency of osc depending on master frequency. <br>
 * @param x My osc object <br>
 * @param master_frequency master frequency of rtap_fmMultiOsc object<br>
 * Sets frequency of osc depending on master frequency. <br>
 */
void vas_osc_set_master_frequency(vas_osc *x, float frequency_factor);

/**
 * @related vas_osc
 * @brief Sets amp of oscillator. <br>
 * @param x My osc object <br>
 * @param amp_factor amp_factor of oscillator<br>
 * Sets frequency of oscillator depending on amp factor. <br>
 */
void vas_osc_setAmp(vas_osc *x, float amp_factor);
  
#ifdef __cplusplus
}
#endif

#endif /* vas_osc_h */
