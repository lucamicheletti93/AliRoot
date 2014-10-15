# **************************************************************************
# * Copyright(c) 1998-2014, ALICE Experiment at CERN, All rights reserved. *
# *                                                                        *
# * Author: The ALICE Off-line Project.                                    *
# * Contributors are mentioned in the code where appropriate.              *
# *                                                                        *
# * Permission to use, copy, modify and distribute this software and its   *
# * documentation strictly for non-commercial purposes is hereby granted   *
# * without fee, provided that the above copyright notice appears in all   *
# * copies and that both the copyright notice and this permission notice   *
# * appear in the supporting documentation. The authors make no claims     *
# * about the suitability of this software for any purpose. It is          *
# * provided "as is" without express or implied warranty.                  *
# **************************************************************************

# Module
set(MODULE ITSbase)

# Module include folder
include_directories(${CMAKE_SOURCE_DIR}/ITS/${MODULE})

# Additional include folders in alphabetical order except ROOT
include_directories(${ROOT_INCLUDE_DIRS} 
                    ${CMAKE_SOURCE_DIR}/ITS/ITSrec
                    ${CMAKE_SOURCE_DIR}/ITS/ITSsim
                    ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase
                    ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec
                    ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim
                    ${CMAKE_SOURCE_DIR}/RAW/dateStream
                    ${CMAKE_SOURCE_DIR}/STEER/CDB
                    ${CMAKE_SOURCE_DIR}/STEER/ESD
                    ${CMAKE_SOURCE_DIR}/STEER/STEERBase
                    ${CMAKE_SOURCE_DIR}/STEER/STEER
                    ${CMAKE_SOURCE_DIR}/TPC
                   )

# Sources in alphabetical order
set(SRCS
    AliITSBadChannelsSSD.cxx
    AliITSBadChannelsSSDv2.cxx
    AliITSCalibration.cxx
    AliITSCalibrationSDD.cxx
    AliITSCalibrationSPD.cxx
    AliITSCalibrationSSD.cxx
    AliITSChannelDaSSD.cxx
    AliITSChannelStatus.cxx
    AliITSclusterV2.cxx
    AliITSCompressRawDataSDD.cxx
    AliITSCorrMap1DSDD.cxx
    AliITSCorrMap2DSDD.cxx
    AliITSCorrMapSDD.cxx
    AliITSDCSAnalyzerSDD.cxx
    AliITSDCSDataSDD.cxx
    AliITSDDLModuleMapSDD.cxx
    AliITSDDLRawData.cxx
    AliITSdEdxSamples.cxx
    AliITSdigit.cxx
    AliITSdigitSDD.cxx
    AliITSdigitSPD.cxx
    AliITSdigitSSD.cxx
    AliITSDriftSpeedArraySDD.cxx
    AliITSDriftSpeedSDD.cxx
    AliITSEventHeader.cxx
    AliITSGainSSD.cxx
    AliITSGainSSDv2.cxx
    AliITSgeom.cxx
    AliITSgeomMatrix.cxx
    AliITSgeomTGeo.cxx
    AliITSHandleDaSSD.cxx
    AliITSHLTforSDD.cxx
    AliITSInitGeometry.cxx
    AliITSIntMap.cxx
    AliITSIntMapNode.cxx
    AliITSLoader.cxx
    AliITSMapA1.cxx
    AliITSMapA2.cxx
    AliITSMap.cxx
    AliITSMapSDD.cxx
    AliITSMisAligner.cxx
    AliITSModuleDaSSD.cxx
    AliITSNoiseSSD.cxx
    AliITSNoiseSSDv2.cxx
    AliITSPedestalSSD.cxx
    AliITSPedestalSSDv2.cxx
    AliITSPlaneEff.cxx
    AliITSPlaneEffSDD.cxx
    AliITSPlaneEffSPD.cxx
    AliITSPlaneEffSSD.cxx
    AliITSpList.cxx
    AliITSpListItem.cxx
    AliITSRawStream.cxx
    AliITSRawStreamSDDCompressed.cxx
    AliITSRawStreamSDD.cxx
    AliITSRawStreamSPD.cxx
    AliITSRawStreamSPDErrorLog.cxx
    AliITSRawStreamSSD.cxx
    AliITSRawStreamSSDv1.cxx
    AliITSRecPoint.cxx
    AliITSresponse.cxx
    AliITSresponseSDD.cxx
    AliITSresponseSPD.cxx
    AliITSsegmentation.cxx
    AliITSsegmentationSDD.cxx
    AliITSsegmentationSPD.cxx
    AliITSsegmentationSSD.cxx
    AliITSSurveyToAlign.cxx
    AliITSTransientDigit.cxx
    AliITSTriggerAlgorithmConditions.cxx
    AliITSTriggerConditions.cxx
   )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Generate the dictionary
# It will create G_ARG1.cxx and G_ARG1.h / ARG1 = function first argument
get_directory_property(incdirs INCLUDE_DIRECTORIES)
generate_dictionary("${MODULE}" "${MODULE}LinkDef.h" "${HDRS}" "${incdirs}")

# Generate the ROOT map
# Dependecies
set(LIBDEPS CDB ESD STEERBase STEER ITSrec ITSsim RAWDatabase RAWDatarec RAWDatasim GenVector Gui)
generate_rootmap("${MODULE}" "${LIBDEPS}" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}LinkDef.h")


# Add a library to the project using the specified source files
add_library(${MODULE} SHARED ${SRCS} G__${MODULE}.cxx)

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES COMPILE_FLAGS "-O -g")

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
    set_target_properties(${MODULE} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

install(FILES ${HDRS} DESTINATION include)