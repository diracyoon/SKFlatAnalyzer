#ifndef __Vcb_Modelling_Patch__
#define __Vcb_Modelling_Patch__

#include "AnalyzerCore.h"

#include "Vcb_Def.h"

class Vcb_Modelling_Patch : public AnalyzerCore
{
public:
  Vcb_Modelling_Patch();
  ~Vcb_Modelling_Patch();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);

protected:
  TTree *result_tree;

  TString central_pdf;
  TString central_new_pdf;

  AnalyzerParameter param;

  float weight;
  float weight_lumi;
  float weight_mc;

  float weight_pdf_alternative;
  float weight_pdf_error_set[100];
  float weight_pdf_as_down;
  float weight_pdf_as_up;

  float weight_top_pt;
  float weight_top_pt_mva;

  float weight_hdamp_mva_down;
  float weight_hdamp_mva_up;

  float weight_b_frag_mva_nominal;
  float weight_b_frag_mva_up;

  float weight_ps[4];

  float weight_scale_variation_1;
  float weight_scale_variation_2;
  float weight_scale_variation_3;
  float weight_scale_variation_4;
  float weight_scale_variation_6;
  float weight_scale_variation_8;

  // int decay_mode;

  void Clear();
  bool Cut_PDF_Weight();
  TString Histo_Name();
  bool IsNaN();
  void Set_Result_Tree();
};

#endif /* __Vcb_Modelling_Patch__ */