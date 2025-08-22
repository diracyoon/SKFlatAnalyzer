#include "Vcb_Cal_TF.h"

//////////

Vcb_Cal_TF::Vcb_Cal_TF()
{
} // Vcb_Cal_TF::Vcb_Cal_TF()

//////////

Vcb_Cal_TF::~Vcb_Cal_TF()
{

  for (unsigned int i = 0; i < vec_channel.size(); i++)
  {
    for (unsigned int j = 0; j < vec_syst_type.size(); j++)
    {
      param.syst_ = vec_syst_type.at(j);

      dir_syst[i][j]->cd();
      map_result_tree[vec_channel[i] + param.GetSystType()]->Write();
    }
  }

  outfile->Close();

} // Vcb_Cal_TF::~Vcb_Cal_TF()

//////////

void Vcb_Cal_TF::initializeAnalyzer()
{
  run_debug = HasFlag("RunDebug");
  cout << "[Vcb_Cal_TF::initializeAnalyzer] RunDebug = " << run_debug << endl;

  run_syst = HasFlag("RunSyst");
  cout << "[Vcb_Cal_TF::initializeAnalyzer] RunSyst = " << run_syst << endl;

  run_no_jes_breakdown = HasFlag("RunNoJESBreakdown");
  cout << "[Vcb_Cal_TF::initializeAnalyzer] RunNoJESBreakdown" << run_no_jes_breakdown << endl;

  if (DataEra == "2016preVFP")
    era_index = 0;
  else if (DataEra == "2016postVFP")
    era_index = 1;
  else if (DataEra == "2017")
    era_index = 2;
  else if (DataEra == "2018")
    era_index = 3;

  // set sigle lepton trigger
  if (DataYear == 2016)
  {
    vec_mu_trig.push_back("HLT_IsoMu24_v");
    mu_trig = "IsoMu24";
    mu_trig_safe_pt_cut = 26.;

    vec_el_trig.push_back("HLT_Ele27_WPTight_Gsf_v");
    el_trig = "Ele27";
    el_trig_safe_pt_cut = 30.;
  } // if (DataYear == 2016)
  else if (DataYear == 2017)
  {
    vec_mu_trig.push_back("HLT_IsoMu27_v");
    mu_trig = "IsoMu27";
    mu_trig_safe_pt_cut = 30.;

    // vec_el_trig.push_back("HLT_Ele35_WPTight_Gsf_v");
    // sl_trig = "Ele35";
    // sl_trig_safe_pt_cut = 37.;

    vec_el_trig.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG_");
    el_trig = "Ele32";
    el_trig_safe_pt_cut = 35.;
  } // else if (DataYear == 2017)
  else if (DataYear == 2018)
  {
    vec_mu_trig.push_back("HLT_IsoMu24_v");
    mu_trig = "IsoMu24";
    mu_trig_safe_pt_cut = 26.;

    vec_el_trig.push_back("HLT_Ele32_WPTight_Gsf_v");
    el_trig = "Ele32";
    el_trig_safe_pt_cut = 35.;
  } // else if (DataYear == 2018)
  else
    std::runtime_error("No trigger configuration for year");

  for (auto &trigger_name : vec_mu_trig)
    cout << "[Vcb_Cal_TF::initializeAnalyzer] Single Muon Trigger Name = " << trigger_name << endl;
  cout << "[Vcb_Cal_TF::initializeAnalyzer] Single Muon Trigger Safe Pt Cut = " << mu_trig_safe_pt_cut << endl;

  for (auto &trigger_name : vec_el_trig)
    cout << "[Vcb_Cal_TF::initializeAnalyzer] Single Electron Trigger Name = " << trigger_name << endl;
  cout << "[Vcb_Cal_TF::initializeAnalyzer] Single Electron Trigger Safe Pt Cut = " << el_trig_safe_pt_cut << endl;

  // Jet Tagging Parameters
  if (run_debug)
  {
    vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::comb));
    vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet_C, JetTagging::Medium, JetTagging::incl, JetTagging::wcharm));
  }
  else
  {
    vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::iterativefit, JetTagging::iterativefit));
    vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet_C, JetTagging::Medium, JetTagging::iterativefit, JetTagging::iterativefit));
  }
  mcCorr->SetJetTaggingParameters(vec_jet_tagging_para);

  // PDF syst
  if (run_syst)
  {
    LHAPDFHandler LHAPDFHandler_Prod;
    // LHAPDFHandler_Prod.CentralPDFName = "NNPDF31_nnlo_hessian_pdfas";
    LHAPDFHandler_Prod.CentralPDFName = "NNPDF31_nnlo_as_0118_mc_hessian_pdfas";
    LHAPDFHandler_Prod.init();

    LHAPDFHandler LHAPDFHandler_New;
    LHAPDFHandler_New.CentralPDFName = "NNPDF31_nlo_hessian_pdfas";
    LHAPDFHandler_New.ErrorSetMember_Start = 1;
    LHAPDFHandler_New.ErrorSetMember_End = 100;
    LHAPDFHandler_New.AlphaSMember_Down = 101;
    LHAPDFHandler_New.AlphaSMember_Up = 102;
    LHAPDFHandler_New.init();

    pdfReweight->SetProdPDF(LHAPDFHandler_Prod.PDFCentral);
    pdfReweight->SetNewPDF(LHAPDFHandler_New.PDFCentral);
    pdfReweight->SetNewPDFErrorSet(LHAPDFHandler_New.PDFErrorSet);
    pdfReweight->SetNewPDFAlphaS(LHAPDFHandler_New.PDFAlphaSDown, LHAPDFHandler_New.PDFAlphaSUp);
  }

  vec_channel = {"Mu", "El"};

  if (!IsDATA && run_syst)
  {
    vec_syst_type = {AnalyzerParameter::Central,
                     AnalyzerParameter::UnclusteredEnergyDown, AnalyzerParameter::UnclusteredEnergyUp,
                     AnalyzerParameter::JetResDown, AnalyzerParameter::JetResUp,
                     AnalyzerParameter::JetEnDown, AnalyzerParameter::JetEnUp};

    if (!run_no_jes_breakdown)
    {
      // set JES breakdown
      string year = to_string(GetYear());
      JECSources = {"Absolute", "BBEC1", "EC2", "FlavorQCD", "HF", "RelativeBal"};
      JECSources_byYear = {"Absolute", "BBEC1", "EC2", "HF", "RelativeSample"};
      for (auto jec_source : JECSources_byYear)
        JECSources.push_back(jec_source + "_" + year);
      for (auto jec_source : JECSources)
        SetupJECUncertainty(jec_source, "AK4PFchs");

      // vec_syst_type.push_back(AnalyzerParameter::JetEnDown);
      // vec_syst_type.push_back(AnalyzerParameter::JetEnUp);
      vec_syst_type.push_back(AnalyzerParameter::JetEnAbsoluteDown);
      vec_syst_type.push_back(AnalyzerParameter::JetEnAbsoluteUp);
      vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC1Down);
      vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC1Up);
      vec_syst_type.push_back(AnalyzerParameter::JetEnEC2Down);
      vec_syst_type.push_back(AnalyzerParameter::JetEnEC2Up);
      vec_syst_type.push_back(AnalyzerParameter::JetEnFlavorQCDDown);
      vec_syst_type.push_back(AnalyzerParameter::JetEnFlavorQCDUp);
      vec_syst_type.push_back(AnalyzerParameter::JetEnHFDown);
      vec_syst_type.push_back(AnalyzerParameter::JetEnHFUp);
      vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeBalDown);
      vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeBalUp);

      if (to_string(GetYear()) == "2016")
      {
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2016Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2016Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12016Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12016Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22016Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22016Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2016Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2016Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2016Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2016Up);
      } // if (to_string(GetYear()) == "2016")
      else if (to_string(GetYear()) == "2017")
      {
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2017Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2017Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12017Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12017Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22017Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22017Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2017Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2017Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2017Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2017Up);
      } // else if (to_string(GetYear()) == "2017")
      else if (to_string(GetYear()) == "2018")
      {
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2018Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnAbsolute2018Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12018Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnBBEC12018Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22018Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnEC22018Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2018Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnHF2018Up);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2018Down);
        vec_syst_type.push_back(AnalyzerParameter::JetEnRelativeSample2018Up);
      } // else if (to_string(GetYear()) == "2018")
    } // if(!run_no_jes_breakdown)

    // if (run_el_ch)
    // {
    //   vec_syst_type.push_back(AnalyzerParameter::ElectronEnDown);
    //   vec_syst_type.push_back(AnalyzerParameter::ElectronEnUp);
    //   vec_syst_type.push_back(AnalyzerParameter::ElectronResDown);
    //   vec_syst_type.push_back(AnalyzerParameter::ElectronResUp);
    // }
  }
  else
    vec_syst_type = {AnalyzerParameter::Central};

  // to make output dir
  dir_channel = new TDirectory *[vec_channel.size()];
  dir_syst = new TDirectory **[vec_channel.size()];
  for (unsigned int i = 0; i < vec_channel.size(); i++)
  {
    dir_channel[i] = outfile->mkdir(vec_channel[i]);
    dir_syst[i] = new TDirectory *[vec_syst_type.size()];

    for (unsigned int j = 0; j < vec_syst_type.size(); j++)
    {
      param.syst_ = vec_syst_type.at(j);

      dir_syst[i][j] = dir_channel[i]->mkdir(param.GetSystType());
    } // loop over syst

  } // loop over channel

  Set_Result_Tree();

  return;
} // void Vcb_Cal_TF::initializeAnalyzer()

//////////

void Vcb_Cal_TF::executeEvent()
{
  // Apply Jet Veto Map
  if (IsEventJetMapVetoed())
    return;

  // init and clear
  vec_muon.clear();
  vec_electron.clear();
  vec_jet.clear();

  vec_muon = GetAllMuons();
  vec_electron = GetAllElectrons();
  vec_jet = GetAllJets();

  for (unsigned int i = 0; i < vec_channel.size(); i++)
  {
    run_mu_ch = false;
    run_el_ch = false;

    if (i == 0)
      run_mu_ch = true;
    else if (i == 1)
      run_el_ch = true;

    channel_name = vec_channel[i];

    for (unsigned int j = 0; j < vec_syst_type.size(); j++)
    {
      // setup setting
      param.Clear();

      param.Muon_Tight_ID = "POGTightWithLooseIso";
      // param.Muon_Loose_ID = "POGLooseWithLooseIso";
      param.Muon_Jet_Cleaning_ID = "POGTightWithTightIso";

      param.Electron_Tight_ID = "passMVAID_iso_WP90";
      // param.Electron_Loose_ID = "passMVAID_iso_WP90";
      param.Electron_Jet_Cleaning_ID = "passMVAID_iso_WP80";

      param.Muon_ID_SF_Key = "NUM_TightID_DEN_TrackerMuons";
      param.Muon_ISO_SF_Key = "NUM_TightRelIso_DEN_TightIDandIPCut";

      param.Jet_ID = "tight";
      // param.Jet_ID = "tightLepVeto";
      param.PUJet_Veto_ID = "LoosePileupJetVeto";

      param.syst_ = vec_syst_type.at(j);

      param.Name = channel_name + "/" + param.GetSystType();

      executeEventFromParameter(param);
    } // loop over syst
  } // loop over channel

  return;
} // void Vcb_Cal_TF::executeEvent()

//////////

void Vcb_Cal_TF::executeEventFromParameter(AnalyzerParameter param)
{
  // cout << param.Name << endl;

  Clear();

  Event ev = GetEvent();

  if (!IsDATA)
  {
    vector<Gen> vec_gen = GetGens();

    decay_mode = Get_W_Decay_Mode(vec_gen);

    // lumi
    weight_lumi = ev.GetTriggerLumi("Full");

    weight *= weight_lumi;

    // MCweight +1 or -1
    weight_mc = MCweight();

    weight *= weight_mc;

    // PDF
    if (param.syst_ == AnalyzerParameter::Central && run_syst)
    {
      weight_pdf_alternative = GetPDFReweight();
      for (int i = 0; i < 100; i++)
        weight_pdf_error_set[i] = GetPDFReweight(i);
      weight_pdf_as_up = GetPDFReweight("As_Up");
      weight_pdf_as_down = GetPDFReweight("As_Down");
    }

    // pileup reweight
    weight_pileup = mcCorr->GetPileUpWeight_Json(nPileUp, 0);
    if (param.syst_ == AnalyzerParameter::Central && run_syst)
    {
      weight_pileup_down = mcCorr->GetPileUpWeight_Json(nPileUp, -1);
      weight_pileup_up = mcCorr->GetPileUpWeight_Json(nPileUp, +1);
    }

    weight *= weight_pileup;

    // L1 prefire
    weight_prefire = GetPrefireWeight(0);
    if (param.syst_ == AnalyzerParameter::Central && run_syst)
    {
      weight_prefire_down = GetPrefireWeight(-1);
      weight_prefire_up = GetPrefireWeight(+1);
    }

    weight *= weight_prefire;

    // Top Pt reweight
    weight_top_pt = mcCorr->GetTopPtReweight(vec_gen);

    weight *= weight_top_pt;

    // Scale Variation
    if (param.syst_ == AnalyzerParameter::Central && run_syst)
    {
      weight_scale_variation_1 = GetScaleVariation(1);
      weight_scale_variation_2 = GetScaleVariation(2);
      weight_scale_variation_3 = GetScaleVariation(3);
      weight_scale_variation_4 = GetScaleVariation(4);
      weight_scale_variation_6 = GetScaleVariation(6);
      weight_scale_variation_8 = GetScaleVariation(8);

      // PS Reweight
      Get_Reweight_PS(weight_ps);
    }

    // if(!TMath::Finite(lumi_weight)) cout << lumi_weight << endl;
    // if(!TMath::Finite(mc_weight)) cout << mc_weight << endl;
    // if(!TMath::Finite(pileup_weight)) cout << pileup_weight << endl;
    // if(!TMath::Finite(prefire_weight)) cout << prefire_weight << endl;
  } // if (!IsDATA)

  // met filter
  if (!PassMETFilter())
    return;

  // set objects
  vec_this_muon = vec_muon;
  vec_this_electron = vec_electron;
  vec_this_jet = vec_jet;

  if (param.syst_ == AnalyzerParameter::UnclusteredEnergyDown)
    Met_Syst_Unclustered(met, -1);
  else if (param.syst_ == AnalyzerParameter::UnclusteredEnergyUp)
    Met_Syst_Unclustered(met, 1);
  else
    met = ev.GetMETVector("PUPPI");

  // xy correction
  pair<double, double> met_corr = xy_met_correction.METXYCorr_Met_MetPhi(met.Pt(), met.Phi(), run, to_string(DataYear), !IsDATA, nPV, true, true);

  // Particle met = ev.GetMETVector("PF");
  // pair<double, double> met_corr = xy_met_correction.METXYCorr_Met_MetPhi(met.Pt(), met.Phi(), run, to_string(DataYear), !IsDATA, nPV, true, false);
  // cout << met.Pt() << " " << met_corr.first << " " << met.Phi() << " " << met_corr.second << endl;

  met.SetPtEtaPhiE(met_corr.first, 0, met_corr.second, met_corr.first);

  //////////////////////
  /* syst for objects */
  //////////////////////

  // if(param.syst_ == AnalyzerParameter::MuonEnUp) vec_this_muon = ScaleMuons(vec_this_muon, +1);
  // if(param.syst_ == AnalyzerParameter::MuonEnDown) vec_this_muon = ScaleMuons(vec_this_muon, -1);
  if (param.syst_ == AnalyzerParameter::ElectronEnDown)
  {
    vec_this_electron = ScaleElectrons(vec_this_electron, -1);
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::ElectronEnUp)
  {
    vec_this_electron = ScaleElectrons(vec_this_electron, +1);
    met = Rebalance_Met();
  }

  if (param.syst_ == AnalyzerParameter::ElectronResDown)
  {
    vec_this_electron = SmearElectrons(vec_this_electron, -1);
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::ElectronResUp)
  {
    vec_this_electron = SmearElectrons(vec_this_electron, +1);
    met = Rebalance_Met();
  }

  if (param.syst_ == AnalyzerParameter::JetResDown)
  {
    vec_this_jet = SmearJets(vec_this_jet, -1);
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetResUp)
  {
    vec_this_jet = SmearJets(vec_this_jet, +1);
    met = Rebalance_Met();
  }

  // JES Total
  else if (param.syst_ == AnalyzerParameter::JetEnDown)
  {
    vec_this_jet = ScaleJets(vec_this_jet, -1);
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnUp)
  {
    vec_this_jet = ScaleJets(vec_this_jet, +1);
    met = Rebalance_Met();
  }

  // JES breakdown
  else if (param.syst_ == AnalyzerParameter::JetEnAbsoluteDown)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "Absolute");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsoluteUp)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "Absolute");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC1Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "BBEC1");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC1Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "BBEC1");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC2Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "EC2");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC2Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "EC2");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnFlavorQCDDown)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "FlavorQCD");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnFlavorQCDUp)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "FlavorQCD");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHFDown)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "HF");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHFUp)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "HF");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeBalDown)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "RelativeBal");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeBalUp)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "RelativeBal");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2016Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "Absolute_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2016Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "Absolute_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12016Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "BBEC1_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12016Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "BBEC1_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22016Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "EC2_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22016Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "EC2_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2016Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "HF_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2016Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "HF_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "RelativeSample_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "RelativeSample_2016");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2017Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "Absolute_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2017Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "Absolute_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12017Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "BBEC1_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12017Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "BBEC1_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22017Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "EC2_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22017Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "EC2_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2017Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "HF_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2017Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "HF_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "RelativeSample_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "RelativeSample_2017");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2018Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "Absolute_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2018Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "Absolute_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12018Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "BBEC1_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnBBEC12018Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "BBEC1_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22018Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "EC2_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnEC22018Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "EC2_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2018Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "HF_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnHF2018Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "HF_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Down)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, -1, "RelativeSample_2018");
    met = Rebalance_Met();
  }
  else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Up)
  {
    vec_this_jet = ScaleJetsIndividualSource(vec_this_jet, +1, "RelativeSample_2018");
    met = Rebalance_Met();
  }

  /////////////////
  /*Setup Objects*/
  /////////////////

  // for lepton selection
  vector<Muon> vec_sel_muon = SelectMuons(vec_this_muon, param.Muon_Tight_ID, mu_trig_safe_pt_cut, MUON_ETA);
  vector<Electron> vec_sel_electron = SelectElectrons(vec_this_electron, param.Electron_Tight_ID, el_trig_safe_pt_cut, ELECTRON_ETA);

  // for lepton veto
  // vector<Muon> vec_muon_veto = SelectMuons(vec_this_muon, param.Muon_Loose_ID, MUON_PT_VETO, MUON_ETA);
  // vector<Electron> vec_electron_veto = SelectElectrons(vec_this_electron, param.Electron_Loose_ID, ELECTRON_PT_VETO, ELECTRON_ETA);

  // for lepton jet cleaning
  vector<Muon> vec_muon_jet_cleaning = SelectMuons(vec_this_muon, param.Muon_Jet_Cleaning_ID, mu_trig_safe_pt_cut, MUON_ETA);
  vector<Electron> vec_electron_jet_cleaning = SelectElectrons(vec_this_electron, param.Electron_Jet_Cleaning_ID, el_trig_safe_pt_cut, ELECTRON_ETA);

  float jet_eta_cut = 999;
  if (DataYear == 2016)
    jet_eta_cut = JET_ETA_2016;
  else if (DataYear == 2017 || DataYear == 2018)
    jet_eta_cut = JET_ETA;

  // Jet selection
  vec_sel_jet = SelectJets(vec_this_jet, param.Jet_ID, JET_PT, jet_eta_cut);
  vec_sel_jet = SelectJets(vec_sel_jet, param.PUJet_Veto_ID, JET_PT, jet_eta_cut);
  vec_sel_jet = JetsVetoLeptonInside(vec_sel_jet, vec_electron_jet_cleaning, vec_muon_jet_cleaning, DR_LEPTON_VETO);
  n_sel_jet = vec_sel_jet.size();

  // HEM veto
  weight_hem_veto = Weight_HEM_Veto(vec_sel_jet);
  weight *= weight_hem_veto;

  // sort jet as pt ordering
  sort(vec_sel_jet.begin(), vec_sel_jet.end(), PtComparing);

  // single lepton trigger
  if (run_mu_ch)
  {
    if (!ev.PassTrigger(vec_mu_trig))
      return;
  }
  else if (run_el_ch)
  {
    if (!ev.PassTrigger(vec_el_trig) || !HLT_SE_Filter_2017(vec_sel_electron))
      return;
  }

  if (!IsDATA)
  {
    if (run_mu_ch)
    {
      // SF for muon trigger effi
      weight_sl_trig = mcCorr->MuonTrigger_SF("POGTight", mu_trig, vec_sel_muon, 0);
      if (run_syst)
      {
        weight_sl_trig_down = mcCorr->MuonTrigger_SF("POGTight", mu_trig, vec_sel_muon, -1);
        weight_sl_trig_up = mcCorr->MuonTrigger_SF("POGTight", mu_trig, vec_sel_muon, +1);
      }
    }
    else if (run_el_ch)
    {
      weight_sl_trig = mcCorr->ElectronTrigger_SF(param.Electron_Tight_ID, el_trig, vec_sel_electron, 0);
      if (run_syst)
      {
        weight_sl_trig_down = mcCorr->ElectronTrigger_SF(param.Electron_Tight_ID, el_trig, vec_sel_electron, -1);
        weight_sl_trig_up = mcCorr->ElectronTrigger_SF(param.Electron_Tight_ID, el_trig, vec_sel_electron, +1);
      }
    }
    weight *= weight_sl_trig;
  }

  // cut on lepton
  // veto additional lepton
  if (run_mu_ch)
  {
    if (vec_sel_muon.size() != 1 || vec_sel_electron.size() != 0)
      return;

    muon = vec_sel_muon.at(0);
    lepton = muon;
  }
  else if (run_el_ch)
  {
    if (vec_sel_electron.size() != 1 || vec_sel_muon.size() != 0)
      return;

    electron = vec_sel_electron.at(0);
    lepton = electron;
  }

  lepton_pt = lepton.Pt();
  if (run_el_ch)
    lepton_pt_uncorr = electron.UncorrPt();
  lepton_eta = lepton.Eta();
  lepton_phi = lepton.Phi();

  lepton_rel_iso = lepton.RelIso();

  if (run_el_ch)
  {
    electron_id_bit = electron.GetIDBit();

    // unsigned int mva_iso_wp80_bit_mask = 1 << 4;
    // unsigned int mva_iso_wp90_bit_mask = 1 << 5;
    // cout << electron_id_bit << ", " << ((electron_id_bit & mva_iso_wp80_bit_mask) == mva_iso_wp80_bit_mask) << ", " << ((electron_id_bit & mva_iso_wp90_bit_mask) == mva_iso_wp90_bit_mask) << endl;
  }

  if (run_mu_ch)
  {
    if (lepton_pt <= mu_trig_safe_pt_cut)
      return;
  }
  else if (run_el_ch)
  {
    if (lepton_pt <= el_trig_safe_pt_cut)
      return;
  }

  if (!IsDATA)
  {
    if (run_mu_ch)
    {
      // SF for muon id
      weight_mu_id = mcCorr->MuonID_SF_Json(param.Muon_ID_SF_Key, muon.Eta(), muon.MiniAODPt(), 0);
      if (run_syst)
      {
        weight_mu_id_down = mcCorr->MuonID_SF_Json(param.Muon_ID_SF_Key, muon.Eta(), muon.MiniAODPt(), -1);
        weight_mu_id_up = mcCorr->MuonID_SF_Json(param.Muon_ID_SF_Key, muon.Eta(), muon.MiniAODPt(), +1);
      }
      weight *= weight_mu_id;

      // SF for muon iso
      weight_mu_iso = mcCorr->MuonISO_SF_Json(param.Muon_ISO_SF_Key, muon.Eta(), muon.MiniAODPt(), 0);
      if (run_syst)
      {
        weight_mu_iso_down = mcCorr->MuonISO_SF_Json(param.Muon_ISO_SF_Key, muon.Eta(), muon.MiniAODPt(), -1);
        weight_mu_iso_up = mcCorr->MuonISO_SF_Json(param.Muon_ISO_SF_Key, muon.Eta(), muon.MiniAODPt(), +1);
      }
      weight *= weight_mu_iso;
      // if(run_debug) cout << "M ISO SF = " << muon.Eta() << "\t" << muon.MiniAODPt() << "\t" << sf_mu_iso_effi << endl;
    } // if (run_mu_ch)
    else if (run_el_ch)
    {
      // SF for electron id
      weight_el_id = mcCorr->ElectronID_SF(param.Electron_Tight_ID, electron.scEta(), electron.UncorrPt(), 0);
      if (run_syst)
      {
        weight_el_id_down = mcCorr->ElectronID_SF(param.Electron_Tight_ID, electron.scEta(), electron.UncorrPt(), -1);
        weight_el_id_up = mcCorr->ElectronID_SF(param.Electron_Tight_ID, electron.scEta(), electron.UncorrPt(), +1);
      }
      weight *= weight_el_id;

      // SF for electron Reco eff
      weight_el_reco = mcCorr->ElectronReco_SF(electron.scEta(), electron.UncorrPt(), 0);
      if (run_syst)
      {
        weight_el_reco_down = mcCorr->ElectronReco_SF(electron.scEta(), electron.UncorrPt(), -1);
        weight_el_reco_up = mcCorr->ElectronReco_SF(electron.scEta(), electron.UncorrPt(), +1);
      }
      weight *= weight_el_reco;
    } // else if (run_el_ch)
  } //  if (!IsDATA)

  // cut on jet
  // // at least three jets
  if (n_sel_jet < 4)
    return;

  if (!IsDATA)
  {
    // SF for PUJet Veto
    weight_pujet_veto = mcCorr->PileupJetVeto_Reweight(vec_sel_jet, param.PUJet_Veto_ID, 0);
    if (run_syst)
    {
      weight_pujet_veto_down = mcCorr->PileupJetVeto_Reweight(vec_sel_jet, param.PUJet_Veto_ID, -1);
      weight_pujet_veto_up = mcCorr->PileupJetVeto_Reweight(vec_sel_jet, param.PUJet_Veto_ID, +1);
    }

    weight *= weight_pujet_veto;
  }

  // n of btag
  n_b_jet = 0;
  for (auto &jet : vec_sel_jet)
  {
    float tagging_score = jet.GetTaggerResult(JetTagging::DeepJet);
    if (mcCorr->GetJetTaggingCutValue(JetTagging::DeepJet, JetTagging::Medium) < tagging_score)
    {
      n_b_jet++;
      vec_btag.push_back(true);
    }
    else
      vec_btag.push_back(false);
  }

  // n of ctag
  n_c_jet = 0;
  for (auto &jet : vec_sel_jet)
  {
    float cvsb = jet.GetTaggerResult(JetTagging::DeepJet_CvsB);
    float cvsl = jet.GetTaggerResult(JetTagging::DeepJet_CvsL);

    float cvsl_wp = -1;
    float cvsb_wp = -1;

    if (DataEra == "2016preVFP")
    {
      cvsl_wp = cvsl_2016a_m;
      cvsb_wp = cvsb_2016a_m;
    }
    else if (DataEra == "2016postVFP")
    {
      cvsl_wp = cvsl_2016b_m;
      cvsb_wp = cvsb_2016b_m;
    }
    else if (DataEra == "2017")
    {
      cvsl_wp = cvsl_2017_m;
      cvsb_wp = cvsb_2017_m;
    }
    else if (DataEra == "2018")
    {
      cvsl_wp = cvsl_2018_m;
      cvsb_wp = cvsb_2018_m;
    }

    if (cvsl_wp < cvsl && cvsb_wp < cvsb)
    {
      n_c_jet++;
      vec_ctag.push_back(true);
    }
    else
      vec_ctag.push_back(false);
  }
  // cout << "test n_of_ctag" << n_c_jet << endl;

  if (!IsDATA)
  {
    // // SF for b-tagging old & buggy
    // if (run_debug)
    //   weight_b_tag = mcCorr->GetBTaggingReweight_1a(vec_sel_jet, vec_jet_tagging_para.at(0));
    // else
    // {
    //   if (param.syst_ == AnalyzerParameter::Central && run_syst)
    //   {
    //     weight_b_tag = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "central");

    //     weight_b_tag_down_hf = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_hf");
    //     weight_b_tag_up_hf = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_hf");

    //     weight_b_tag_down_lf = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_lf");
    //     weight_b_tag_up_lf = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_lf");

    //     weight_b_tag_down_lfstats1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_lfstats1");
    //     weight_b_tag_up_lfstats1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_lfstats1");

    //     weight_b_tag_down_lfstats2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_lfstats2");
    //     weight_b_tag_up_lfstats2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_lfstats2");

    //     weight_b_tag_down_cferr1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_cferr1");
    //     weight_b_tag_up_cferr1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_cferr1");

    //     weight_b_tag_down_cferr2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_cferr2");
    //     weight_b_tag_up_cferr2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_cferr2");

    //     weight_b_tag_down_hfstats1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_hfstats1");
    //     weight_b_tag_up_hfstats1 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_hfstats1");

    //     weight_b_tag_down_hfstats2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_hfstats2");
    //     weight_b_tag_up_hfstats2 = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_hfstats2");
    //   }
    //   else if (param.syst_ == AnalyzerParameter::JetEnDown)
    //     weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "down_jes");
    //   else if (param.syst_ == AnalyzerParameter::JetEnUp)
    //     weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "up_jes");
    //   else
    //     weight_b_tag = mcCorr->GetBTaggingReweight_1d(vec_sel_jet, vec_jet_tagging_para.at(0), "central");
    // } //   if (run_debug)

    if (param.syst_ == AnalyzerParameter::Central)
    {
      weight_b_tag = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "central");

      weight_b_tag_down_hf = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_hf");
      weight_b_tag_up_hf = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_hf");

      weight_b_tag_down_lf = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_lf");
      weight_b_tag_up_lf = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_lf");

      weight_b_tag_down_lfstats1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_lfstats1");
      weight_b_tag_up_lfstats1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_lfstats1");

      weight_b_tag_down_lfstats2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_lfstats2");
      weight_b_tag_up_lfstats2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_lfstats2");

      weight_b_tag_down_cferr1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_cferr1");
      weight_b_tag_up_cferr1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_cferr1");

      weight_b_tag_down_cferr2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_cferr2");
      weight_b_tag_up_cferr2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_cferr2");

      weight_b_tag_down_hfstats1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_hfstats1");
      weight_b_tag_up_hfstats1 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_hfstats1");

      weight_b_tag_down_hfstats2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_hfstats2");
      weight_b_tag_up_hfstats2 = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_hfstats2");
    }
    else if (param.syst_ == AnalyzerParameter::JetEnDown)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jes");
    else if (param.syst_ == AnalyzerParameter::JetEnUp)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jes");
    else if (param.syst_ == AnalyzerParameter::JetEnAbsoluteDown)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesAbsolute");
    else if (param.syst_ == AnalyzerParameter::JetEnAbsoluteUp)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesAbsolute");
    else if (param.syst_ == AnalyzerParameter::JetEnBBEC1Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesBBEC1");
    else if (param.syst_ == AnalyzerParameter::JetEnBBEC1Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesBBEC1");
    else if (param.syst_ == AnalyzerParameter::JetEnEC2Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesEC2");
    else if (param.syst_ == AnalyzerParameter::JetEnEC2Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesEC2");
    else if (param.syst_ == AnalyzerParameter::JetEnFlavorQCDDown)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesFlavorQCD");
    else if (param.syst_ == AnalyzerParameter::JetEnFlavorQCDUp)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesFlavorQCD");
    else if (param.syst_ == AnalyzerParameter::JetEnHFDown)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesHF");
    else if (param.syst_ == AnalyzerParameter::JetEnHFUp)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesHF");
    else if (param.syst_ == AnalyzerParameter::JetEnRelativeBalDown)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesRelativeBal");
    else if (param.syst_ == AnalyzerParameter::JetEnRelativeBalUp)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesRelativeBal");
    else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2016Down || param.syst_ == AnalyzerParameter::JetEnAbsolute2017Down || param.syst_ == AnalyzerParameter::JetEnAbsolute2017Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesAbsolute_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnAbsolute2016Up || param.syst_ == AnalyzerParameter::JetEnAbsolute2017Up || param.syst_ == AnalyzerParameter::JetEnAbsolute2017Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesAbsolute_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnBBEC12016Down || param.syst_ == AnalyzerParameter::JetEnBBEC12017Down || param.syst_ == AnalyzerParameter::JetEnBBEC12018Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesBBEC1_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnBBEC12016Up || param.syst_ == AnalyzerParameter::JetEnBBEC12017Up || param.syst_ == AnalyzerParameter::JetEnBBEC12018Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesBBEC1_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnEC22016Down || param.syst_ == AnalyzerParameter::JetEnEC22017Down || param.syst_ == AnalyzerParameter::JetEnEC22018Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesEC2_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnEC22016Up || param.syst_ == AnalyzerParameter::JetEnEC22017Up || param.syst_ == AnalyzerParameter::JetEnEC22018Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesEC2_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnHF2016Down || param.syst_ == AnalyzerParameter::JetEnHF2017Down || param.syst_ == AnalyzerParameter::JetEnHF2018Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesHF_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnHF2016Up || param.syst_ == AnalyzerParameter::JetEnHF2017Up || param.syst_ == AnalyzerParameter::JetEnHF2018Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesHF_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Down || param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Down || param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Down)
      weight_b_tag_down_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "down_jesRelativeSample_" + to_string(DataYear));
    else if (param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Up || param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Up || param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Up)
      weight_b_tag_up_jes = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "up_jesRelativeSample_" + to_string(DataYear));
    else
      weight_b_tag = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "central");

    weight *= weight_b_tag;
  }

  if (!IsDATA)
  {
    // SF for c-tagging JSON based
    if (param.syst_ == AnalyzerParameter::Central)
    {
      weight_c_tag = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "central");

      weight_c_tag_down_extrap = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_Extrap");
      weight_c_tag_up_extrap = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_Extrap");

      weight_c_tag_down_interp = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_Interp");
      weight_c_tag_up_interp = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_Interp");

      weight_c_tag_down_lhe_scale_muf = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_LHEScaleWeight_muF");
      weight_c_tag_up_lhe_scale_muf = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_LHEScaleWeight_muF");

      weight_c_tag_down_lhe_scale_mur = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_LHEScaleWeight_muR");
      weight_c_tag_up_lhe_scale_mur = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_LHEScaleWeight_muR");

      weight_c_tag_down_ps_fsr_fixed = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_PSWeightFSR");
      weight_c_tag_up_ps_fsr_fixed = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_PSWeightFSR");

      weight_c_tag_down_ps_isr_fixed = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_PSWeightISR");
      weight_c_tag_up_ps_isr_fixed = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_PSWeightISR");

      weight_c_tag_down_pu = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_PUWeight");
      weight_c_tag_up_pu = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_PUWeight");

      weight_c_tag_down_stat = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_Stat");
      weight_c_tag_up_stat = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_Stat");

      weight_c_tag_down_xsec_brunc_dyjets_b = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_XSec_BRUnc_DYJets_b");
      weight_c_tag_up_xsec_brunc_dyjets_b = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_XSec_BRUnc_DYJets_b");

      weight_c_tag_down_xsec_brunc_dyjets_c = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_XSec_BRUnc_DYJets_c");
      weight_c_tag_up_xsec_brunc_dyjets_c = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_XSec_BRUnc_DYJets_c");

      weight_c_tag_down_xsec_brunc_wjets_c = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_XSec_BRUnc_WJets_c");
      weight_c_tag_up_xsec_brunc_wjets_c = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_XSec_BRUnc_WJets_c");
    } // central
    else if (param.syst_ == AnalyzerParameter::JetResDown)
      weight_c_tag_down_jer = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_jer");
    else if (param.syst_ == AnalyzerParameter::JetResUp)
      weight_c_tag_up_jer = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_jer");
    else if (param.syst_ == AnalyzerParameter::JetEnDown ||
             param.syst_ == AnalyzerParameter::JetEnAbsoluteDown ||
             param.syst_ == AnalyzerParameter::JetEnBBEC1Down ||
             param.syst_ == AnalyzerParameter::JetEnEC2Down ||
             param.syst_ == AnalyzerParameter::JetEnFlavorQCDDown ||
             param.syst_ == AnalyzerParameter::JetEnHFDown ||
             param.syst_ == AnalyzerParameter::JetEnRelativeBalDown ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2016Down ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12016Down ||
             param.syst_ == AnalyzerParameter::JetEnEC22016Down ||
             param.syst_ == AnalyzerParameter::JetEnHF2016Down ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Down ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2017Down ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12017Down ||
             param.syst_ == AnalyzerParameter::JetEnEC22017Down ||
             param.syst_ == AnalyzerParameter::JetEnHF2017Down ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Down ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2018Down ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12018Down ||
             param.syst_ == AnalyzerParameter::JetEnEC22018Down ||
             param.syst_ == AnalyzerParameter::JetEnHF2018Down ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Down)
      weight_c_tag_down_jes_total = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "down_jesTotal");
    else if (param.syst_ == AnalyzerParameter::JetEnUp ||
             param.syst_ == AnalyzerParameter::JetEnAbsoluteUp ||
             param.syst_ == AnalyzerParameter::JetEnBBEC1Up ||
             param.syst_ == AnalyzerParameter::JetEnEC2Up ||
             param.syst_ == AnalyzerParameter::JetEnFlavorQCDUp ||
             param.syst_ == AnalyzerParameter::JetEnHFUp ||
             param.syst_ == AnalyzerParameter::JetEnRelativeBalUp ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2016Up ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12016Up ||
             param.syst_ == AnalyzerParameter::JetEnEC22016Up ||
             param.syst_ == AnalyzerParameter::JetEnHF2016Up ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2016Up ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2017Up ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12017Up ||
             param.syst_ == AnalyzerParameter::JetEnEC22017Up ||
             param.syst_ == AnalyzerParameter::JetEnHF2017Up ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2017Up ||
             param.syst_ == AnalyzerParameter::JetEnAbsolute2018Up ||
             param.syst_ == AnalyzerParameter::JetEnBBEC12018Up ||
             param.syst_ == AnalyzerParameter::JetEnEC22018Up ||
             param.syst_ == AnalyzerParameter::JetEnHF2018Up ||
             param.syst_ == AnalyzerParameter::JetEnRelativeSample2018Up)
      weight_c_tag_up_jes_total = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "up_jesTotal");
    else
      weight_c_tag = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "central");

    weight *= weight_c_tag;
  }

  // at least two b-tag jets
  if (n_b_jet < 2)
    return;

  met_pt = met.Pt();
  met_phi = met.Phi();

  // mt
  mt = Calculate_Mt(lepton, met.Px(), met.Py());

  Make_Result_Tree(param);

  return;
} // Vcb_Cal_TF::executeEventFromParameter(AnalyzerParameter param)

//////////

float Vcb_Cal_TF::Calculate_Mt(const Particle &lepton, const float &neu_px, const float &neu_py)
{
  float lepton_px = lepton.Px();
  float lepton_py = lepton.Py();
  float lepton_et = Sqrt(lepton_px * lepton_px + lepton_py * lepton_py);

  float neu_et = Sqrt(neu_px * neu_px + neu_py * neu_py);

  TVector3 lepton3(lepton_px, lepton_py, 0);
  TVector3 neu3(neu_px, neu_py, 0);

  float angle = lepton3.Angle(neu3);

  float mt = Sqrt(lepton_et * neu_et * (1 - Cos(angle)));

  return mt;
} // float Vcb_Cal_TF::Calculate_Mt(const Particle &lepton, const float &neu_px, const float &neu_py)

//////////

void Vcb_Cal_TF::Clear()
{
  weight = 1;

  weight_b_tag = 1;
  weight_b_tag_down_hf = 1;
  weight_b_tag_up_hf = 1;
  weight_b_tag_down_lf = 1;
  weight_b_tag_up_lf = 1;
  weight_b_tag_down_jes = 1;
  weight_b_tag_up_jes = 1;
  weight_b_tag_down_lfstats1 = 1;
  weight_b_tag_up_lfstats1 = 1;
  weight_b_tag_down_lfstats2 = 1;
  weight_b_tag_up_lfstats2 = 1;
  weight_b_tag_down_cferr1 = 1;
  weight_b_tag_up_cferr1 = 1;
  weight_b_tag_down_cferr2 = 1;
  weight_b_tag_up_cferr2 = 1;
  weight_b_tag_down_hfstats1 = 1;
  weight_b_tag_up_hfstats1 = 1;
  weight_b_tag_down_hfstats2 = 1;
  weight_b_tag_up_hfstats2 = 1;

  weight_c_tag = 1;

  weight_hem_veto = 1;
  weight_lumi = 1;
  weight_mc = 1;

  weight_pdf_alternative = 1;
  memset(weight_pdf_error_set, 1, sizeof(weight_pdf_error_set));
  weight_pdf_as_up = 1;
  weight_pdf_as_down = 1;

  weight_pileup = 1;
  weight_pileup_down = 1;
  weight_pileup_up = 1;

  weight_pujet_veto = 1;
  weight_pujet_veto_down = 1;
  weight_pujet_veto_up = 1;

  weight_prefire = 1;
  weight_prefire_down = 1;
  weight_prefire_up = 1;

  weight_scale_variation_1 = 1;
  weight_scale_variation_2 = 1;
  weight_scale_variation_3 = 1;
  weight_scale_variation_4 = 1;
  weight_scale_variation_6 = 1;
  weight_scale_variation_8 = 1;

  weight_top_pt = 1;

  weight_mu_id = 1;
  weight_mu_id_down = 1;
  weight_mu_id_up = 1;

  weight_mu_iso = 1;
  weight_mu_iso_down = 1;
  weight_mu_iso_up = 1;

  weight_el_id = 1;
  weight_el_id_down = 1;
  weight_el_id_up = 1;

  weight_el_reco = 1;
  weight_el_reco_down = 1;
  weight_el_reco_up = 1;

  weight_sl_trig = 1;
  weight_sl_trig_down = 1;
  weight_sl_trig_up = 1;

  vec_this_muon.clear();
  vec_this_electron.clear();
  vec_this_jet.clear();

  vec_sel_jet.clear();

  vec_btag.clear();
  vec_ctag.clear();

  vec_jet_pt.clear();
  vec_jet_eta.clear();
  vec_jet_phi.clear();
  vec_jet_mass.clear();
  vec_jet_bvsc.clear();
  vec_jet_cvsb.clear();
  vec_jet_cvsl.clear();

  vec_jet_flavor.clear();

  return;
} // void Vcb_Cal_TF::Clear()

//////////

int Vcb_Cal_TF::Chk_Included(const int index_matched_jet[4])
{
  int result = 0;
  unsigned int tmp = 1;
  for (int i = 0; i < 4; ++i)
  {
    if (index_matched_jet[i] < 0)
      result += tmp;

    tmp = tmp << 1;
  } // for(int i=0; i<4; i++)

  // 0: every gen jet is matched to loose jets wo/ lepton veto and the loose jets passes baseline selection
  return result;
} // int Vcb_Cal_TF::Chk_Included(const int index_matched_jet[4])

//////////

void Vcb_Cal_TF::Make_Result_Tree(AnalyzerParameter &param)
{
  map_result_tree[channel_name + param.GetSystType()]->Fill();

  return;
} // void Vcb_Cal_TF::Make_Result_Tree()

//////////

Particle Vcb_Cal_TF::Rebalance_Met()
{
  // cout << "before " << met.Pt() << " " << met.Phi() << endl;

  Particle met_rebal;
  met_rebal += met;

  // before
  for (auto &muon : vec_muon)
    met_rebal += muon;
  for (auto &electron : vec_electron)
    met_rebal += electron;
  for (auto &jet : vec_jet)
    met_rebal += jet;

  // after
  for (auto &muon : vec_this_muon)
    met_rebal -= muon;
  for (auto &electron : vec_this_electron)
    met_rebal -= electron;
  for (auto &jet : vec_this_jet)
    met_rebal -= jet;

  // cout << "after " << met_rebal.Pt() << " " << met_rebal.Phi() << endl;

  return met_rebal;
} // Particle Vcb_Cal_TF::Rebalance_Met()

//////////

void Vcb_Cal_TF::Set_Result_Tree()
{
  for (unsigned int i = 0; i < vec_channel.size(); i++)
  {
    for (unsigned int j = 0; j < vec_syst_type.size(); j++)
    {
      AnalyzerParameter::Syst syst_type = vec_syst_type.at(j);
      param.syst_ = syst_type;

      TTree *result_tree = new TTree("Result_Tree", "Result_Tree");
      result_tree->SetDirectory(dir_syst[i][j]);

      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_b_tag", &weight_b_tag);
        result_tree->Branch("weight_b_tag_down_hf", &weight_b_tag_down_hf);
        result_tree->Branch("weight_b_tag_up_hf", &weight_b_tag_up_hf);
        result_tree->Branch("weight_b_tag_down_lf", &weight_b_tag_down_lf);
        result_tree->Branch("weight_b_tag_up_lf", &weight_b_tag_up_lf);
        result_tree->Branch("weight_b_tag_down_lfstats1", &weight_b_tag_down_lfstats1);
        result_tree->Branch("weight_b_tag_up_lfstats1", &weight_b_tag_up_lfstats1);
        result_tree->Branch("weight_b_tag_down_lfstats2", &weight_b_tag_down_lfstats2);
        result_tree->Branch("weight_b_tag_up_lfstats2", &weight_b_tag_up_lfstats2);
        result_tree->Branch("weight_b_tag_down_cferr1", &weight_b_tag_down_cferr1);
        result_tree->Branch("weight_b_tag_up_cferr1", &weight_b_tag_up_cferr1);
        result_tree->Branch("weight_b_tag_down_cferr2", &weight_b_tag_down_cferr2);
        result_tree->Branch("weight_b_tag_up_cferr2", &weight_b_tag_up_cferr2);
        result_tree->Branch("weight_b_tag_down_hfstats1", &weight_b_tag_down_hfstats1);
        result_tree->Branch("weight_b_tag_up_hfstats1", &weight_b_tag_up_hfstats1);
        result_tree->Branch("weight_b_tag_down_hfstats2", &weight_b_tag_down_hfstats2);
        result_tree->Branch("weight_b_tag_up_hfstats2", &weight_b_tag_up_hfstats2);
      }
      else if (syst_type == AnalyzerParameter::JetEnDown)
        result_tree->Branch("weight_b_tag_down_jes", &weight_b_tag_down_jes);
      else if (syst_type == AnalyzerParameter::JetEnUp)
        result_tree->Branch("weight_b_tag_up_jes", &weight_b_tag_up_jes);
      else
        result_tree->Branch("weight_b_tag", &weight_b_tag);

      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_c_tag", &weight_c_tag);
        result_tree->Branch("weight_c_tag_down_extrap", &weight_c_tag_down_extrap);
        result_tree->Branch("weight_c_tag_up_extrap", &weight_c_tag_up_extrap);
        result_tree->Branch("weight_c_tag_down_interp", &weight_c_tag_down_interp);
        result_tree->Branch("weight_c_tag_up_interp", &weight_c_tag_up_interp);
        result_tree->Branch("weight_c_tag_down_lhe_scale_muf", &weight_c_tag_down_lhe_scale_muf);
        result_tree->Branch("weight_c_tag_up_lhe_scale_muf", &weight_c_tag_up_lhe_scale_muf);
        result_tree->Branch("weight_c_tag_down_lhe_scale_mur", &weight_c_tag_down_lhe_scale_mur);
        result_tree->Branch("weight_c_tag_up_lhe_scale_mur", &weight_c_tag_up_lhe_scale_mur);
        result_tree->Branch("weight_c_tag_down_ps_fsr_fixed", &weight_c_tag_down_ps_fsr_fixed);
        result_tree->Branch("weight_c_tag_up_ps_fsr_fixed", &weight_c_tag_up_ps_fsr_fixed);
        result_tree->Branch("weight_c_tag_down_ps_isr_fixed", &weight_c_tag_down_ps_isr_fixed);
        result_tree->Branch("weight_c_tag_up_ps_isr_fixed", &weight_c_tag_up_ps_isr_fixed);
        result_tree->Branch("weight_c_tag_down_pu", &weight_c_tag_down_pu);
        result_tree->Branch("weight_c_tag_up_pu", &weight_c_tag_up_pu);
        result_tree->Branch("weight_c_tag_down_stat", &weight_c_tag_down_stat);
        result_tree->Branch("weight_c_tag_up_stat", &weight_c_tag_up_stat);
        result_tree->Branch("weight_c_tag_down_xsec_brunc_dyjets_b", &weight_c_tag_down_xsec_brunc_dyjets_b);
        result_tree->Branch("weight_c_tag_up_xsec_brunc_dyjets_b", &weight_c_tag_up_xsec_brunc_dyjets_b);
        result_tree->Branch("weight_c_tag_down_xsec_brunc_dyjets_c", &weight_c_tag_down_xsec_brunc_dyjets_c);
        result_tree->Branch("weight_c_tag_up_xsec_brunc_dyjets_c", &weight_c_tag_up_xsec_brunc_dyjets_c);
        result_tree->Branch("weight_c_tag_down_xsec_brunc_wjets_c", &weight_c_tag_down_xsec_brunc_wjets_c);
        result_tree->Branch("weight_c_tag_up_xsec_brunc_wjets_c", &weight_c_tag_up_xsec_brunc_wjets_c);
      }
      else if (syst_type == AnalyzerParameter::JetResDown)
        result_tree->Branch("weight_c_tag_down_jer", &weight_c_tag_down_jer);
      else if (syst_type == AnalyzerParameter::JetResUp)
        result_tree->Branch("weight_c_tag_up_jer", &weight_c_tag_up_jer);
      else if (syst_type == AnalyzerParameter::JetEnDown ||
               syst_type == AnalyzerParameter::JetEnAbsoluteDown ||
               syst_type == AnalyzerParameter::JetEnBBEC1Down ||
               syst_type == AnalyzerParameter::JetEnEC2Down ||
               syst_type == AnalyzerParameter::JetEnFlavorQCDDown ||
               syst_type == AnalyzerParameter::JetEnHFDown ||
               syst_type == AnalyzerParameter::JetEnRelativeBalDown ||
               syst_type == AnalyzerParameter::JetEnAbsolute2016Down ||
               syst_type == AnalyzerParameter::JetEnBBEC12016Down ||
               syst_type == AnalyzerParameter::JetEnEC22016Down ||
               syst_type == AnalyzerParameter::JetEnHF2016Down ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2016Down ||
               syst_type == AnalyzerParameter::JetEnAbsolute2017Down ||
               syst_type == AnalyzerParameter::JetEnBBEC12017Down ||
               syst_type == AnalyzerParameter::JetEnEC22017Down ||
               syst_type == AnalyzerParameter::JetEnHF2017Down ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2017Down ||
               syst_type == AnalyzerParameter::JetEnAbsolute2018Down ||
               syst_type == AnalyzerParameter::JetEnBBEC12018Down ||
               syst_type == AnalyzerParameter::JetEnEC22018Down ||
               syst_type == AnalyzerParameter::JetEnHF2018Down ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2018Down)
        result_tree->Branch("weight_c_tag_down_jes_total", &weight_c_tag_down_jes_total);
      else if (syst_type == AnalyzerParameter::JetEnUp ||
               syst_type == AnalyzerParameter::JetEnAbsoluteUp ||
               syst_type == AnalyzerParameter::JetEnBBEC1Up ||
               syst_type == AnalyzerParameter::JetEnEC2Up ||
               syst_type == AnalyzerParameter::JetEnFlavorQCDUp ||
               syst_type == AnalyzerParameter::JetEnHFUp ||
               syst_type == AnalyzerParameter::JetEnRelativeBalUp ||
               syst_type == AnalyzerParameter::JetEnAbsolute2016Up ||
               syst_type == AnalyzerParameter::JetEnBBEC12016Up ||
               syst_type == AnalyzerParameter::JetEnEC22016Up ||
               syst_type == AnalyzerParameter::JetEnHF2016Up ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2016Up ||
               syst_type == AnalyzerParameter::JetEnAbsolute2017Up ||
               syst_type == AnalyzerParameter::JetEnBBEC12017Up ||
               syst_type == AnalyzerParameter::JetEnEC22017Up ||
               syst_type == AnalyzerParameter::JetEnHF2017Up ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2017Up ||
               syst_type == AnalyzerParameter::JetEnAbsolute2018Up ||
               syst_type == AnalyzerParameter::JetEnBBEC12018Up ||
               syst_type == AnalyzerParameter::JetEnEC22018Up ||
               syst_type == AnalyzerParameter::JetEnHF2018Up ||
               syst_type == AnalyzerParameter::JetEnRelativeSample2018Up)
        result_tree->Branch("weight_c_tag_up_jes_total", &weight_c_tag_up_jes_total);
      else
        result_tree->Branch("weight_c_tag", &weight_c_tag);

      result_tree->Branch("weight_el_id", &weight_el_id);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_el_id_down", &weight_el_id_down);
        result_tree->Branch("weight_el_id_up", &weight_el_id_up);
      }

      result_tree->Branch("weight_el_reco", &weight_el_reco);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_el_reco_down", &weight_el_reco_down);
        result_tree->Branch("weight_el_reco_up", &weight_el_reco_up);
      }

      result_tree->Branch("weight_hem_veto", &weight_hem_veto);
      result_tree->Branch("weight_lumi", &weight_lumi);
      result_tree->Branch("weight_mc", &weight_mc);

      result_tree->Branch("weight_mu_id", &weight_mu_id);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_mu_id_down", &weight_mu_id_down);
        result_tree->Branch("weight_mu_id_up", &weight_mu_id_up);
      }

      result_tree->Branch("weight_mu_iso", &weight_mu_iso);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_mu_iso_down", &weight_mu_iso_down);
        result_tree->Branch("weight_mu_iso_up", &weight_mu_iso_up);
      }

      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_pdf_alternative", &weight_pdf_alternative);
        result_tree->Branch("weight_pdf_error_set", weight_pdf_error_set, "weight_pdf_error_set[100]/F");
        result_tree->Branch("weight_pdf_as_down", &weight_pdf_as_down);
        result_tree->Branch("weight_pdf_as_up", &weight_pdf_as_up);
      }

      result_tree->Branch("weight_pileup", &weight_pileup);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_pileup_down", &weight_pileup_down);
        result_tree->Branch("weight_pileup_up", &weight_pileup_up);
      }

      result_tree->Branch("weight_prefire", &weight_prefire);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_prefire_down", &weight_prefire_down);
        result_tree->Branch("weight_prefire_up", &weight_prefire_up);
      }

      if (syst_type == AnalyzerParameter::Central && run_syst)
        result_tree->Branch("weight_ps", weight_ps, "weight_ps[4]");

      result_tree->Branch("weight_pujet_veto", &weight_pujet_veto);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_pujet_veto_down", &weight_pujet_veto_down);
        result_tree->Branch("weight_pujet_veto_up", &weight_pujet_veto_up);
      }

      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_scale_variation_1", &weight_scale_variation_1);
        result_tree->Branch("weight_scale_variation_2", &weight_scale_variation_2);
        result_tree->Branch("weight_scale_variation_3", &weight_scale_variation_3);
        result_tree->Branch("weight_scale_variation_4", &weight_scale_variation_4);
        result_tree->Branch("weight_scale_variation_6", &weight_scale_variation_6);
        result_tree->Branch("weight_scale_variation_8", &weight_scale_variation_8);
      }

      result_tree->Branch("weight_sl_trig", &weight_sl_trig);
      if (syst_type == AnalyzerParameter::Central && run_syst)
      {
        result_tree->Branch("weight_sl_trig_down", &weight_sl_trig_down);
        result_tree->Branch("weight_sl_trig_up", &weight_sl_trig_up);
      }

      result_tree->Branch("weight_top_pt", &weight_top_pt);

      result_tree->Branch("lepton_pt", &lepton_pt);
      result_tree->Branch("lepton_pt_uncorr", &lepton_pt_uncorr);
      result_tree->Branch("lepton_eta", &lepton_eta);
      result_tree->Branch("lepton_rel_iso", &lepton_rel_iso);

      result_tree->Branch("electron_id_bit", &electron_id_bit, "electron_id_bit/i");

      result_tree->Branch("n_jets", &n_sel_jet);
      result_tree->Branch("n_bjets", &n_b_jet);
      result_tree->Branch("n_cjets", &n_c_jet);

      // result_tree->Branch("pt_leading_jet", &pt_leading_jet);
      // result_tree->Branch("pt_subleading_jet", &pt_subleading_jet);

      // result_tree->Branch("eta_leading_jet", &eta_leading_jet);
      // result_tree->Branch("eta_subleading_jet", &eta_subleading_jet);

      // result_tree->Branch("bvsc_leading_jet", &bvsc_leading_jet);
      // result_tree->Branch("cvsb_leading_jet", &cvsb_leading_jet);
      // result_tree->Branch("cvsl_leading_jet", &cvsl_leading_jet);

      // result_tree->Branch("bvsc_subleading_jet", &bvsc_subleading_jet);
      // result_tree->Branch("cvsb_subleading_jet", &cvsb_subleading_jet);
      // result_tree->Branch("cvsl_subleading_jet", &cvsl_subleading_jet);

      result_tree->Branch("met_pt", &met_pt);
      result_tree->Branch("met_phi", &met_phi);

      result_tree->Branch("mt", &mt);

      // For MC
      result_tree->Branch("decay_mode", &decay_mode);
      result_tree->Branch("genTtbarId", &genTtbarId);

      result_tree->Branch("Jet_Pt", &vec_jet_pt);
      result_tree->Branch("Jet_Eta", &vec_jet_eta);
      result_tree->Branch("Jet_Flavor", &vec_jet_flavor);

      map_result_tree.insert({vec_channel[i] + param.GetSystType(), result_tree});
    } // loop over syst
  } // loop over channel

  return;
} // void Vcb_Cal_TF::Set_Result_Tree()

//////////
