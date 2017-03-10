MODULE := libos32
include /production/main/cm/make/init_P6.make
include ${CM.PATH}/gtn_std.make

# this directory needs to suppress these benign warnings until fixed.
CFLAGS += -erroff=E_INIT_DOES_NOT_FIT
//CFLAGS += -erroff=E_INITIALIZATION_TYPE_MISMATCH
//CFLAGS += -erroff=E_NO_IMPLICIT_DECL_ALLOWED
//FFLAGS += -erroff="WARNING_OPEN_READONLY"

# exclude these headers from direct compilation.
SRCS := $(wildcard *.c)
SRCS := ${SRCS:process_assign.c=}
SRCS := ${SRCS:process_runfiles.c=}
SRCS := ${SRCS:process_cmds.c=}
SRCS := ${SRCS:process_openlu.c=}
SRCS := ${SRCS:process_base.c=}

ifeq (${OS},SunOS)
  # do not build on Sun
  SRCS := ${SRCS:getfilep.c=}
  SRCS := ${SRCS:itime.c=}
  FSRCS := ${FSRCS:getfd.f=}
  FSRCS := ${FSRCS:getfnum.f=}
  FSRCS := ${FSRCS:iargc_getarg.f=}
endif

#show:
#	echo ${SRCS}

INCLUDES ?= Includes_v14_2

ifeq ($(OS),SunOS)
LIBSHMSUPP = libshmsupp_v14_2
else
LIBSHMSUPP = libshmsuppc_v14_2
endif

#LIBOAM ?= std/libOAM_v7_15_1
LIBOAM ?= std/libOAM_v8_6_1
GTNSDK ?= gtnsdk3_v25_296

$(MODULE).INCLUDE = .                           \
                    ${LIBOAM}                   \
                    ${INCLUDES}                 \
                    ${LIBSHMSUPP}               \
                    ${GTNSDK}/include-gtn       \
                    ${GTNSDK}/include-gtngeb    \
                    ${GTNSDK}/include-gtngebbrk 

$(MODULE).LIBS = ${LIBOAM}/libOAM.a 

ifeq ($(OS), Linux)
  CFLAGS += -Wall
endif

include $(CM.PATH)/library_P6.make
