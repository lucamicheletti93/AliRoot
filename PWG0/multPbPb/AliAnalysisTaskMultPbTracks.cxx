// AliAnalysisTaskMultPbTracks

// Author: Michele Floris, CERN
// TODO:
// - Add chi2/cluster plot for primary, secondaries and fakes

#include "AliAnalysisTaskMultPbTracks.h"
#include "AliESDInputHandler.h"
#include "AliAnalysisMultPbTrackHistoManager.h"
#include "AliAnalysisManager.h"
#include "AliESDtrackCuts.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "TH1I.h"
#include "TH3D.h"
#include "AliMCParticle.h"
#include "AliGenEventHeader.h"
#include "AliESDCentrality.h"
#include "AliMultiplicity.h"
#include <iostream>
#include "AliAnalysisMultPbCentralitySelector.h"

using namespace std;

ClassImp(AliAnalysisTaskMultPbTracks)

AliAnalysisTaskMultPbTracks::AliAnalysisTaskMultPbTracks()
: AliAnalysisTaskSE("TaskMultPbTracks"),
  fESD(0),fHistoManager(0),fCentrSelector(0),fTrackCuts(0),fTrackCutsNoDCA(0),fIsMC(0)
{
  // constructor

  DefineOutput(1, AliAnalysisMultPbTrackHistoManager::Class());
  DefineOutput(2, AliESDtrackCuts::Class());
  DefineOutput(3, AliAnalysisMultPbCentralitySelector::Class());

  fHistoManager = new AliAnalysisMultPbTrackHistoManager("histoManager","Hitograms, Multiplicity, Track analysis");
  if(fIsMC) fHistoManager->SetSuffix("MC");

}
AliAnalysisTaskMultPbTracks::AliAnalysisTaskMultPbTracks(const char * name)
  : AliAnalysisTaskSE(name),
    fESD(0),fHistoManager(0),fCentrSelector(0),fTrackCuts(0),fTrackCutsNoDCA(0),fIsMC(0)
{
  //
  // Standard constructur which should be used
  //

  DefineOutput(1, AliAnalysisMultPbTrackHistoManager::Class());
  DefineOutput(2, AliESDtrackCuts::Class());
  DefineOutput(3, AliAnalysisMultPbCentralitySelector::Class());

  fHistoManager = new AliAnalysisMultPbTrackHistoManager("histoManager","Hitograms, Multiplicity, Track analysis");
  if(fIsMC) fHistoManager->SetSuffix("MC");

}

AliAnalysisTaskMultPbTracks::AliAnalysisTaskMultPbTracks(const AliAnalysisTaskMultPbTracks& obj) : 
  AliAnalysisTaskSE(obj) ,fESD (0), fHistoManager(0), fCentrSelector(0), fTrackCuts(0),fTrackCutsNoDCA(0),fIsMC(0)
{
  //copy ctor
  fESD = obj.fESD ;
  fHistoManager= obj.fHistoManager;
  fTrackCuts  = obj.fTrackCuts;
  fTrackCutsNoDCA  = obj.fTrackCutsNoDCA;
  fCentrSelector = obj.fCentrSelector;

}

AliAnalysisTaskMultPbTracks::~AliAnalysisTaskMultPbTracks(){
  // destructor

  if(!AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
    if(fHistoManager) {
      delete fHistoManager;
      fHistoManager = 0;
    }
  }
  // Histo list should not be destroyed: fListWrapper is owner!

}
void AliAnalysisTaskMultPbTracks::UserCreateOutputObjects()
{
  // Called once

  // For the DCA distributions, we want to use exactly the same cuts
  // as for the other distributions, with the exception of the DCA cut
  fTrackCutsNoDCA = new AliESDtrackCuts(*fTrackCuts); // clone cuts
  // Reset all DCA cuts; FIXME: is this all?
  fTrackCutsNoDCA->SetMaxDCAToVertexXY();
  fTrackCutsNoDCA->SetMaxDCAToVertexZ ();
  fTrackCutsNoDCA->SetMaxDCAToVertexXYPtDep();
  fTrackCutsNoDCA->SetMaxDCAToVertexZPtDep();

}


void AliAnalysisTaskMultPbTracks::UserExec(Option_t *)
{
  // User code

  /* PostData(0) is taken care of by AliAnalysisTaskSE */
  PostData(1,fHistoManager);
  PostData(2,fTrackCuts);
  PostData(3,fCentrSelector);

  // Cache histogram pointers
  static TH3D * hTracks  [AliAnalysisMultPbTrackHistoManager::kNHistos];
  static TH1D * hDCA     [AliAnalysisMultPbTrackHistoManager::kNHistos];
  static TH1D * hNTracks [AliAnalysisMultPbTrackHistoManager::kNHistos];
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoGen]        = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoGen       );
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRec]        = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRec       );
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecPrim]    = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRecPrim   );
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecFake]    = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRecFake   );
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat]  = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat );
  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak] = fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak);

  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoGen]        = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoGen       );
  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRec]        = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoRec       );
  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecPrim]    = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoRecPrim   );
  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecFake]    = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoRecFake   );
  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat]  = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat );
  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak] = fHistoManager->GetHistoDCA(AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak);

  hNTracks[AliAnalysisMultPbTrackHistoManager::kHistoGen]        = fHistoManager->GetHistoMult(AliAnalysisMultPbTrackHistoManager::kHistoGen );
  hNTracks[AliAnalysisMultPbTrackHistoManager::kHistoRec]        = fHistoManager->GetHistoMult(AliAnalysisMultPbTrackHistoManager::kHistoRec );

  fESD = dynamic_cast<AliESDEvent*>(fInputEvent);
  if (strcmp(fESD->ClassName(),"AliESDEvent")) {
    AliFatal("Not processing ESDs");
  }
  
  // FIXME: use physics selection here to keep track of events lost?
  fHistoManager->GetHistoStats()->Fill(AliAnalysisMultPbTrackHistoManager::kStatAll);

  Bool_t isSelected = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & fOfflineTrigger);

  if(!isSelected) return;
  fHistoManager->GetHistoStats()->Fill(AliAnalysisMultPbTrackHistoManager::kStatPhysSel);


  // Centrality selection
  Bool_t isCentralitySelected = fCentrSelector->IsCentralityBinSelected(fESD,fTrackCuts);  
  if(!isCentralitySelected) return;

  // AliESDCentrality *centrality = fESD->GetCentrality();
  // if(!centrality) {
  //   AliFatal("Centrality object not available"); 
  // }
  // else {
  //   Int_t centrBin = centrality->GetCentralityClass5(fCentralityEstimator.Data()) ;
    
  //   if (centrBin != fCentrBin && fCentrBin != -1) return;
  // }

  fHistoManager->GetHistoStats()->Fill(AliAnalysisMultPbTrackHistoManager::kStatCentr);



  if (fIsMC) {
    // Get MC vertex
    //FIXME: which vertex do I take for MC?
    TArrayF   vertex;
    fMCEvent->GenEventHeader()->PrimaryVertex(vertex);
    Float_t zvGen = vertex[2];    

    if (!fMCEvent) {
      AliError("No MC info found");
    } else {
      
      //loop on the MC event, only over primaries, which are always
      //      the first in stack.
      Int_t nMCTracks = fMCEvent->GetNumberOfPrimaries();
      Int_t nPhysicalPrimaries = 0;
      for (Int_t ipart=0; ipart<nMCTracks; ipart++) { 
	
	AliMCParticle *mcPart  = (AliMCParticle*)fMCEvent->GetTrack(ipart);
	
	// We don't care about neutrals and non-physical primaries
	if(mcPart->Charge() == 0) continue;

	//check if current particle is a physical primary
	if(!IsPhysicalPrimaryAndTransportBit(ipart)) continue;
 
	nPhysicalPrimaries++;
	// Fill species histo and particle species
	fHistoManager->GetHistoProcess(AliAnalysisMultPbTrackHistoManager::kHistoGen)->Fill(mcPart->Particle()->GetUniqueID());
	fHistoManager->FillParticleID(AliAnalysisMultPbTrackHistoManager::kHistoGen, mcPart);
	
	//	Float_t zv = vtxESD->GetZ();
	// Fill generated histo
	hTracks[AliAnalysisMultPbTrackHistoManager::kHistoGen]->Fill(mcPart->Pt(),mcPart->Eta(),zvGen);
	Int_t partCode = fHistoManager->GetLocalParticleID(mcPart);
	if (partCode == AliAnalysisMultPbTrackHistoManager::kPartPiPlus  || 
	    partCode == AliAnalysisMultPbTrackHistoManager::kPartPiMinus)
	  fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoGen, partCode);
	
      }
      hNTracks[AliAnalysisMultPbTrackHistoManager::kHistoGen]->Fill(nPhysicalPrimaries);
      fHistoManager->GetHistoVzEvent(AliAnalysisMultPbTrackHistoManager::kHistoGen)->Fill(zvGen);
  
    
    }
  }
  

  // FIXME: shall I take the primary vertex?
  const AliESDVertex* vtxESD = fESD->GetPrimaryVertex();
  if(!vtxESD) return;
  // FIXME: leave the cuts here or find a better place?)
  // Quality cut on vertexer Z, as suggested by Francesco Prino
  if(vtxESD->IsFromVertexerZ()) {
    if (vtxESD->GetNContributors() <= 0) return;
    if (vtxESD->GetDispersion() >= 0.04) return;
    if (vtxESD->GetZRes() >= 0.25) return;
  }
  // "Beam gas" vertex cut
  const AliESDVertex * vtxESDTPC= fESD->GetPrimaryVertexTPC(); 
  if (vtxESDTPC->GetNContributors()<(-10.+0.25*fESD->GetMultiplicity()->GetNumberOfITSClusters(0)))     return;
  fHistoManager->GetHistoStats()->Fill(AliAnalysisMultPbTrackHistoManager::kStatVtx);
  fHistoManager->GetHistoVzEvent(AliAnalysisMultPbTrackHistoManager::kHistoRec)->Fill(vtxESD->GetZ());

  // loop on tracks
  Int_t ntracks = fESD->GetNumberOfTracks();
  Int_t acceptedTracks = 0;

  for (Int_t itrack = 0; itrack<ntracks; itrack++) {    
    AliESDtrack * esdTrack = fESD->GetTrack(itrack);

    // Fill DCA distibutions, without the DCA cut, Fill the other stuff, with all cuts!
    Bool_t accepted = fTrackCuts->AcceptTrack(esdTrack);
    Bool_t acceptedNoDCA = fTrackCutsNoDCA->AcceptTrack(esdTrack);

    if(accepted) acceptedTracks++;

    // Compute weighted offset
    // TODO: other choiches for the DCA?
    Double_t b = fESD->GetMagneticField();
    Double_t dca[2];
    Double_t cov[3];
    Double_t weightedDCA = 10;
    


    if (esdTrack->PropagateToDCA(vtxESD, b,3., dca, cov)) {
      Double_t det = cov[0]*cov[2]-cov[1]*cov[1]; 
      if (det<=0) {
	AliError("DCA Covariance matrix is not positive definite!");
      }
      else {
	weightedDCA = (dca[0]*dca[0]*cov[2]+dca[1]*dca[1]*cov[0]-2*dca[0]*dca[1]*cov[1])/det; 
	weightedDCA = weightedDCA>0 ? TMath::Sqrt(weightedDCA) : 10;
      }
      //      printf("dR:%e dZ%e  sigR2:%e sigRZ:%e sigZ2:%e\n",dca[0],dca[1],cov[0],cov[1],cov[2]);
    }
    

    

    // Alternative: p*DCA
    // Float_t xz[2]; 
    // esdTrack->GetDZ(vtxESD->GetX(),vtxESD->GetY(),vtxESD->GetZ(), fESD->GetMagneticField(), xz); 
    // Float_t dca = xz[0]*esdTrack->P();

    // for each track
    if(accepted) 
      hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRec]->Fill(esdTrack->Pt(),esdTrack->Eta(),vtxESD->GetZ());
    if(acceptedNoDCA)
      hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRec]->Fill(weightedDCA);

    // Fill separately primaries and secondaries
    // FIXME: fakes? Is this the correct way to account for them?
    // Get label and corresponding mcPart;
    if (fIsMC) {
      //      Int_t label = TMath::Abs(esdTrack->GetLabel()); // no fakes!!!
      Int_t label = esdTrack->GetLabel(); // 
      AliMCParticle *mcPart  = label < 0 ? 0 : (AliMCParticle*)fMCEvent->GetTrack(label);
      if (!mcPart)  {
	if(accepted)
	  hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecFake]->Fill(esdTrack->Pt(),esdTrack->Eta(),vtxESD->GetZ());
	if(acceptedNoDCA)
	  hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecFake]->Fill(weightedDCA);
      }
      else {
	if(IsPhysicalPrimaryAndTransportBit(label)) {
	  // Fill species histo
	  fHistoManager->GetHistoProcess(AliAnalysisMultPbTrackHistoManager::kHistoRecPrim)->Fill(mcPart->Particle()->GetUniqueID());
	  if(accepted) {
	    hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecPrim]->Fill(esdTrack->Pt(),esdTrack->Eta(),vtxESD->GetZ());
	    Int_t partCode = fHistoManager->GetLocalParticleID(mcPart);
	    if (partCode == AliAnalysisMultPbTrackHistoManager::kPartPiPlus  || 
		partCode == AliAnalysisMultPbTrackHistoManager::kPartPiMinus)
	      fHistoManager->GetHistoPtEtaVz(AliAnalysisMultPbTrackHistoManager::kHistoRecPrim, partCode);
	  }
	  if(acceptedNoDCA)
	    hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecPrim]->Fill(weightedDCA);
	} 
	else {
	  Int_t mfl=0;
	  Int_t indexMoth=mcPart->Particle()->GetFirstMother();
	  if(indexMoth>=0){
	    TParticle* moth = fMCEvent->Stack()->Particle(indexMoth);
	    Float_t codemoth = TMath::Abs(moth->GetPdgCode());
	    mfl = Int_t (codemoth/ TMath::Power(10, Int_t(TMath::Log10(codemoth))));
	  }
	  if(mfl==3){ // strangeness
	    fHistoManager->GetHistoProcess(AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak)->Fill(mcPart->Particle()->GetUniqueID());
	    if(accepted)
	      hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak]->Fill(esdTrack->Pt(),esdTrack->Eta(),vtxESD->GetZ());
	    if(acceptedNoDCA)
	      hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecSecWeak]->Fill(weightedDCA);	  
	  }else{ // material
	    fHistoManager->GetHistoProcess(AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat)->Fill(mcPart->Particle()->GetUniqueID());
	    if(accepted)
	      hTracks[AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat]->Fill(esdTrack->Pt(),esdTrack->Eta(),vtxESD->GetZ());
	    if(acceptedNoDCA)
	      hDCA[AliAnalysisMultPbTrackHistoManager::kHistoRecSecMat]->Fill(weightedDCA);	  

	  }


	}
      }
    }


  }
  //  cout << acceptedTracks << endl;
  
  hNTracks[AliAnalysisMultPbTrackHistoManager::kHistoRec]  ->Fill(acceptedTracks);
  // FIXME
  //  hNTracks[AliAnalysisMultPbTrackHistoManager::kHistoRec]  ->Fill(fESD->GetMultiplicity()->GetNumberOfTracklets());


}

void   AliAnalysisTaskMultPbTracks::Terminate(Option_t *){
  // terminate

}


Bool_t AliAnalysisTaskMultPbTracks::IsPhysicalPrimaryAndTransportBit(Int_t ipart) {

  Bool_t physprim=fMCEvent->IsPhysicalPrimary(ipart);
  if (!physprim) return kFALSE;
  Bool_t transported = ((AliMCParticle*)fMCEvent->GetTrack(ipart))->Particle()->TestBit(kTransportBit);
  if(!transported) return kFALSE;

  return kTRUE;

}

// void AliAnalysisTaskEvil::PrintProcInfo()
// {
//   ProcInfo_t info;
//   gSystem->GetProcInfo(&info);
//   AliInfo(Form("fMemResident=%10ld kB  fMemVirtual=%10ld kB",info.fMemResident,info.fMemVirtual));
// }
