#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
MKDIR=mkdir -p
RM=rm -f 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/CANm.o ${OBJECTDIR}/_ext/1018694054/can18xx8.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH=/usr/lib/jvm/java-6-openjdk/jre/bin/
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=/opt/microchip/mplabc18/v3.40/bin/mcc18
# MP_BC is not defined
MP_AS=/opt/microchip/mplabc18/v3.40/bin/../mpasm/MPASMWIN
MP_LD=/opt/microchip/mplabc18/v3.40/bin/mplink
MP_AR=/opt/microchip/mplabc18/v3.40/bin/mplib
# MP_BC is not defined
MP_CC_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined
MP_AS_DIR=/opt/microchip/mplabc18/v3.40/bin/../mpasm
MP_LD_DIR=/opt/microchip/mplabc18/v3.40/bin
MP_AR_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined

# This makefile will use a C preprocessor to generate dependency files
MP_CPP=/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/mplab-cpp

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof

MP_PROCESSOR_OPTION=18F2680
MP_PROCESSOR_OPTION_LD=18f2680
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0xfd80 -u_DEBUGCODELEN=0x280 -u_DEBUGDATASTART=0xcf4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: createRevGrep
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
else
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/CANm.o: CANm.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/CANm.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/CANm.o   CANm.c  > ${OBJECTDIR}/CANm.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/CANm.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/CANm.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/CANm.o.temp CANm.c __temp_cpp_output__ -D __18F2680 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2680
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/CANm.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/CANm.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/CANm.o.d
else
	cat ${OBJECTDIR}/CANm.o.temp >> ${OBJECTDIR}/CANm.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1018694054/can18xx8.o: ../libcan/can18xx8.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1018694054 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1018694054/can18xx8.o   ../libcan/can18xx8.c  > ${OBJECTDIR}/_ext/1018694054/can18xx8.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1018694054/can18xx8.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1018694054/can18xx8.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp ../libcan/can18xx8.c __temp_cpp_output__ -D __18F2680 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2680
	printf "%s/" ${OBJECTDIR}/_ext/1018694054 > ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
else
	cat ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp >> ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
endif
	${RM} __temp_cpp_output__
else
${OBJECTDIR}/CANm.o: CANm.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/CANm.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/CANm.o   CANm.c  > ${OBJECTDIR}/CANm.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/CANm.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/CANm.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/CANm.o.temp CANm.c __temp_cpp_output__ -D __18F2680 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2680
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/CANm.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/CANm.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/CANm.o.d
else
	cat ${OBJECTDIR}/CANm.o.temp >> ${OBJECTDIR}/CANm.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1018694054/can18xx8.o: ../libcan/can18xx8.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1018694054 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1018694054/can18xx8.o   ../libcan/can18xx8.c  > ${OBJECTDIR}/_ext/1018694054/can18xx8.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1018694054/can18xx8.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1018694054/can18xx8.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp ../libcan/can18xx8.c __temp_cpp_output__ -D __18F2680 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2680
	printf "%s/" ${OBJECTDIR}/_ext/1018694054 > ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
else
	cat ${OBJECTDIR}/_ext/1018694054/can18xx8.o.temp >> ${OBJECTDIR}/_ext/1018694054/can18xx8.o.d
endif
	${RM} __temp_cpp_output__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
else
dist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/CANm.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
