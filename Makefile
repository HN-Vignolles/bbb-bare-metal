ROOT=$(shell pwd)

LIB_PATH=${HOME}/src/gcc-arm-none-eabi-9-2020-q2-update
LIB_GCC=${LIB_PATH}/lib/gcc/arm-none-eabi/9.3.1/
LIB_C=${LIB_PATH}/arm-none-eabi/lib/
GCCR=${HOME}/src/gcc-arm-none-eabi-9-2020-q2-update/bin
PREFIX=arm-none-eabi-

DEVICE=am335x
EVM=beaglebone
TARGET_MODE=Debug
Debug_FLAG=-g
DEVICE_D=-D${DEVICE}
EVM_D=-D${EVM}
TARGET=armv7a
COMPILER=gcc
BOOT=MMCSD
CONSOLE=UARTCONSOLE
CYGPATH=cypath

CC=${GCCR}/${PREFIX}gcc
LD=${GCCR}/${PREFIX}ld
AR=${GCCR}/${PREFIX}ar
OBJC=${GCCR}/${PREFIX}objcopy

CFLAGS=-mcpu=cortex-a8
CFLAGS+=-c ${${TARGET_MODE}_FLAG} -mlong-calls -fdata-sections -funsigned-char \
            -ffunction-sections -Wall ${IPATH} -Dgcc ${DEVICE_D} ${EVM_D} \
            -D SUPPORT_UNALIGNED -D ${BOOT} -D${CONSOLE}
            
#FPU=NEON
#ifeq ($(FPU), NEON)
#CFLAGS+= -mfpu=neon -mfloat-abi=softfp -ftree-vectorize

APP_SRC=test.c
APPNAME=test

LDFLAGS=-e Entry -u Entry -u __aeabi_uidiv -u __aeabi_idiv --gc-sections

APP_LIB=-ldrivers  \
	-lutils    \
	-lplatform \
	-lsystem_config
	
# Pre/recompiled library paths
DRIVERS_BIN=${ROOT}/binary/${TARGET}/${COMPILER}/${DEVICE}/drivers
PLATFORM_BIN=${ROOT}/binary/${TARGET}/${COMPILER}/${DEVICE}/${EVM}/platform
SYSCONFIG_BIN=${ROOT}/binary/${TARGET}/${COMPILER}/${DEVICE}/system_config
UTILITY_BIN=${ROOT}/binary/${TARGET}/${COMPILER}/utils

LPATH=-L"${LIB_C}" \
      -L"${LIB_GCC}" \
      -L${DRIVERS_BIN}/${TARGET_MODE} \
      -L${PLATFORM_BIN}/${TARGET_MODE} \
      -L${SYSCONFIG_BIN}/${TARGET_MODE} \
      -L${UTILITY_BIN}/${TARGET_MODE}

# Include paths
IPATH=-I${ROOT}/include \
      -I${ROOT}/include/hw \
      -I${ROOT}/include/${TARGET}\
      -I${ROOT}/include/${TARGET}/${DEVICE} \
      -I"${LIB_PATH}/include"
	
all:
	${CC}  ${CFLAGS} ${APP_SRC}
	@mkdir -p ${TARGET_MODE}/
	@mv *.o* ${TARGET_MODE}/
	${LD} ${LDFLAGS} ${LPATH} -o ${TARGET_MODE}/${APPNAME}.out \
             -Map ${TARGET_MODE}/${APPNAME}.map ${TARGET_MODE}/*.o* \
	      ${APP_LIB} -lc ${LIBGCC} ${APP_LIB} -lm -lrdimon -lc -lgcc -T ${APPNAME}.lds
	${OBJC} -O binary ${TARGET_MODE}/${APPNAME}.out ${TARGET_MODE}/${APPNAME}.bin
	cp ${TARGET_MODE}/${APPNAME}.bin ${APPNAME}.bin


clean:
	@rm -rf Debug
