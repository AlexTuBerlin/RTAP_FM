# Makefile to build class 'stp_gain' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name

# add folder shortcut examples:
# INCLUDES = -I../includes
# TESTING = testing

lib.name = rtap_fmMultiOsc~

# input source file (class name == source file basename)
class.sources = rtap_fmMultiOsc~.c
rtap_fmMultiOsc~.class.sources += vas_mem.c
rtap_fmMultiOsc~.class.sources += vas_osc.c


# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=pd-lib-builder/

CC += $(INCLUDES)
# CC +=  -mavx -DVAS_USE_AVX

include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder




