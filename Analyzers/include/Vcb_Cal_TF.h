#ifndef __Vcb_Cal_TF_h__
#define __Vcb_Cal_TF_h__

#include <TMVA/Reader.h>
#include <TDirectory.h>

#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "XYMETCorrection_withUL17andUL18andUL16.h"

#include "AnalyzerCore.h"

#include "Vcb_Def.h"

using namespace std;
using namespace TMath;

class Vcb_Cal_TF : public AnalyzerCore
{
public:
  Vcb_Cal_TF();
  ~Vcb_Cal_TF();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);

protected:
  vector<AnalyzerParameter::Syst> vec_syst_type;

  bool run_mu_ch;
  bool run_el_ch;
  TString channel_name;

  bool run_debug;
  bool run_syst;
  bool run_no_jes_breakdown;

  vector<TString> vec_channel;
  TDirectory **dir_channel; // channel
  TDirectory ***dir_syst;   // channel, syst

  typedef enum cut_flow
  {
    No_Cut,
    Met_Filter,
    Trigger,
    Single_Lepton,
    At_Least_Four_Jets,
    At_Least_Two_B_Tagged,
    MET,
    KF_Pass
  } Cut_Flow;
  int n_cut_flow = Cut_Flow::KF_Pass + 1;

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
  vector<Jet> vec_sel_jet_match;

  vector<float> vec_jet_pt;
  vector<float> vec_jet_eta;
  vector<float> vec_jet_phi;
  vector<float> vec_jet_mass;
  vector<float> vec_jet_bvsc;
  vector<float> vec_jet_cvsb;
  vector<float> vec_jet_cvsl;
  vector<int> vec_jet_flavor;

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

  unsigned int electron_id_bit;

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
  float weight_b_tag_down_hf;
  float weight_b_tag_up_hf;
  float weight_b_tag_down_lf;
  float weight_b_tag_up_lf;
  float weight_b_tag_down_jes;
  float weight_b_tag_up_jes;
  float weight_b_tag_down_lfstats1;
  float weight_b_tag_up_lfstats1;
  float weight_b_tag_down_lfstats2;
  float weight_b_tag_up_lfstats2;
  float weight_b_tag_down_cferr1;
  float weight_b_tag_up_cferr1;
  float weight_b_tag_down_cferr2;
  float weight_b_tag_up_cferr2;
  float weight_b_tag_down_hfstats1;
  float weight_b_tag_up_hfstats1;
  float weight_b_tag_down_hfstats2;
  float weight_b_tag_up_hfstats2;

  float weight_c_tag;
  float weight_c_tag_down_extrap;
  float weight_c_tag_up_extrap;
  float weight_c_tag_down_interp;
  float weight_c_tag_up_interp;
  float weight_c_tag_down_lhe_scale_muf;
  float weight_c_tag_up_lhe_scale_muf;
  float weight_c_tag_down_lhe_scale_mur;
  float weight_c_tag_up_lhe_scale_mur;
  float weight_c_tag_down_ps_fsr_fixed;
  float weight_c_tag_up_ps_fsr_fixed;
  float weight_c_tag_down_ps_isr_fixed;
  float weight_c_tag_up_ps_isr_fixed;
  float weight_c_tag_down_pu;
  float weight_c_tag_up_pu;
  float weight_c_tag_down_stat;
  float weight_c_tag_up_stat;
  float weight_c_tag_down_xsec_brunc_dyjets_b;
  float weight_c_tag_up_xsec_brunc_dyjets_b;
  float weight_c_tag_down_xsec_brunc_dyjets_c;
  float weight_c_tag_up_xsec_brunc_dyjets_c;
  float weight_c_tag_down_xsec_brunc_wjets_c;
  float weight_c_tag_up_xsec_brunc_wjets_c;
  float weight_c_tag_down_jer;
  float weight_c_tag_up_jer;
  float weight_c_tag_down_jes_total;
  float weight_c_tag_up_jes_total;

  float weight_el_id;
  float weight_el_id_down;
  float weight_el_id_up;

  float weight_el_reco;
  float weight_el_reco_down;
  float weight_el_reco_up;

  float weight_hem_veto;
  float weight_lumi;
  float weight_mc;

  float weight_mu_id;
  float weight_mu_id_down;
  float weight_mu_id_up;

  float weight_mu_iso;
  float weight_mu_iso_down;
  float weight_mu_iso_up;

  float weight_pdf_alternative;
  float weight_pdf_error_set[100];
  float weight_pdf_as_down;
  float weight_pdf_as_up;

  float weight_pileup;
  float weight_pileup_down;
  float weight_pileup_up;

  float weight_prefire;
  float weight_prefire_down;
  float weight_prefire_up;

  float weight_ps[4];

  float weight_pujet_veto;
  float weight_pujet_veto_down;
  float weight_pujet_veto_up;

  float weight_scale_variation_1;
  float weight_scale_variation_2;
  float weight_scale_variation_3;
  float weight_scale_variation_4;
  float weight_scale_variation_6;
  float weight_scale_variation_8;

  float weight_sl_trig;
  float weight_sl_trig_down;
  float weight_sl_trig_up;

  float weight_top_pt;

  TString region;

  float ht;
  float mt;

  int decay_mode;
  vector<int> vec_gen_hf_flavour;
  vector<int> vec_gen_hf_origin;

  vector<int> vec_sel_gen_hf_flavour;
  vector<int> vec_sel_gen_hf_origin;

  XYMETCorrection_withUL17andUL18andUL16 xy_met_correction;

  map<TString, TTree *> map_result_tree;

  float Calculate_Mt(const Particle &lepton, const float &neu_px, const float &neu_py);
  void Clear();
  int Chk_Included(const int index_matched_jet[4]);
  void Make_Result_Tree(AnalyzerParameter &param);
  Particle Rebalance_Met();
  void Set_Result_Tree();
};

#endif /* __Vcb_Cal_TF_h__ */
