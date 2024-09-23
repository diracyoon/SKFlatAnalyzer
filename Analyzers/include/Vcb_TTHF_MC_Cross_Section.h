#ifndef __Vcb_TTHF_MC_Cross_Section_h__
#define __Vcb_TTHF_MC_Cross_Section_h__

#include "AnalyzerCore.h"

#include "Vcb_Def.h"

using namespace std;
// using namespace TMath;

class Vcb_TTHF_MC_Cross_Section : public AnalyzerCore
{
public:
  Vcb_TTHF_MC_Cross_Section();
  ~Vcb_TTHF_MC_Cross_Section();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);

protected:
  AnalyzerParameter param;

  vector<Electron> vec_electron;
  vector<Muon> vec_muon;
  vector<Jet> vec_jet;
  vector<Gen> vec_gen;

  vector<Electron> vec_this_electron;
  vector<Muon> vec_this_muon;
  vector<Jet> vec_this_jet;

  vector<TString> vec_el_trig;
  vector<TString> vec_mu_trig;

  TString el_trig;
  TString mu_trig;

  float el_trig_safe_pt_cut;
  float mu_trig_safe_pt_cut;

  bool chk_mu_channel;
  bool chk_el_channel;

  float weight;

  float weight_lumi;
  float weight_mc;
  float weight_pileup;
  float weight_prefire;
  float weight_top_pt;
  float weight_hem_veto;
  float weight_mu_trig;
  float weight_mu_id;
  float weight_mu_iso;
  float weight_el_trig;
  float weight_el_id;
  float weight_el_reco;
  float weight_pujet_veto;
  float weight_c_tag;

  void Clear();
  vector<Electron> Select_Electrons_Iso(vector<Electron> &vec_electron);
  vector<Muon> Select_Muons_Iso(vector<Muon> &vec_muon);
};

#endif /* __Vcb_TTHF_MC_Cross_Section_h__ */
