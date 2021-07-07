/**
 * @file vas_adsr.h
 * @author C.Jaedicke, A.Monciero, P.Schuhladen, F.Müller <br>
 * An interpolated delay <br>
 * <br>
 * @brief Audio Object for adding delay to the input<br>
 * <br>
 * vas_adsr allows for delaying<br>
 * any incoming audio signal. <br>
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

#ifdef __cplusplus
extern "C" {
#endif

    
/**
 * @struct vas_adsr
 * @brief A structure for a delay object <br>
 * @var vas_adsr::buffer The buffer we save the incoming signal in <br>
 * @var vas_adsr::delay_in_samples The parameter value for adjusting the <br>
 * delay of the incoming signal
 * @var vas_adsr::buffer_size The size of the delay buffer <br>
 * @var vas_adsr::circular_pointer Circular pointer to the delay buffer <br>
 * @var vas_adsr::delay_sample The current sample from the delay buffer <br>
 */

typedef struct vas_adsr
{
    int tableSize;
    float index;
    float *lookupTable;
    float currentIndex;

    float att_t;
    float dec_t;
    float sus_t;
    float rel_t;

    float sus_v;
    float att_q;
    float dec_q;
    float rel_q;

} vas_adsr;

/**
 * @related vas_adsr
 * @brief Creates a new delay object<br>
 * The function sets the buffer size and delay parameter of <br>
 * the delay class
 * @return a pointer to the newly created vas_adsr object <br>
 */
vas_adsr *vas_adsr_new(int tableSize);

/**
 * @related vas_adsr
 * @brief Frees a delay object<br>
 * @param x My delay object <br>
 * The function frees the allocated memory<br>
 * of a delay object
 */
void vas_adsr_free(vas_adsr *x);

/**
 * @related vas_adsr
 * @brief Performs the delay in realtime. <br>
 * @param x My delay object <br>
 * @param in The input vector <br>
 * @param out The output vector <br>
 * @param vector_size The size of the i/o vectors <br>
 * The function vas_adsr_perform delays any <br>
 * incoming signal and copies the result to the output vector <br>
 */
void vas_adsr_process(vas_adsr *x, float *in, float *out, int vector_size, int mode);

/**
 * @related vas_adsr
 * @brief Sets the delay time in samples with floating point precision. <br>
 * @param x My delay object <br>
 * @param _delay_in_samples The delay in samples <br>
 * Sets the delay time in samples with floating point precision. <br>
 * Delays exceeding the buffer size are handeled by setting the delay to zero. <br>
 */
void vas_adsr_updateADSR(vas_adsr *x, float a, float d, float s , float r);
float vas_adsr_func_attack_normalized(float x,float q);
float vas_adsr_func_decay_normalized(float x,float q,float s);
float vas_adsr_func_release_normalized(float x,float q,float s);
void vas_adsr_setQ(vas_adsr *x, float qa, float qd, float qr);
#ifdef __cplusplus
}
#endif

#endif /* vas_adsr_h */
