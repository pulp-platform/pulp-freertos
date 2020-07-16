REAL_PWD := $(dir $(lastword $(MAKEFILE_LIST)))

INCLUDES := $(INCLUDES) -I$(REAL_PWD)
APPLDIRS := $(APPL_DIRS) $(REAL_PWD)
APPL_COBJS += tm_porting_layer_hrp3.obj tm_porting_layer_hrp3_task.obj

