/**
 * @file rtap_fmMultiOsc_pd.c
 * @author Thomas Resch <br>
 * Audiocommunication Group, Technical University Berlin <br>
 * University of Applied Sciences Nordwestschweiz (FHNW), Music-Academy, Research and Development <br>
 * A gain object for pure data <br>
 * <br>
 * @brief A Pure Data gain object for adjusting the volume<br>
 * <br>
 * rtap_fmMultiOsc~ allows for adjusting the level<br>
 * of any incoming audio signal. <br>
 * <br>
 */

#include "m_pd.h"
#include "vas_osc.h"

static t_class *rtap_fmMultiOsc_tilde_class;

/**
 * @struct rtap_fmMultiOsc_tilde
 * @brief The Pure Data struct of the rtap_fmMultiOsc~ object. <br>
 * @var rtap_fmMultiOsc_tilde::x_obj Necessary for every signal object in Pure Data <br>
 * @var rtap_fmMultiOsc_tilde::f Also necessary for signal objects, float dummy dataspace <br>
 * for converting a float to signal if no signal is connected (CLASS_MAINSIGNALIN) <br>
 * @var rtap_fmMultiOsc_tilde::gain The gain object for the actual signal processing <br>
 * @var rtap_fmMultiOsc_tilde::x_out A signal outlet for the adjusted signal
 * level of the incoming signal
 */

typedef struct rtap_fmMultiOsc_tilde
{
    t_object  x_obj;
    t_sample f;
    vas_osc *osc1;
    vas_osc *osc2;
    //vas_osc_mod *osc_m;
    t_word *table;
    
    t_outlet *out;
} rtap_fmMultiOsc_tilde;

/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Calculates the volume adjusted output vector<br>
 * @param w A pointer to the object, input and output vectors. <br>
 * For more information please refer to the Pure Data Docs <br>
 * The function calls the rtap_fmMultiOsc_perform method. <br>
 * @return A pointer to the signal chain right behind the rtap_fmMultiOsc_tilde object. <br>
 */

t_int *rtap_fmMultiOsc_tilde_perform(t_int *w)
{
    rtap_fmMultiOsc_tilde *x = (rtap_fmMultiOsc_tilde *)(w[1]);
    t_sample  *in = (t_sample *)(w[2]);
    t_sample  *out =  (t_sample *)(w[3]);
    int n =  (int)(w[4]);

    vas_osc_process(x->osc1, in, out, n, MODE_MOD_NO_INPUT);
    vas_osc_process(x->osc2, in, out, n, MODE_CARRIER_WITH_INPUT);

    /* return a pointer to the dataspace for the next dsp-object */
    return (w+5);
}

/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Adds rtap_fmMultiOsc_tilde_perform to the signal chain. <br>
 * @param x A pointer the rtap_fmMultiOsc_tilde object <br>
 * @param sp A pointer the input and output vectors <br>
 * For more information please refer to the <a href = "https://github.com/pure-data/externals-howto" > Pure Data Docs </a> <br>
 */

void rtap_fmMultiOsc_tilde_dsp(rtap_fmMultiOsc_tilde *x, t_signal **sp)
{
    dsp_add(rtap_fmMultiOsc_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Frees our object. <br>
 * @param x A pointer the rtap_fmMultiOsc_tilde object <br>
 * For more information please refer to the <a href = "https://github.com/pure-data/externals-howto" > Pure Data Docs </a> <br>
 */

void rtap_fmMultiOsc_tilde_free(rtap_fmMultiOsc_tilde *x)
{
    outlet_free(x->out);
    vas_osc_free(x->osc1);
    vas_osc_free(x->osc2);
    //vas_osc_free(x->osc_m);
}

/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Creates a new rtap_fmMultiOsc_tilde object.<br>
 * @param f Sets the initial gain value. <br>
 * For more information please refer to the <a href = "https://github.com/pure-data/externals-howto" > Pure Data Docs </a> <br>
 */

void rtap_getArray(rtap_fmMultiOsc_tilde *x, t_symbol *arrayname, t_word **array, int *length)
{
    t_garray *a;
    if (!(a = (t_garray *)pd_findbyclass(arrayname, garray_class)))
    {
        if (*arrayname->s_name) pd_error(x, "vas_osc~: %s: no such array",
            arrayname->s_name);
        *array = 0;
    }
    else if (!garray_getfloatwords(a, length, array))
    {
        pd_error(x, "%s: bad template for rtap_fmMultiOsc~", arrayname->s_name);
        *array = 0;
    }
    else
    {
        post("Reading IRs from array %s", arrayname->s_name);
    }
}

void rtap_write2FloatArray_osc1(rtap_fmMultiOsc_tilde *x)
{
    for (int i=0;i<44100; i++)
    {
        x->osc1->lookupTable[i] = x->table[i].w_float;
    }
}

void rtap_write2FloatArray_osc2(rtap_fmMultiOsc_tilde *x)
{
    for (int i=0;i<44100; i++)
    {
        x->osc2->lookupTable[i] = x->table[i].w_float;
    }
}

void rtap_fmMultiOsc_tilde_setExternTable_osc1(rtap_fmMultiOsc_tilde *x, t_symbol *name)
{
    int length = 0;
    rtap_getArray(x, name, &x->table, &length);
    rtap_write2FloatArray_osc1(x);
}

void rtap_fmMultiOsc_tilde_setExternTable_osc2(rtap_fmMultiOsc_tilde *x, t_symbol *name)
{
    int length = 0;
    rtap_getArray(x, name, &x->table, &length);
    rtap_write2FloatArray_osc2(x);
}

void rtap_fmMultiOsc_tilde_setFrequency_osc1(rtap_fmMultiOsc_tilde *x, float frequency)
{
    vas_osc_setFrequency(x->osc1, frequency);
}

void rtap_fmMultiOsc_tilde_setFrequency_osc2(rtap_fmMultiOsc_tilde *x, float frequency)
{
    vas_osc_setFrequency(x->osc2, frequency);
}

void rtap_fmMultiOsc_tilde_setAmp_osc1(rtap_fmMultiOsc_tilde *x, float amp)
{
    vas_osc_setAmp(x->osc1, amp);
}

void rtap_fmMultiOsc_tilde_setAmp_osc2(rtap_fmMultiOsc_tilde *x, float amp)
{
    vas_osc_setAmp(x->osc2, amp);
}

void rtap_fmMultiOsc_tilde_setADSR_osc1(rtap_fmMultiOsc_tilde *x, float a, float d, float s, float r)
{
    vas_osc_updateADSR(x->osc1, a,d,s,r);
}

void rtap_fmMultiOsc_tilde_setADSR_osc2(rtap_fmMultiOsc_tilde *x, float a, float d, float s, float r)
{
    vas_osc_updateADSR(x->osc2, a,d,s,r);
}


void *rtap_fmMultiOsc_tilde_new(t_floatarg f)
{
    rtap_fmMultiOsc_tilde *x = (rtap_fmMultiOsc_tilde *)pd_new(rtap_fmMultiOsc_tilde_class);
    
    //The main inlet is created automatically
    x->out = outlet_new(&x->x_obj, &s_signal);
    x->osc1 = vas_osc_new(44100);
    x->osc2 = vas_osc_new(44100);
    //x->osc_m = vas_osc_mod_new(44100);

    return (void *)x;
}


/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Sets the gain adjustment parameter. <br>
 * @param x A pointer the rtap_fmMultiOsc_tilde object <br>
 * @param level Sets the level parameter <br>
 * For more information please refer to the <a href = "https://github.com/pure-data/externals-howto" > Pure Data Docs </a> <br>
 */


/**
 * @related rtap_fmMultiOsc_tilde
 * @brief Setup of rtap_fmMultiOsc_tilde <br>
 * For more information please refer to the <a href = "https://github.com/pure-data/externals-howto" > Pure Data Docs </a> <br>
 */

void rtap_fmMultiOsc_tilde_setup(void)
{
      rtap_fmMultiOsc_tilde_class = class_new(gensym("rtap_fmMultiOsc~"),
            (t_newmethod)rtap_fmMultiOsc_tilde_new,
            (t_method)rtap_fmMultiOsc_tilde_free,
        sizeof(rtap_fmMultiOsc_tilde),
            CLASS_DEFAULT,
            A_DEFFLOAT, 0);

      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_dsp, gensym("dsp"), 0);

      // this adds the gain message to our object
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setFrequency_osc1, gensym("freq_osc1"), A_DEFFLOAT, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setFrequency_osc2, gensym("freq_osc2"), A_DEFFLOAT, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setExternTable_osc1, gensym("table_osc1"), A_SYMBOL, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setExternTable_osc2, gensym("table_osc2"), A_SYMBOL, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setAmp_osc1, gensym("amp_osc1"), A_DEFFLOAT, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setAmp_osc2, gensym("amp_osc2"), A_DEFFLOAT, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setADSR_osc1, gensym("adsr_osc1"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
      class_addmethod(rtap_fmMultiOsc_tilde_class, (t_method)rtap_fmMultiOsc_tilde_setADSR_osc2, gensym("adsr_osc2"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

      CLASS_MAINSIGNALIN(rtap_fmMultiOsc_tilde_class, rtap_fmMultiOsc_tilde, f);
}
