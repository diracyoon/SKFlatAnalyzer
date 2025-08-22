#ifndef __Vcb_Modelling_Patch__
#define __Vcb_Modelling_Patch__

#include "AnalyzerCore.h"

class Vcb_Modelling_Patch : public AnalyzerCore
{
public:
  Vcb_Modelling_Patch();
  ~Vcb_Modelling_Patch();

  void initializeAnalyzer();
  void executeEvent();
  void executeEventFromParameter(AnalyzerParameter param);

protected:
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

  float weight_ps[4];

  float weight_scale_variation_1;
  float weight_scale_variation_2;
  float weight_scale_variation_3;
  float weight_scale_variation_4;
  float weight_scale_variation_6;
  float weight_scale_variation_8;

  bool Cut_PDF_Weight();
  bool IsNaN();
};

#endif /* __Vcb_Modelling_Patch__ */