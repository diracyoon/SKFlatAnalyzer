#include "Vcb_TTHF_MC_Cross_Section.h"

//////////

Vcb_TTHF_MC_Cross_Section::Vcb_TTHF_MC_Cross_Section()
{
} // Vcb_TTHF_MC_Cross_Section::Vcb_TTHF_MC_Cross_Section()

//////////

Vcb_TTHF_MC_Cross_Section::~Vcb_TTHF_MC_Cross_Section()
{
} // Vcb_TTHF_MC_Cross_Section::~Vcb_TTHF_MC_Cross_Section()

//////////

void Vcb_TTHF_MC_Cross_Section::initializeAnalyzer()
{
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

  return;
} // void Vcb_TTHF_MC_Cross_Section::initializeAnalyzer()

//////////

void Vcb_TTHF_MC_Cross_Section::executeEvent()
{
  vec_muon.clear();
  vec_electron.clear();
  vec_jet.clear();
  vec_gen.clear();

  vec_muon = GetAllMuons();
  vec_electron = GetAllElectrons();
  vec_jet = GetAllJets();
  vec_gen = GetGens();

  param.Clear();

  param.Muon_Tight_ID = "POGTight";
  param.Muon_Loose_ID = "POGLoose";

  param.Muon_ID_SF_Key = "NUM_TightID_DEN_TrackerMuons";
  param.Muon_ISO_SF_Key = "NUM_TightRelIso_DEN_TightIDandIPCut";

  param.Electron_Tight_ID = "passMVAID_noIso_WP80";
  param.Electron_Loose_ID = "passMVAID_noIso_WP90";

  param.Jet_ID = "tight";
  param.PUJet_Veto_ID = "LoosePileupJetVeto";

  executeEventFromParameter(param);

  return;
} // void Vcb_TTHF_MC_Cross_Section::executeEvent()

//////////

void Vcb_TTHF_MC_Cross_Section::executeEventFromParameter(AnalyzerParameter param)
{
  Clear();

  Event ev = GetEvent();

  // lumi
  weight_lumi = ev.GetTriggerLumi("Full");
  weight *= weight_lumi;

  // MCweight +1 or -1
  weight_mc = MCweight();
  weight *= weight_mc;

  // // pileup
  // weight_pileup = mcCorr->GetPileUpWeight(nPileUp, 0);
  // weight *= weight_pileup;

  // // L1 prefire
  // weight_prefire = GetPrefireWeight(0);
  // weight *= weight_prefire;

  // // Top Pt reweight
  // weight_top_pt = mcCorr->GetTopPtReweight(vec_gen);
  // weight *= weight_top_pt;

  //////////////////
  /* No Selection */
  //////////////////

  vec_this_muon = vec_muon;
  vec_this_electron = vec_electron;
  vec_this_jet = vec_jet;

  int chk_b = 0;
  int chk_c = 0;

  for (unsigned int i = 0; i < vec_this_jet.size(); i++)
  {
    Jet jet = vec_this_jet[i];

    if (jet.Pt() < 20)
      continue;

    if (2.4 < abs(jet.Eta()))
      continue;

    int flavour = jet.GenHFHadronMatcherFlavour();
    int origin = jet.GenHFHadronMatcherOrigin();

    if (flavour == 5 && abs(origin) == 21)
      // if (flavour == 5 && abs(origin) != 6 && abs(origin) != 24)
      chk_b++;
    else if (flavour == 4 && abs(origin) == 21)
      // else if (flavour == 4 && abs(origin) != 6 && abs(origin) != 24)
      chk_c++;
  }

  int index;
  if (1 < chk_b)
    index = 2;
  else if (chk_b == 0 && 1 < chk_c)
    index = 1;
  else
    index = 0;

  FillHist("No_Baseline_Selection_Two", index, weight, 3, 0, 3);

  if (chk_b)
    index = 2;
  else if (chk_b == 0 && chk_c)
    index = 1;
  else
    index = 0;

  FillHist("No_Baseline_Selection_One", index, weight, 3, 0, 3);

  ////////////////////////
  /* Baseline Selection */
  ////////////////////////

  // for lepton selection
  vector<Muon> vec_sel_muon = SelectMuons(vec_this_muon, param.Muon_Tight_ID, mu_trig_safe_pt_cut, MUON_ETA);
  vector<Electron> vec_sel_electron = SelectElectrons(vec_this_electron, param.Electron_Tight_ID, el_trig_safe_pt_cut, ELECTRON_ETA);

  vec_sel_muon = Select_Muons_Iso(vec_sel_muon);
  vec_sel_electron = Select_Electrons_Iso(vec_sel_electron);

  // for lepton veto
  vector<Muon> vec_muon_veto = SelectMuons(vec_this_muon, param.Muon_Loose_ID, MUON_PT_VETO, MUON_ETA);
  vector<Electron> vec_electron_veto = SelectElectrons(vec_this_electron, param.Electron_Loose_ID, ELECTRON_PT_VETO, ELECTRON_ETA);

  // cut on lepton
  if (vec_sel_muon.size() == 1 && vec_muon_veto.size() == 1 && vec_electron_veto.size() == 0)
    chk_mu_channel = true;
  else if (vec_muon_veto.size() == 0 && vec_sel_electron.size() == 1 && vec_electron_veto.size() == 1)
    chk_el_channel = true;
  else
    return;

  if (chk_mu_channel)
  {
    Muon muon = vec_sel_muon[0];

    // trig safe cut
    if (muon.Pt() < mu_trig_safe_pt_cut)
      return;

    // SF for muon trig
    weight_mu_trig = mcCorr->MuonTrigger_SF("POGTight", mu_trig, vec_sel_muon, 0);
    weight *= weight_mu_trig;

    // SF for muon id
    weight_mu_id = mcCorr->MuonID_SF(param.Muon_ID_SF_Key, muon.Eta(), muon.MiniAODPt(), 0);
    weight *= weight_mu_id;

    // SF for muon iso
    weight_mu_iso = mcCorr->MuonISO_SF(param.Muon_ISO_SF_Key, muon.Eta(), muon.MiniAODPt(), 0);
    weight *= weight_mu_iso;
  }
  else if (chk_el_channel)
  {
    Electron electron = vec_sel_electron[0];

    // trig safe cut
    if (vec_sel_electron[0].Pt() < el_trig_safe_pt_cut)
      return;

    // SF for electron trig
    weight_el_trig = mcCorr->ElectronTrigger_SF(param.Electron_Tight_ID, el_trig, vec_sel_electron, 0);
    weight *= weight_el_trig;

    // SF for electron id
    weight_el_id = mcCorr->ElectronID_SF(param.Electron_Tight_ID, electron.scEta(), electron.UncorrPt(), 0);
    weight *= weight_el_id;

    // SF for electron Reco eff
    weight_el_reco = mcCorr->ElectronReco_SF(electron.scEta(), electron.UncorrPt(), 0);
    weight *= weight_el_reco;
  }

  // Jet selection
  float jet_eta_cut = 999;
  if (DataYear == 2016)
    jet_eta_cut = JET_ETA_2016;
  else if (DataYear == 2017 || DataYear == 2018)
    jet_eta_cut = JET_ETA;

  vector<Jet> vec_sel_jet = SelectJets(vec_this_jet, param.Jet_ID, JET_PT, jet_eta_cut);
  vec_sel_jet = SelectJets(vec_sel_jet, param.PUJet_Veto_ID, JET_PT, jet_eta_cut);
  vec_sel_jet = JetsVetoLeptonInside(vec_sel_jet, vec_electron_veto, vec_muon_veto, DR_LEPTON_VETO);
  int n_sel_jet = vec_sel_jet.size();

  // sort jet as pt ordering
  sort(vec_sel_jet.begin(), vec_sel_jet.end(), PtComparing);

  // cut on jet
  // at least four jets
  if (n_sel_jet < 4)
    return;

  // minimum pt cut on leading jet
  if (vec_sel_jet[0].Pt() < LEADING_JET_PT)
    return;

  // minimum pt cut on subleading jet
  if (vec_sel_jet[1].Pt() < SUBLEADING_JET_PT)
    return;

  // at least two b-tag jet
  int n_b_jet = 0;
  for (auto &jet : vec_sel_jet)
  {
    float tagging_score = jet.GetTaggerResult(JetTagging::DeepJet);

    if (mcCorr->GetJetTaggingCutValue(JetTagging::DeepJet, JetTagging::Medium) < tagging_score)
      n_b_jet++;
  }

  if (n_b_jet < 2)
    return;

  // hem veto
  weight_hem_veto = Weight_HEM_Veto(vec_sel_jet);
  weight *= weight_hem_veto;

  // SF for PUJet Veto
  weight_pujet_veto = mcCorr->PileupJetVeto_Reweight(vec_sel_jet, param.PUJet_Veto_ID, 0);
  weight *= weight_pujet_veto;

  // met filter
  if (!PassMETFilter())
    return;

  Particle met = ev.GetMETVector("PUPPI");

  float met_pt = met.Pt();
  if (met_pt < MET_PT)
    return;

  chk_b = 0;
  chk_c = 0;

  for (unsigned int i = 0; i < vec_sel_jet.size(); i++)
  {
    Jet jet = vec_sel_jet[i];

    int flavour = jet.GenHFHadronMatcherFlavour();
    int origin = jet.GenHFHadronMatcherOrigin();

    // if (flavour == 5 && abs(origin) == 21)
    if (flavour == 5 && abs(origin) != 6 && abs(origin) != 24)
    {
      chk_b++;
      break;
    }
    // else if (flavour == 4 && abs(origin) == 21)
    else if (flavour == 4 && abs(origin) != 6 && abs(origin) != 24)
      chk_c++;
  }

  if (chk_b)
    index = 2;
  else if (chk_c)
    index = 1;
  else
    index = 0;

  FillHist("Baseline_Selection", index, weight, 3, 0, 3);

  return;
} // Vcb_TTHF_MC_Cross_Section::executeEventFromParameter(AnalyzerParameter param)

//////////

void Vcb_TTHF_MC_Cross_Section::Clear()
{
  weight = 1;

  weight_lumi = 1;
  weight_mc = 1;
  weight_pileup = 1;
  weight_prefire = 1;
  weight_top_pt = 1;
  weight_hem_veto = 1;
  weight_mu_trig = 1;
  weight_mu_id = 1;
  weight_mu_iso = 1;
  weight_el_trig = 1;
  weight_el_id = 1;
  weight_el_reco = 1;
  weight_pujet_veto = 1;
  weight_c_tag = 1;

  chk_mu_channel = false;
  chk_el_channel = false;

  return;
} // void Vcb_TTHF_MC_Cross_Section::Clear()

//////////

vector<Electron> Vcb_TTHF_MC_Cross_Section::Select_Electrons_Iso(vector<Electron> &vec_electron)
{
  vector<Electron> vec_out;
  for (unsigned int i = 0; i < vec_electron.size(); i++)
  {
    Electron electron = vec_electron[i];

    float rel_iso_electron_a;
    float rel_iso_electron_b;
    if (TMath::Abs(electron.Eta()) < 1.479)
    {
      rel_iso_electron_a = REL_ISO_ELECTRON_BARREL_A;
      rel_iso_electron_b = REL_ISO_ELECTRON_BARREL_B;
    }
    else
    {
      rel_iso_electron_a = REL_ISO_ELECTRON_ENDCAP_A;
      rel_iso_electron_b = REL_ISO_ELECTRON_ENDCAP_B;
    }

    if (electron.RelIso() < rel_iso_electron_a + rel_iso_electron_b / electron.UncorrPt())
      vec_out.push_back(electron);
  }

  return vec_out;
} // vector<Electron> Vcb_TTHF_MC_Cross_Section::Select_Electron_Iso(vector<Electron>& vec_electron)

//////////

vector<Muon> Vcb_TTHF_MC_Cross_Section::Select_Muons_Iso(vector<Muon> &vec_muon)
{
  vector<Muon> vec_out;
  for (unsigned int i = 0; i < vec_muon.size(); i++)
  {
    Muon muon = vec_muon[i];

    if (muon.RelIso() < REL_ISO_MUON)
      vec_out.push_back(muon);
  }

  return vec_out;
} // vector<muon> Vcb_TTHF_MC_Cross_Section::Select_Muon_Iso(vector<Muon>& vec_muon)

//////////