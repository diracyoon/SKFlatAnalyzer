#include "GetEffLumi.h"

void GetEffLumi::initializeAnalyzer()
{
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("IsData", 1);
  if (!IsDATA)
  {
    fChain->SetBranchStatus("gen_weight", 1);
    if (fChain->GetBranch("weight_Scale"))
      fChain->SetBranchStatus("weight_Scale", 1);
    // fChain->SetBranchStatus("genTtbarId", 1);
  }
}

void GetEffLumi::executeEvent()
{
  double weight = 1.;
  double weight_sign = 1.;
  if (!IsDATA)
  {
    weight = MCweight(false, false);
    weight_sign = MCweight(true, false);

    //cout << "test " << weight << " " << weight_sign << endl;
  }

  // if (genTtbarId % 100 == 56 || genTtbarId % 100 == 46)
  //   cout << "test " << genTtbarId << endl;

  FillHist("sumW", 0, weight, 1, 0., 1.);
  FillHist("sumSign", 0, weight_sign, 1, 0., 1.);
  // if(!IsDATA && weight_Scale->size()>0)
  //   {
  // for(int i=0,n=weight_Scale->size();i<n;i++)
  //{
  //   FillHist("sumW_Reweight", i, weight*weight_Scale->at(i), n, 0, n);
  // }
  //}

  return;
}
