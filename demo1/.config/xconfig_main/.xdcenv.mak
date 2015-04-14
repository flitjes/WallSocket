#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/flitjesdev/ti/grace_3_00_02_64/packages;/home/flitjesdev/ti/msp430/MSP430ware_1_97_00_47/driverlib/packages;/home/flitjesdev/ti/msp430/MSP430ware_1_97_00_47/driverlib;/home/flitjesdev/ti/ccsv6/ccs_base;/home/flitjesdev/workspace_v6_0/demo1/.config
override XDCROOT = /home/flitjesdev/ti/xdctools_3_30_06_67_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/flitjesdev/ti/grace_3_00_02_64/packages;/home/flitjesdev/ti/msp430/MSP430ware_1_97_00_47/driverlib/packages;/home/flitjesdev/ti/msp430/MSP430ware_1_97_00_47/driverlib;/home/flitjesdev/ti/ccsv6/ccs_base;/home/flitjesdev/workspace_v6_0/demo1/.config;/home/flitjesdev/ti/xdctools_3_30_06_67_core/packages;..
HOSTOS = Linux
endif
