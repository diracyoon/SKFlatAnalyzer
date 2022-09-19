#ifndef __Vcb_Mu_h__
#define __Vcb_Mu_h__

#include <TMVA/Reader.h>

#include "AnalyzerCore.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include "TKinFitterDriver.h"
#include "Vcb_Def.h"
#include "Results_Container.h"

#include "XYMETCorrection_withUL17andUL18andUL16.h"

using namespace std;
using namespace TMath;

class Vcb_Mu : public AnalyzerCore 
{
 public:
  Vcb_Mu();
  ~Vcb_Mu();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);
    
 protected:
  bool run_debug;
  bool run_reco_eval;
  bool run_permutation_tree;
  bool run_hf_contamination_tree;
  bool run_chi;
  bool run_template;
  bool run_template_truth;
  bool run_result;
  bool run_syst;
  bool rm_wm_constraint;
  bool rm_bjet_energy_reg_nn;

  typedef enum cut_flow {No_Cut, Met_Filter, Trigger, Single_Lepton, At_Least_Four_Jets, At_Least_Two_B_Tagged, MET, KF_Pass} Cut_Flow;
  int n_cut_flow = Cut_Flow::KF_Pass+1;

  vector<TString> vec_mu_id;
  vector<TString> vec_mu_id_sf_key;
  vector<TString> vec_mu_iso_sf_key;
  vector<TString> vec_mu_trig;
  TString mu_trig;
  float trig_safe_pt_cut;

  vector<JetTagging::Parameters> vec_jet_tagging_para;

  JME::JetResolution jet_resolution;
  JME::JetResolutionScaleFactor jet_resolution_sf;

  TKinFitterDriver* fitter_driver;

  vector<Muon> vec_muon;
  vector<Electron> vec_electron;
  vector<Jet> vec_jet;
   
  bool chk_reco_correct;
  bool chk_included;
  bool chk_matched_jets_only;
  
  bool pu_conta_had_t_b;
  bool pu_conta_w_u;
  bool pu_conta_w_d;
  bool pu_conta_lep_t_b;

  int n_sel_jet;
  int n_b_jet;
  int n_c_jet;
  int n_matched_jets;

  int n_jet_bin;
  int n_bjet_bin;
  
  float n_b_jet_f;
  float n_c_jet_f;

  //Particle met;

  float lepton_pt;
  float lepton_eta;
  
  float met_pt;
  float met_phi;

  float sf_b_tag;
  float sf_c_tag;
  float sf_mu_id;
  float sf_mu_iso;
  float sf_mu_trig;
  float sf_pujet_veto;

  float weight;
  float weight_lumi;
  float weight_mc;
  float weight_pileup;
  float weight_prefire;

  float pt_leading_jet;
  float pt_subleading_jet;
  
  float eta_leading_jet;
  float eta_subleading_jet;
  
  float pt_had_t_b;
  float pt_w_u;
  float pt_w_d;
  float pt_lep_t_b;
  
  float del_phi_w_u_w_d;
  float del_phi_had_w_had_t_b;
  float del_phi_lep_neu;
  float del_phi_lep_w_lep_t_b;
  float del_phi_had_t_lep_t;
  
  float del_eta_w_u_w_d;
  float del_eta_had_w_had_t_b;
  float del_eta_lep_neu;
  float del_eta_lep_w_lep_t_b;
  float del_eta_had_t_lep_t;

  float del_r_w_u_w_d;
  float del_r_had_w_had_t_b;
  float del_r_lep_neu;
  float del_r_lep_w_lep_t_b;
  float del_r_had_t_lep_t;

  float theta_w_u_w_d;
  float theta_had_w_had_t_b;
  float theta_lep_neu;
  float theta_lep_w_lep_t_b;
  float theta_had_t_lep_t;
  
  float theta_w_u_b;
  float theta_w_d_b;
  float theta_p_had_w;
  float theta_b_b;
  float theta_c_c;

  float w_u_b_bscore;
  float w_d_b_bscore;
  
  float m_w_u_b;
  float m_w_d_b;

  float had_t_mass;
  float had_w_mass;
  float lep_t_mass;
  float lep_t_partial_mass;

  float chi2_jet_had_t_b;
  float chi2_jet_w_u;
  float chi2_jet_w_d;
  float chi2_jet_lep_t_b;

  float chi2_jet_extra;

  float chi2_constraint_had_t;
  float chi2_constraint_had_w;
  float chi2_constraint_lep_t;
  float chi2_constraint_lep_w;
 
  float chi2;

  float gen_neutrino_px;
  float gen_neutrino_py;
  float gen_neutrino_pz;
  float met_px;
  float met_py;
  float met_rebalance_px;
  float met_rebalance_py;
  float neutrino_pz_sol;
  float neutrino_pz_sol_unrebal;
  float mt_gen;
  float mt_met;
  float mt_met_rebalance;

  bool chk_real_neu_pz;
  float nu_pz_sol_0;
  float nu_pz_sol_1;

  TString region;

  TTree* permutation_tree_correct;
  TTree* permutation_tree_wrong;

  TTree* hf_contamination_tree_correct;
  TTree* hf_contamination_tree_wrong;

  float best_chi2;
  float best_mva_score_pre;
  float best_mva_score;
  float mt;
  float mva_hf_score;

  bool chk_hf_contamination;
  
  TTree* reco_eval_tree_correct;
  TTree* reco_eval_tree_wrong;

  int decay_mode;
  
  float bvsc_had_t_b;
  float cvsb_had_t_b;
  float cvsl_had_t_b;
  
  float bvsc_w_u;
  float cvsb_w_u;
  float cvsl_w_u;
  float m_w_u;
  
  float bvsc_w_d;
  float cvsb_w_d;
  float cvsl_w_d;
  float m_w_d;

  float bvsc_lep_t_b;
  float cvsb_lep_t_b;
  float cvsl_lep_t_b;

  int swapped_truth;
  int swapped_mva;
  
  XYMETCorrection_withUL17andUL18andUL16 xy_met_correction;

  TTree* template_tree[5];
  TTree* template_truth_tree[5];
  TTree* result_tree;

  TMVA::Reader* reader_swapper[2];
  TMVA::Reader* reader_hf_contamination_lessthantwo;
  TMVA::Reader* reader_hf_contamination_morethantwo;

  float Calculate_Mt(const Particle& lepton, const float& neu_px, const float& neu_py);
  int Chk_Included(const int index_matched_jet[4]);
  bool Compare_Jet(const Jet& jet0, const Jet& jet1);
  int Compare_Jet_Pair(const Jet jet0[2], const Jet jet1[2]);
  void Gen_Match_Residual(const vector<Jet>& vec_jet, const vector<Gen>& vec_gen, const vector<int>& vec_hf_flavour, const vector<int>& vec_hf_origin, const vector<float>& vec_jer, int index_gen[4], int index_matched_jet[4], bool surely_matched[4], float dr_return[4]);
  void Gen_Match_TT(const vector<Jet>& vec_jet, const vector<Gen>& vec_gen, const vector<int>& vec_hf_flavour, const vector<int>& vec_hf_origin, const vector<float>& vec_jer, int index_gen[4], int index_matched_jet[4], bool surely_matched[4], float dr_return[4]);
  int Gen_Match_W(const vector<Jet>& vec_jet, const vector<Gen>& vec_gen, const vector<int>& vec_hf_flavour, const vector<int>& vec_hf_origin, const vector<float>& vec_jer, int index_gen[2], int index_matched_jet[2], bool surely_matched[2], float dr_return[2]);
  int Get_W_Decay_Mode(const vector<Gen>& vec_gen);
  void Index_Converter(const vector<Jet>& vec_sel_jet, const vector<Jet>& vec_sel_jet_match, const int index_matched_jet_match[4], int index_matched_jet[4]);
  //void Index_Restorer(int& index_had_t_b, int& index_w_u, int& index_w_d, int& index_lep_t_b);
  void KF_Ambiguity_Remover(const vector<Jet>& vec_sel_jet, const int index_matched_jet[4]);
  Gen Neutrino(const vector<Gen>& vec_gen);
  void Region_Setter();
  void Sol_Neutrino_Pz(const Particle& lepton, const Particle& met, float neutrino_pz_sol[2]);
};

#endif /* __Vcb_Mu_h__ */

