#ifndef __Val_B_Frag_h__
#define __Val_B_Frag_h__

#include <TDirectory.h>

#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "XYMETCorrection_withUL17andUL18andUL16.h"

#include "AnalyzerCore.h"

#include "Vcb_Def.h"

using namespace std;
using namespace TMath;

class Val_B_Frag : public AnalyzerCore
{
public:
  Val_B_Frag();
  ~Val_B_Frag();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);

protected:
  vector<AnalyzerParameter::Syst> vec_syst_type;

  bool run_mu_ch;
  bool run_el_ch;
  TString channel_name;

  vector<TString> vec_channel;
  TDirectory **dir_channel; // channel
  TDirectory ***dir_syst;   // channel, syst

  int era_index;

  vector<TString> vec_mu_id;
  vector<TString> vec_mu_id_sf_key;
  vector<TString> vec_mu_iso_sf_key;

  // vector<TString> vec_el_id;
  // vector<TString> vec_el_id_sf_key;

  vector<TString> vec_el_trig;
  vector<TString> vec_mu_trig;
  // vector<TString> vec_sl_trig; // single lepton

  TString el_trig;
  TString mu_trig;
  // TString sl_trig;

  float el_trig_safe_pt_cut;
  float mu_trig_safe_pt_cut;
  // float sl_trig_safe_pt_cut;

  vector<JetTagging::Parameters> vec_jet_tagging_para;

  JME::JetResolution jet_resolution;
  JME::JetResolutionScaleFactor jet_resolution_sf;

  vector<Muon> vec_muon;
  vector<Electron> vec_electron;
  vector<Lepton> vec_lepton;
  vector<Jet> vec_jet;

  vector<Muon> vec_this_muon;
  vector<Electron> vec_this_electron;
  vector<Jet> vec_this_jet;

  vector<Jet> vec_sel_jet;

  vector<bool> vec_btag;
  vector<bool> vec_btag_match;
  vector<bool> vec_ctag;

  Muon muon;
  Electron electron;
  Lepton lepton;
  Particle met;

  int index_matched_jet[4];
  int index_matched_jet_match[4];
  vector<int> vec_index_matched_jet;

  AnalyzerParameter param;

  bool chk_reco_correct;
  bool chk_included;
  bool chk_gentau_conta;
  bool chk_tau_conta;
  bool chk_matched_jets_only;

  bool pu_conta_had_t_b;
  bool pu_conta_w_u;
  bool pu_conta_w_d;
  bool pu_conta_lep_t_b;

  float lepton_pt;
  float lepton_pt_uncorr;
  float lepton_eta;
  float lepton_phi;
  float lepton_rel_iso;

  int n_sel_jet;
  int n_b_jet;
  int n_c_jet;
  int n_matched_jets;

  int n_jet_bin;
  int n_bjet_bin;

  float n_b_jet_f;
  float n_c_jet_f;

  float met_pt;
  float met_phi;

  float pt_ratio;

  float charge_leading_jet;
  float charge_subleading_jet;

  float pt_leading_jet;
  float pt_subleading_jet;

  float eta_leading_jet;
  float eta_subleading_jet;

  float bvsc_leading_jet;
  float cvsb_leading_jet;
  float cvsl_leading_jet;

  float bvsc_subleading_jet;
  float cvsb_subleading_jet;
  float cvsl_subleading_jet;

  bool chk_real_neu_pz;
  float nu_pz_sol_0;
  float nu_pz_sol_1;

  float weight;

  float weight_b_tag;


  float weight_c_tag;

  float weight_el_id;

  float weight_el_reco;

  float weight_hem_veto;
  float weight_lumi;
  float weight_mc;

  float weight_mu_id;


  float weight_mu_iso;

  float weight_pileup;
  float weight_pileup_down;
  float weight_pileup_up;

  float weight_prefire;

  float weight_pujet_veto;

  float weight_sl_trig;


  float weight_top_pt;
  float weight_top_pt_mva;

  float weight_b_frag_mva_nominal;
  float weight_b_frag_mva_up;

  TString region;

  float ht;
  float mt;

  float xb_top;
  float xb_antitop;

  int decay_mode;
  vector<int> vec_gen_hf_flavour;
  vector<int> vec_gen_hf_origin;

  vector<int> vec_sel_gen_hf_flavour;
  vector<int> vec_sel_gen_hf_origin;

  XYMETCorrection_withUL17andUL18andUL16 xy_met_correction;

  map<TString, TTree *> map_result_tree;

  void Clear();
  int Get_Xb(const vector<Gen> &gens, float &xb_top, float &xb_antitop);
  void Set_Result_Tree();
};

#endif /* __Val_B_Frag_h__ */
