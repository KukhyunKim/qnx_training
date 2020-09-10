# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs.mk

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION=qnx trining test
endef
LIBS+=slog2 gpio_clientS ^pmic_clientS spmi_clientS
LIBS+=gpio_client

LIBS+=chipinfoS dalsysS slog2 dalcfgS nspinclockcyclesS dalconfig smemS hwioS

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
EXTRA_SRCVPATH+=$(PROJECT_ROOT)/src

INCVPATH+=$(AMSS_INC)

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(PROJECT_ROOT)/inc  \
	  $(BSP_A_FAMILY_INC) \
	  $(AMSS_INC)/core \
	  $(PROJECT_ROOT)/public/amss/core \
	  $(INSTALL_ROOT_nto)/usr/include/amss/core \
	  $(PROCESSES_INC) \
	  $(INSTALL_ROOT_nto)/usr/include/amss \
	  $(INSTALL_ROOT_nto)/usr/include/qct\
	  $(AMSS_INC_MM_AF)

	  
#===== INSTALLDIR - Subdirectory where the executable or library is to be installed. 

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
QNX_INTERNAL=$(BSP_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)
include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))



