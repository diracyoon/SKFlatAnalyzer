#include "Vcb_Modelling_Patch.h"

//////////

Vcb_Modelling_Patch::Vcb_Modelling_Patch()
{
  weight_pdf_as_down = 1;
  weight_pdf_as_up = 1;
} // Vcb_Modelling_Patch::Vcb_Modelling_Patch()

//////////

Vcb_Modelling_Patch::~Vcb_Modelling_Patch()
{

} // Vcb_Modelling_Patch::~Vcb_Modelling_Patch()

//////////

void Vcb_Modelling_Patch::initializeAnalyzer()
{
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("gen_phi", 1);
  fChain->SetBranchStatus("gen_eta", 1);
  fChain->SetBranchStatus("gen_pt", 1);
  fChain->SetBranchStatus("gen_mass", 1);
  fChain->SetBranchStatus("gen_charge", 1);
  fChain->SetBranchStatus("gen_mother_index", 1);
  fChain->SetBranchStatus("gen_status", 1);
  fChain->SetBranchStatus("gen_PID", 1);
  fChain->SetBranchStatus("gen_isPrompt", 1);
  fChain->SetBranchStatus("gen_isPromptFinalState", 1);
  fChain->SetBranchStatus("gen_isTauDecayProduct", 1);
  fChain->SetBranchStatus("gen_isPromptTauDecayProduct", 1);
  fChain->SetBranchStatus("gen_isDirectPromptTauDecayProductFinalState", 1);
  fChain->SetBranchStatus("gen_isHardProcess", 1);
  fChain->SetBranchStatus("gen_isLastCopy", 1);
  fChain->SetBranchStatus("gen_isLastCopyBeforeFSR", 1);
  fChain->SetBranchStatus("gen_isPromptDecayed", 1);
  fChain->SetBranchStatus("gen_isDecayedLeptonHadron", 1);
  fChain->SetBranchStatus("gen_fromHardProcessBeforeFSR", 1);
  fChain->SetBranchStatus("gen_fromHardProcessDecayed", 1);
  fChain->SetBranchStatus("gen_fromHardProcessFinalState", 1);
  fChain->SetBranchStatus("gen_isMostlyLikePythia6Status3", 1);
  fChain->SetBranchStatus("gen_weight", 1);
  fChain->SetBranchStatus("genWeight_Q", 1);
  fChain->SetBranchStatus("genWeight_X1", 1);
  fChain->SetBranchStatus("genWeight_X2", 1);
  fChain->SetBranchStatus("genWeight_id1", 1);
  fChain->SetBranchStatus("genWeight_id2", 1);
  fChain->SetBranchStatus("genWeight_alphaQCD", 1);
  fChain->SetBranchStatus("genWeight_alphaQED", 1);
  fChain->SetBranchStatus("weight_Scale", 1);
  fChain->SetBranchStatus("weight_PSSyst", 1);

  if (MCSample.Contains("TT") || MCSample.Contains("SingleTop_tW"))
  {
    if (MCSample.Contains("TTbb"))
    {
      central_pdf = "NNPDF31_nnlo_as_0118_nf_4";
      central_new_pdf = "NNPDF31_nlo_as_0118_nf_4";
    }
    else
    {
      central_pdf = "NNPDF31_nnlo_hessian_pdfas";
      central_new_pdf = "NNPDF31_nlo_hessian_pdfas";
    }
  }
  else if (MCSample.Contains("SingleTop_sch_Lep") || MCSample.Contains("SingleTop_tch"))
  {
    central_pdf = "NNPDF31_nnlo_as_0118_nf_4_mc_hessian";
    central_new_pdf = "NNPDF31_nlo_as_0118_nf_4";
  }
  else if (MCSample.Contains("DYJets_MG") || MCSample.Contains("WJets_HT") ||
           MCSample.Contains("QCD_bEnriched_HT") ||
           MCSample.Contains("ttWTo") || MCSample.Contains("ttZTo") || MCSample.Contains("ttHTo"))
  {
    central_pdf = "NNPDF31_nnlo_as_0118_mc_hessian_pdfas";
    central_new_pdf = "NNPDF31_nlo_as_0118_hessian";
  }
  else if (MCSample.Contains("pythia"))
  {
    central_pdf = "NNPDF31_nnlo_as_0118";
    central_new_pdf = "NNPDF31_nlo_as_0118";
  }
  else
  {
    cerr << "LHAPDFHandler" << endl;
    cerr << "Unknown MC Sample: " << MCSample << endl;
  }

  LHAPDFHandler LHAPDFHandler_Prod;
  LHAPDFHandler_Prod.CentralPDFName = central_pdf;
  LHAPDFHandler_Prod.ErrorSetMember_Start = 1;
  LHAPDFHandler_Prod.ErrorSetMember_End = 100;
  if (central_pdf == "NNPDF31_nnlo_hessian_pdfas" || central_pdf == "NNPDF31_nnlo_as_0118_mc_hessian_pdfas")
  {
    LHAPDFHandler_Prod.AlphaSMember_Down = 101;
    LHAPDFHandler_Prod.AlphaSMember_Up = 102;
  }
  LHAPDFHandler_Prod.init();

  LHAPDFHandler LHAPDFHandler_New;
  LHAPDFHandler_New.CentralPDFName = central_new_pdf;
  LHAPDFHandler_New.init();

  pdfReweight->SetProdPDF(LHAPDFHandler_Prod.PDFCentral);
  pdfReweight->SetNewPDF(LHAPDFHandler_New.PDFCentral);
  pdfReweight->SetNewPDFErrorSet(LHAPDFHandler_Prod.PDFErrorSet);
  if (central_pdf == "NNPDF31_nnlo_hessian_pdfas" || central_pdf == "NNPDF31_nnlo_as_0118_mc_hessian_pdfas")
     pdfReweight->SetNewPDFAlphaS(LHAPDFHandler_Prod.PDFAlphaSDown, LHAPDFHandler_Prod.PDFAlphaSUp);
 

  return;
} // Vcb_Modelling_Patch::initializeAnalyzer()

//////////

void Vcb_Modelling_Patch::executeEvent()
{
  param.Clear();
  param.syst_ = AnalyzerParameter::Central;

  executeEventFromParameter(param);

  return;
} // Vcb_Modelling_Patch::executeEvent()

//////////

void Vcb_Modelling_Patch::executeEventFromParameter(AnalyzerParameter param)
{
  Event ev = GetEvent();

  weight = 1;

  weight_lumi = ev.GetTriggerLumi("Full");
  weight *= weight_lumi;

  weight_mc = MCweight();
  weight *= weight_mc;

  // PDF
  weight_pdf_alternative = GetPDFReweight();
  for (int i = 0; i < 100; i++)
    weight_pdf_error_set[i] = GetPDFReweight(i);

  if (central_pdf == "NNPDF31_nnlo_hessian_pdfas" || central_pdf == "NNPDF31_nnlo_as_0118_mc_hessian_pdfas")
  {
    weight_pdf_as_down = GetPDFReweight("As_Down");
    weight_pdf_as_up = GetPDFReweight("As_Up");
  }

  // Top pT reweight
  vector<Gen> vec_gen = GetGens();
  weight_top_pt = mcCorr->GetTopPtReweight(vec_gen);

  // Scale Variation
  weight_scale_variation_1 = GetScaleVariation(1);
  weight_scale_variation_2 = GetScaleVariation(2);
  weight_scale_variation_3 = GetScaleVariation(3);
  weight_scale_variation_4 = GetScaleVariation(4);
  weight_scale_variation_6 = GetScaleVariation(6);
  weight_scale_variation_8 = GetScaleVariation(8);

  // PS
  Get_Reweight_PS(weight_ps);

   if (IsNaN())
     return;

if (Cut_PDF_Weight())
return;

     //cout << genWeight_Q << " " << genWeight_X1 << " " << genWeight_X2 << " " << weight_pdf_alternative << endl;

  // cout << "test " << weight_pdf_alternative << " " << weight_pdf_as_down << " " << weight_pdf_as_up << " " << endl;
  // for (int i = 0; i < 100; i++)
  //   cout << weight_pdf_error_set[i] << " " << endl;

  /////////////////////
  /* Fill histograms */
  /////////////////////

  // Balseline
  FillHist("Baseline", 0.5, weight, 1, 0, 1);

  // PDF
  FillHist("PDF_Alternative", 0.5, weight * weight_pdf_alternative, 1, 0, 1);
  for (int i = 0; i < 100; i++)
    FillHist(Form("PDF_Error_Set_%d", i + 1), 0.5, weight * weight_pdf_error_set[i], 1, 0, 1);
  FillHist("PDF_As_Down", 0.5, weight * weight_pdf_as_down, 1, 0, 1);
  FillHist("PDF_As_Up", 0.5, weight * weight_pdf_as_up, 1, 0, 1);

  // Top Pt reweight
  FillHist("Top_Pt_Reweight", 0.5, weight * weight_top_pt, 1, 0, 1);

  // Scale Variation
  FillHist("Scale_Variation_1", 0.5, weight * weight_scale_variation_1, 1, 0, 1);
  FillHist("Scale_Variation_2", 0.5, weight * weight_scale_variation_2, 1, 0, 1);
  FillHist("Scale_Variation_3", 0.5, weight * weight_scale_variation_3, 1, 0, 1);
  FillHist("Scale_Variation_4", 0.5, weight * weight_scale_variation_4, 1, 0, 1);
  FillHist("Scale_Variation_6", 0.5, weight * weight_scale_variation_6, 1, 0, 1);
  FillHist("Scale_Variation_8", 0.5, weight * weight_scale_variation_8, 1, 0, 1);

  // PS
  FillHist("PS_0", 0.5, weight * weight_ps[0], 1, 0, 1);
  FillHist("PS_1", 0.5, weight * weight_ps[1], 1, 0, 1);
  FillHist("PS_2", 0.5, weight * weight_ps[2], 1, 0, 1);
  FillHist("PS_3", 0.5, weight * weight_ps[3], 1, 0, 1);

  return;
} // Vcb_Modelling_Patch::executeEventFromParameter(AnalyzerParameter param)

//////////

bool Vcb_Modelling_Patch::Cut_PDF_Weight()
{
  if(5<TMath::Abs(weight_pdf_alternative-1) || 5<TMath::Abs(weight_pdf_as_down-1) || 5<TMath::Abs(weight_pdf_as_up-1))
  return true;
 
  for (int i = 0; i < 100; i++)
  {
    if (5<TMath::Abs(weight_pdf_error_set[i]-1))
      return true;
  }

  return false;
}// bool Vcb_Modeeling_Patch::Cut_PDF_Weight()

//////////

bool Vcb_Modelling_Patch::IsNaN()
{
  if (TMath::IsNaN(weight_pdf_alternative) || TMath::IsNaN(weight_pdf_as_down) || TMath::IsNaN(weight_pdf_as_up) ||
      TMath::IsNaN(weight_top_pt) ||
      TMath::IsNaN(weight_scale_variation_1) || TMath::IsNaN(weight_scale_variation_2) ||
      TMath::IsNaN(weight_scale_variation_3) || TMath::IsNaN(weight_scale_variation_4) ||
      TMath::IsNaN(weight_scale_variation_6) || TMath::IsNaN(weight_scale_variation_8) ||
      TMath::IsNaN(weight_ps[0]) || TMath::IsNaN(weight_ps[1]) || TMath::IsNaN(weight_ps[2]) || TMath::IsNaN(weight_ps[3]))
    return true;

  for (int i = 0; i < 100; i++)
  {
    if (TMath::IsNaN(weight_pdf_error_set[i]))
      return true;
  }

  if (!TMath::Finite(weight_pdf_alternative) || !TMath::Finite(weight_pdf_as_down) || !TMath::Finite(weight_pdf_as_up) ||
      !TMath::Finite(weight_top_pt) ||
      !TMath::Finite(weight_scale_variation_1) || !TMath::Finite(weight_scale_variation_2) ||
      !TMath::Finite(weight_scale_variation_3) || !TMath::Finite(weight_scale_variation_4) ||
      !TMath::Finite(weight_scale_variation_6) || !TMath::Finite(weight_scale_variation_8) ||
      !TMath::Finite(weight_ps[0]) || !TMath::Finite(weight_ps[1]) || !TMath::Finite(weight_ps[2]) || !TMath::Finite(weight_ps[3]))
    return true;

  for (int i = 0; i < 100; i++)
  {
    if (!TMath::Finite(weight_pdf_error_set[i]))
      return true;
  }

  return false;
} // bool Vcb_Modelling_Patch::IsNaN()

//////////
