#include "Val_B_Frag.h"

//////////

Val_B_Frag::Val_B_Frag()
{
} // Val_B_Frag::Val_B_Frag()

//////////

Val_B_Frag::~Val_B_Frag()
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

} // Val_B_Frag::~Val_B_Frag()

//////////

void Val_B_Frag::initializeAnalyzer()
{
  fChain->SetBranchStatus("fatjet*", 0);
  fChain->SetBranchStatus("photon*", 0);
  fChain->SetBranchStatus("tau*", 0);

  //===========================================================================
  // [Optimization] Disable unused Electron branches
  // Electron 클래스에 저장 공간(멤버 변수)이 없는 브랜치들을 비활성화합니다.
  //===========================================================================

  // 1. GSF Track 상세 정보 (General Track 정보만 사용됨)
  fChain->SetBranchStatus("electron_gsfpt", 0);
  fChain->SetBranchStatus("electron_gsfEta", 0);
  fChain->SetBranchStatus("electron_gsfPhi", 0);
  fChain->SetBranchStatus("electron_gsfCharge", 0);

  // 2. SuperCluster 상세 정보 (Eta, Phi, Energy 외 미사용)
  fChain->SetBranchStatus("electron_scPreEnergy", 0);
  fChain->SetBranchStatus("electron_scRawEnergy", 0);
  fChain->SetBranchStatus("electron_scEt", 0);
  fChain->SetBranchStatus("electron_etaWidth", 0);
  fChain->SetBranchStatus("electron_phiWidth", 0);

  // 3. Shower Shape & Energy Fraction (사용되는 변수 제외하고 비활성화)
  // GetAllElectrons에서는 electron_dEtaInSeed를 사용함
  fChain->SetBranchStatus("electron_dEtaIn", 0);
  // GetAllElectrons에서는 electron_Full5x5_SigmaIEtaIEta를 사용함
  fChain->SetBranchStatus("electron_sigmaIEtaIEta", 0);

  fChain->SetBranchStatus("electron_fbrem", 0);
  fChain->SetBranchStatus("electron_eOverP", 0);
  fChain->SetBranchStatus("electron_E15", 0);
  fChain->SetBranchStatus("electron_E25", 0);
  fChain->SetBranchStatus("electron_E55", 0);

  // 4. Isolation - Cone 0.3 상세 성분 (Rho 보정된 RelIso만 사용됨)
  // MiniIso 관련 변수(electron_chMiniIso 등)는 사용되므로 끄면 안 됨!
  fChain->SetBranchStatus("electron_chIso03", 0);
  fChain->SetBranchStatus("electron_nhIso03", 0);
  fChain->SetBranchStatus("electron_phIso03", 0);
  fChain->SetBranchStatus("electron_puChIso03", 0);

  // GetAllElectrons에서 Rho 버전만 사용함
  fChain->SetBranchStatus("electron_RelPFIso_dBeta", 0);

  // 5. Impact Parameter & Vertex (VTX 접미사가 붙은 변수만 사용됨)
  // 주의: electron_dxyVTX 등은 사용되므로 끄면 안 됨
  fChain->SetBranchStatus("electron_dxy", 0);
  fChain->SetBranchStatus("electron_sigdxy", 0);
  fChain->SetBranchStatus("electron_dz", 0);

  // BeamSpot 기준 변수는 사용 안 함
  fChain->SetBranchStatus("electron_dxyBS", 0);
  fChain->SetBranchStatus("electron_dzBS", 0);

  // 6. Charge Consistency (GsfCtfScPix... 버전 하나만 사용됨)
  fChain->SetBranchStatus("electron_isGsfScPixChargeConsistent", 0);
  fChain->SetBranchStatus("electron_isGsfCtfChargeConsistent", 0);

  fChain->SetBranchStatus("muon_PfChargedHadronIsoR03", 0);
  fChain->SetBranchStatus("muon_PfNeutralHadronIsoR03", 0);
  fChain->SetBranchStatus("muon_PfGammaIsoR03", 0);
  fChain->SetBranchStatus("muon_PFSumPUIsoR03", 0);

  fChain->SetBranchStatus("muon_hcaliso", 0);
  fChain->SetBranchStatus("muon_ecaliso", 0);

  fChain->SetBranchStatus("muon_trkisoR05", 0);
  fChain->SetBranchStatus("muon_hcalisoR05", 0);
  fChain->SetBranchStatus("muon_ecalisoR05", 0);

  // 2. Track & Hit 상세 정보
  // (GetAllMuons는 muon_trackerLayers와 muon_normchi(Chi2)만 사용함)
  fChain->SetBranchStatus("muon_nChambers", 0);
  fChain->SetBranchStatus("muon_matchedstations", 0);
  fChain->SetBranchStatus("muon_stationMask", 0);
  fChain->SetBranchStatus("muon_nSegments", 0);
  fChain->SetBranchStatus("muon_validhits", 0);
  fChain->SetBranchStatus("muon_trackerHits", 0);
  fChain->SetBranchStatus("muon_pixelHits", 0);
  fChain->SetBranchStatus("muon_validmuonhits", 0);

  // 3. Impact Parameter & Vertex (VTX가 안 붙은 변수들)
  // (GetAllMuons는 무조건 VTX가 붙은 변수(dxyVTX 등)만 사용함)
  fChain->SetBranchStatus("muon_dxy", 0);
  fChain->SetBranchStatus("muon_d0", 0);
  fChain->SetBranchStatus("muon_dsz", 0);
  fChain->SetBranchStatus("muon_dz", 0);
  fChain->SetBranchStatus("muon_dB", 0); // dB도 사용 안 함

  // BeamSpot 기준 변수 미사용
  fChain->SetBranchStatus("muon_dxyBS", 0);
  fChain->SetBranchStatus("muon_dzBS", 0);
  fChain->SetBranchStatus("muon_dszBS", 0);

  // Vertex 좌표 자체는 미사용 (dxyVTX 값을 직접 씀)
  fChain->SetBranchStatus("muon_vx", 0);
  fChain->SetBranchStatus("muon_vy", 0);
  fChain->SetBranchStatus("muon_vz", 0);

  // 4. Detailed Track Parameters (Inner, Outer, Global, Best)
  // (GetAllMuons는 muon_pt(MiniAOD)와 muon_TuneP_pt만 사용함)
  // Best Track
  fChain->SetBranchStatus("muon_Best_pt", 0);
  fChain->SetBranchStatus("muon_Best_ptError", 0);
  fChain->SetBranchStatus("muon_Best_eta", 0);
  fChain->SetBranchStatus("muon_Best_phi", 0);

  // Inner Track
  fChain->SetBranchStatus("muon_Inner_pt", 0);
  fChain->SetBranchStatus("muon_Inner_ptError", 0);
  fChain->SetBranchStatus("muon_Inner_eta", 0);
  fChain->SetBranchStatus("muon_Inner_phi", 0);

  // Outer Track
  fChain->SetBranchStatus("muon_Outer_pt", 0);
  fChain->SetBranchStatus("muon_Outer_ptError", 0);
  fChain->SetBranchStatus("muon_Outer_eta", 0);
  fChain->SetBranchStatus("muon_Outer_phi", 0);

  // Global Track
  fChain->SetBranchStatus("muon_GLB_pt", 0);
  fChain->SetBranchStatus("muon_GLB_ptError", 0);
  fChain->SetBranchStatus("muon_GLB_eta", 0);
  fChain->SetBranchStatus("muon_GLB_phi", 0);

  // 기타 Track 파라미터
  fChain->SetBranchStatus("muon_qoverp", 0);
  fChain->SetBranchStatus("muon_theta", 0);
  fChain->SetBranchStatus("muon_lambda", 0);

  // 5. MVA & Jet Relation
  // (muon_MVA는 사용하지만, lowpt/soft/jetPtRel 등은 Muon 클래스에 넣지 않음)
  fChain->SetBranchStatus("muon_lowptMVA", 0);
  fChain->SetBranchStatus("muon_softMVA", 0);
  fChain->SetBranchStatus("muon_jetPtRatio", 0);
  fChain->SetBranchStatus("muon_jetPtRel", 0);

  // 6. Simulation Truth
  fChain->SetBranchStatus("muon_simType", 0);
  fChain->SetBranchStatus("muon_simExtType", 0);
  fChain->SetBranchStatus("muon_simFlavour", 0);
  fChain->SetBranchStatus("muon_simHeaviestMotherFlavour", 0);
  fChain->SetBranchStatus("muon_simPdgId", 0);
  fChain->SetBranchStatus("muon_simMotherPdgId", 0);
  fChain->SetBranchStatus("muon_simMatchQuality", 0);

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
    cout << "[Val_B_Frag::initializeAnalyzer] Single Muon Trigger Name = " << trigger_name << endl;
  cout << "[Val_B_Frag::initializeAnalyzer] Single Muon Trigger Safe Pt Cut = " << mu_trig_safe_pt_cut << endl;

  for (auto &trigger_name : vec_el_trig)
    cout << "[Val_B_Frag::initializeAnalyzer] Single Electron Trigger Name = " << trigger_name << endl;
  cout << "[Val_B_Frag::initializeAnalyzer] Single Electron Trigger Safe Pt Cut = " << el_trig_safe_pt_cut << endl;

  // Jet Tagging Parameters
  vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::iterativefit, JetTagging::iterativefit));
  vec_jet_tagging_para.push_back(JetTagging::Parameters(JetTagging::DeepJet_C, JetTagging::Medium, JetTagging::iterativefit, JetTagging::iterativefit));
  mcCorr->SetJetTaggingParameters(vec_jet_tagging_para);

  vec_channel = {"Mu", "El"};

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
} // void Val_B_Frag::initializeAnalyzer()

//////////

void Val_B_Frag::executeEvent()
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

      param.Muon_Tight_ID = "POGTightWithTightIso";
      // param.Muon_Loose_ID = "POGLooseWithLooseIso";
      param.Muon_Jet_Cleaning_ID = "POGTightWithTightIso";

      param.Electron_Tight_ID = "passMVAID_iso_WP80";
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
} // void Val_B_Frag::executeEvent()

//////////

void Val_B_Frag::executeEventFromParameter(AnalyzerParameter param)
{
  // cout << param.Name << endl;

  Clear();

  Event ev = GetEvent();

  // met filter
  if (!PassMETFilter())
    return;

  // set objects
  vec_this_muon = vec_muon;
  vec_this_electron = vec_electron;
  vec_this_jet = vec_jet;

  met = ev.GetMETVector("PUPPI");

  // xy correction
  pair<double, double> met_corr = xy_met_correction.METXYCorr_Met_MetPhi(met.Pt(), met.Phi(), run, to_string(DataYear), !IsDATA, nPV, true, true);

  // Particle met = ev.GetMETVector("PF");
  // pair<double, double> met_corr = xy_met_correction.METXYCorr_Met_MetPhi(met.Pt(), met.Phi(), run, to_string(DataYear), !IsDATA, nPV, true, false);
  // cout << met.Pt() << " " << met_corr.first << " " << met.Phi() << " " << met_corr.second << endl;

  met.SetPtEtaPhiE(met_corr.first, 0, met_corr.second, met_corr.first);

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
      weight_sl_trig = mcCorr->MuonTrigger_SF("POGTight", mu_trig, vec_sel_muon, 0);

    else if (run_el_ch)
      weight_sl_trig = mcCorr->ElectronTrigger_SF("passMVAID_iso_WP80", el_trig, vec_sel_electron, 0);

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
      weight *= weight_mu_id;

      // SF for muon iso
      weight_mu_iso = mcCorr->MuonISO_SF_Json(param.Muon_ISO_SF_Key, muon.Eta(), muon.MiniAODPt(), 0);
      weight *= weight_mu_iso;
      // if(run_debug) cout << "M ISO SF = " << muon.Eta() << "\t" << muon.MiniAODPt() << "\t" << sf_mu_iso_effi << endl;
    } // if (run_mu_ch)
    else if (run_el_ch)
    {
      // SF for electron id
      weight_el_id = mcCorr->ElectronID_SF(param.Electron_Tight_ID, electron.scEta(), electron.UncorrPt(), 0);
      weight *= weight_el_id;

      // SF for electron Reco eff
      weight_el_reco = mcCorr->ElectronReco_SF(electron.scEta(), electron.UncorrPt(), 0);
      weight *= weight_el_reco;
    } // else if (run_el_ch)
  } //  if (!IsDATA)

  // cut on jet
  // // at least three jets
  if (n_sel_jet < 4)
    return;

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

  // at least two b-tag jets
  if (n_b_jet < 2)
    return;

  if (!IsDATA)
  {
    vector<Gen> vec_gen = GetGens();

    // lumi
    weight_lumi = ev.GetTriggerLumi("Full");
    weight *= weight_lumi;

    // MCweight +1 or -1
    weight_mc = MCweight();
    weight *= weight_mc;

    // pileup reweight
    weight_pileup = mcCorr->GetPileUpWeight_Json(nPileUp, 0);
    weight *= weight_pileup;

    // L1 prefire
    weight_prefire = GetPrefireWeight(0);
    weight *= weight_prefire;

    // Top Pt reweight
    weight_top_pt = mcCorr->GetTopPtReweight(vec_gen);
    weight_top_pt_mva = mcCorr->GetTopPtReweightMVA(vec_gen);
    weight *= weight_top_pt_mva;

    // B fragmentation reweight
    weight_b_frag_mva_nominal = mcCorr->GetBFragReweightMVA(vec_gen, 0);
    weight *= weight_b_frag_mva_nominal;

    weight_b_frag_mva_up = mcCorr->GetBFragReweightMVA(vec_gen, +1);

    // SF for PUJet Veto
    weight_pujet_veto = mcCorr->PileupJetVeto_Reweight(vec_sel_jet, param.PUJet_Veto_ID, 0);
    weight *= weight_pujet_veto;

    weight_b_tag = mcCorr->GetBTaggingReweight_Json(vec_sel_jet, "central");
    weight_c_tag = mcCorr->GetCTaggingReweight_Json(vec_sel_jet, "central");
    weight *= weight_b_tag;

    if (Get_Xb(vec_gen, xb_top, xb_antitop) == -999)
      return;
  } // if (!IsDATA)

  met_pt = met.Pt();
  met_phi = met.Phi();

  map_result_tree[channel_name + param.GetSystType()]->Fill();

  return;
} // Val_B_Frag::executeEventFromParameter(AnalyzerParameter param)

//////////

void Val_B_Frag::Clear()
{
  weight = 1;

  weight_b_tag = 1;
  weight_c_tag = 1;

  weight_hem_veto = 1;
  weight_lumi = 1;
  weight_mc = 1;

  weight_pileup = 1;

  weight_pujet_veto = 1;

  weight_prefire = 1;

  weight_top_pt = 1;
  weight_top_pt_mva = 1;

  weight_b_frag_mva_nominal = 1;
  weight_b_frag_mva_up = 1;

  weight_mu_id = 1;

  weight_mu_iso = 1;

  weight_el_id = 1;

  weight_el_reco = 1;

  weight_sl_trig = 1;

  vec_this_muon.clear();
  vec_this_electron.clear();
  vec_this_jet.clear();

  vec_sel_jet.clear();

  vec_btag.clear();
  vec_ctag.clear();

  return;
} // void Val_B_Frag::Clear()

//////////

int Val_B_Frag::Get_Xb(const vector<Gen> &gens, float &xb_top, float &xb_antitop)
{
  if (!MCSample.Contains("TT"))
    return -999;

  int index_last_t = -999;
  int index_last_at = -999;
  int index_last_b = -999;
  int index_last_ab = -999;
  int index_last_w_plus = -999;
  int index_last_w_minus = -999;
  vector<int> index_b_hadrons;
  vector<int> index_ab_hadrons;
  for (unsigned int i = 0; i < gens.size(); i++)
  {
    Gen gen = gens.at(i);

    int pid = gen.PID();
    int m_index = gen.MotherIndex();

    // find last index of t and tbar
    if (pid == 6)
      index_last_t = i;
    if (pid == -6)
      index_last_at = i;

    // find b from t decay
    if (m_index == index_last_t && pid == 5)
      index_last_b = i;
    if (m_index == index_last_b && pid == 5)
      index_last_b = i;

    if (m_index == index_last_at && pid == -5)
      index_last_ab = i;
    if (m_index == index_last_ab && pid == -5)
      index_last_ab = i;

    // find w+
    if (m_index == index_last_t && pid == 24)
      index_last_w_plus = i;
    if (m_index == index_last_w_plus && pid == 24)
      index_last_w_plus = i;

    // find w-
    if (m_index == index_last_at && pid == -24)
      index_last_w_minus = i;
    if (m_index == index_last_w_minus && pid == -24)
      index_last_w_minus = i;

    // if (((pid / 100) % 10 == -5 || (pid / 1000) % 10 == 5) && (m_index == index_last_b || gens[m_index].PID() == 2212))
    if ((pid / 100) % 10 == -5 || (pid / 1000) % 10 == 5)
    {
      float dr = 999.;
      if (index_last_b != -999)
        dr = TMath::Sqrt(TMath::Power(gen.Eta() - gens[index_last_b].Eta(), 2) + TMath::Power(gen.Phi() - gens[index_last_b].Phi(), 2));

      if (dr < 0.2)
        index_b_hadrons.push_back(i);
    }

    if ((pid / 100) % 10 == 5 || (pid / 1000) % 10 == -5)
    {
      float dr = 999.;
      if (index_last_ab != -999)
        dr = TMath::Sqrt(TMath::Power(gen.Eta() - gens[index_last_ab].Eta(), 2) + TMath::Power(gen.Phi() - gens[index_last_ab].Phi(), 2));

      if (dr < 0.2)
        index_ab_hadrons.push_back(i);
    }
  }

  if (index_b_hadrons.size() == 0 || index_ab_hadrons.size() == 0)
    return -999;

  Gen FirstCopyBHadronFromTop = gens.at(index_b_hadrons.at(0));
  Gen FirstCopyBHadronFromAntiTop = gens.at(index_ab_hadrons.at(0));
  Gen LastCopyTop = gens.at(index_last_t);
  Gen LastCopyAntiTop = gens.at(index_last_at);
  Gen LastCopyWPlus = gens.at(index_last_w_plus);
  Gen LastCopyWMinus = gens.at(index_last_w_minus);

  const float x_e_top = 2 * FirstCopyBHadronFromTop * LastCopyTop / LastCopyTop.M2();
  const float x_e_antitop = 2 * FirstCopyBHadronFromAntiTop * LastCopyAntiTop / LastCopyAntiTop.M2();
  const float w_top = LastCopyWPlus.M2() / LastCopyTop.M2();
  const float w_antitop = LastCopyWMinus.M2() / LastCopyAntiTop.M2();
  const float clip_value = 1.2f;
  xb_top = std::min(x_e_top / (1 - w_top), clip_value);
  xb_antitop = std::min(x_e_antitop / (1 - w_antitop), clip_value);

  return 0;
} // int Val_B_Frag::Get_Xb(const vector<Gen> &gens, float &xb_top, float &xb_antitop)

//////////

void Val_B_Frag::Set_Result_Tree()
{
  for (unsigned int i = 0; i < vec_channel.size(); i++)
  {
    for (unsigned int j = 0; j < vec_syst_type.size(); j++)
    {
      AnalyzerParameter::Syst syst_type = vec_syst_type.at(j);
      param.syst_ = syst_type;

      TTree *result_tree = new TTree("Result_Tree", "Result_Tree");
      result_tree->SetDirectory(dir_syst[i][j]);

      result_tree->Branch("weight_b_tag", &weight_b_tag);
      result_tree->Branch("weight_c_tag", &weight_c_tag);

      result_tree->Branch("weight_el_id", &weight_el_id);
      result_tree->Branch("weight_el_reco", &weight_el_reco);

      result_tree->Branch("weight_hem_veto", &weight_hem_veto);
      result_tree->Branch("weight_lumi", &weight_lumi);
      result_tree->Branch("weight_mc", &weight_mc);

      result_tree->Branch("weight_mu_id", &weight_mu_id);

      result_tree->Branch("weight_mu_iso", &weight_mu_iso);

      result_tree->Branch("weight_pileup", &weight_pileup);

      result_tree->Branch("weight_prefire", &weight_prefire);

      result_tree->Branch("weight_pujet_veto", &weight_pujet_veto);

      result_tree->Branch("weight_sl_trig", &weight_sl_trig);

      result_tree->Branch("weight_top_pt", &weight_top_pt);
      result_tree->Branch("weight_top_pt_mva", &weight_top_pt_mva);

      result_tree->Branch("weight_b_frag_mva_nominal", &weight_b_frag_mva_nominal);
      result_tree->Branch("weight_b_frag_mva_up", &weight_b_frag_mva_up);

      result_tree->Branch("n_jets", &n_sel_jet);
      result_tree->Branch("n_bjets", &n_b_jet);

      result_tree->Branch("met_pt", &met_pt);

      result_tree->Branch("xb_top", &xb_top);
      result_tree->Branch("xb_antitop", &xb_antitop);

      map_result_tree.insert({vec_channel[i] + param.GetSystType(), result_tree});
    } // loop over syst
  } // loop over channel

  return;
} // void Val_B_Frag::Set_Result_Tree()

//////////
