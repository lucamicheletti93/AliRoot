//****************************************************************************
//* This file is property of and copyright by the ALICE HLT Project          * 
//* ALICE Experiment at CERN, All rights reserved.                           *
//*                                                                          *
//* Primary Authors: Sergey Gorbunov, Torsten Alt                            *
//* Developers:      Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de> *
//*                  Torsten Alt <talt@cern.ch>                              *
//*                  for The ALICE HLT Project.                              *
//*                                                                          *
//* Permission to use, copy, modify and distribute this software and its     *
//* documentation strictly for non-commercial purposes is hereby granted     *
//* without fee, provided that the above copyright notice appears in all     *
//* copies and that both the copyright notice and this permission notice     *
//* appear in the supporting documentation. The authors make no claims       *
//* about the suitability of this software for any purpose. It is            *
//* provided "as is" without express or implied warranty.                    *
//****************************************************************************

//  @file   AliHLTTPCHWCFMergerUnit.cxx
//  @author Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de>
//  @author Torsten Alt <talt@cern.ch> 
//  @date   
//  @brief  Channel Merger unit of FPGA ClusterFinder Emulator for TPC
//  @brief  ( see AliHLTTPCHWCFEmulator class )
//  @note 

#include "AliHLTTPCHWCFMergerUnit.h"
#include <iostream>

AliHLTTPCHWCFMergerUnit::AliHLTTPCHWCFMergerUnit()
  :
  fDebug(0),
  fMatchDistance(3),
  fMatchTimeFollow(1),
  fDeconvolute(0),
  fByPassMerger(0),
  fInput()
{
  //constructor 
  Init();
}


AliHLTTPCHWCFMergerUnit::~AliHLTTPCHWCFMergerUnit()
{   
  //destructor 
}

AliHLTTPCHWCFMergerUnit::AliHLTTPCHWCFMergerUnit(const AliHLTTPCHWCFMergerUnit&)
  :
  fDebug(0),
  fMatchDistance(3),
  fMatchTimeFollow(1),
  fDeconvolute(0),
  fByPassMerger(0),
  fInput()
{
  // dummy
  Init();
}

AliHLTTPCHWCFMergerUnit& AliHLTTPCHWCFMergerUnit::operator=(const AliHLTTPCHWCFMergerUnit&)
{
  // dummy  
  return *this;
}

int AliHLTTPCHWCFMergerUnit::Init()
{
  // initialise 

  fInput.fFlag = 0;
  for( int i=0; i<2; i++ ){
    fSearchRange[i] = fMemory[i];
    fInsertRange[i] = (fMemory[i]+AliHLTTPCHWCFDefinitions::kMaxNTimeBins);
    fSearchStart[i] = 0;
    fSearchEnd[i] = 0;
    fInsertEnd[i] = 0;
    fInsertRow[i] = -1;
    fInsertPad[i] = -1;
  }    
  return 0;
}

int AliHLTTPCHWCFMergerUnit::InputStream( const AliHLTTPCHWCFClusterFragment *fragment )
{
  // input stream of data 

  fInput.fFlag = 0;

  if( fragment ){
    fInput = *fragment;
    fInput.fSlope = 0;
    fInput.fLastQ = fInput.fQ;
    if( fDebug ){
      std::cout<<"Merger: input F: "<<fragment->fFlag<<" R: "<<fragment->fRow
	       <<" Q: "<<(fragment->fQ>>AliHLTTPCHWCFDefinitions::kFixedPoint)
	       <<" P: "<<fragment->fPad<<" Tmean: "<<fragment->fTMean;	        
      if( fragment->fFlag==1 && fragment->fQ > 0 ){
	std::cout<<" Pw: "<<((float)fragment->fP)/fragment->fQ
		 <<" Tw: "<<((float)fragment->fT)/fragment->fQ;
      }
      std::cout<<std::endl;
    }
  }
  return 0;
}

const AliHLTTPCHWCFClusterFragment *AliHLTTPCHWCFMergerUnit::OutputStream()
{
  // output stream of data 

  if( fInput.fFlag==0 ) return 0;

  if( fByPassMerger ){
    fInsertRange[0][0] = fInput;
    fInput.fFlag = 0;
    return &fInsertRange[0][0];
  }

  if( fInput.fFlag!=1 ){

    for( int ib=0; ib<2; ib++){
      
      // move insert range to search range
      
      if( fSearchStart[ib]>=fSearchEnd[ib] && fInsertEnd[ib]>0 ){	
	AliHLTTPCHWCFClusterFragment *tmp = fSearchRange[ib];
	fSearchRange[ib] = fInsertRange[ib];
	fSearchStart[ib] = 0;
	fSearchEnd[ib] = fInsertEnd[ib];
	fInsertRange[ib] = tmp;
	fInsertEnd[ib] = 0;
	fInsertPad[ib]++;
      }    

      // flush the search range
  
      if( fSearchStart[ib]<fSearchEnd[ib] ){
      fSearchStart[ib]++;
      return &(fSearchRange[ib][fSearchStart[ib]-1]);
      }    
    
      fInsertRow[ib] = -1;
      fInsertPad[ib] = -1;
    }
    
    fInsertRange[0][0] = fInput; // forward the input
    fInput.fFlag = 0;
    return &fInsertRange[0][0];  
  }

  if( fInput.fFlag!=1 ) return 0; // should not happen

  int ib = fInput.fBranch;
  
  // move insert range to search range
    
  if( (int)fInput.fRow!=fInsertRow[ib] || (int)fInput.fPad!=fInsertPad[ib] ){
    
    if( fSearchStart[ib]>=fSearchEnd[ib] && fInsertEnd[ib]>0 ){
      // cout<<"move insert range pad "<<fInsertPad[ib]<<endl;
      AliHLTTPCHWCFClusterFragment *tmp = fSearchRange[ib];
      fSearchRange[ib] = fInsertRange[ib];
      fSearchStart[ib] = 0;
      fSearchEnd[ib] = fInsertEnd[ib];
      fInsertRange[ib] = tmp;
      fInsertEnd[ib] = 0;
      fInsertPad[ib]++;
    }
  }

  // flush the search range
  
  if( (int)fInput.fRow!=fInsertRow[ib] || (int)fInput.fPad!=fInsertPad[ib] ){
    if( fSearchStart[ib]<fSearchEnd[ib] ){
      //cout<<"push from search range at "<<fSearchStart[ib]<<" of "<<fSearchEnd[ib]<<endl;
      fSearchStart[ib]++;
      return &(fSearchRange[ib][fSearchStart[ib]-1]);
    }
  }
    
  fInsertRow[ib] = fInput.fRow;
  fInsertPad[ib] = fInput.fPad;
  
  // flush the search range
  
  if( fSearchStart[ib]<fSearchEnd[ib]  && fSearchRange[ib][fSearchStart[ib]].fTMean>=fInput.fTMean+fMatchDistance
      ){
    //cout<<"push from search range at "<<fSearchStart[ib]<<" of "<<fSearchEnd[ib]<<endl;
    fSearchStart[ib]++;
    return &(fSearchRange[ib][fSearchStart[ib]-1]);
  }

  // merge 
    
  while( fSearchStart[ib]<fSearchEnd[ib]  && fSearchRange[ib][fSearchStart[ib]].fTMean+fMatchDistance>fInput.fTMean ){
    AliHLTTPCHWCFClusterFragment &s = fSearchRange[ib][fSearchStart[ib]++];
    if( fDeconvolute && s.fSlope && s.fLastQ<fInput.fLastQ ){
      //cout<<"push from search range at "<<fSearchStart[ib]-1<<" of "<<fSearchEnd[ib]<<endl;
      return &s;
    }
    // cout<<"merge search range at "<<fSearchStart-1<<" of "<<fSearchEnd<<endl;
    if( !fInput.fSlope && s.fLastQ > fInput.fQ ) fInput.fSlope = 1;
    fInput.fQ += s.fQ;
    fInput.fT += s.fT;
    fInput.fT2 += s.fT2;
    fInput.fP += s.fP;
    fInput.fP2 += s.fP2;
    fInput.fMC.insert(fInput.fMC.end(), s.fMC.begin(), s.fMC.end());
    if( !fMatchTimeFollow ) fInput.fTMean = s.fTMean;
  }
  
  // insert 
  
  fInsertRange[ib][fInsertEnd[ib]++] = fInput;
  fInput.fFlag = 0;
  return 0;
}
