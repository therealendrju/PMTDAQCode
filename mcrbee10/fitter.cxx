#include "TMath.h"
#include "TVirtualFitter.h"
#include "Minuit2/FCNBase.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitter.h"
#include "TFitterMinuit.h"

using namespace std;

#define MY_MAX_NPMT 8

typedef struct {
        int nrun;
        int nevent;
        int evtime;
        int nn;
        float s1;
        float s2;
        int t1;
        int t2;
        int t2last;
        int t2largest;
        int ov;
        float s1intetime;
        int nsec;
        float tm;
        float sf;
        int tp[MY_MAX_NPMT];
        float sp[MY_MAX_NPMT];
        float spf[MY_MAX_NPMT];
        float ss[MY_MAX_NPMT];
        float rtp;
        float rts;
        float fit[6];
} my_event;

extern my_event the_event;
extern float adccsum[];

inline float myabs(float aa) {return (aa<0) ? -aa : aa;}

class MyFCN : public ROOT::Minuit2::FCNBase { 

public: 

  double *data;
  double sample;  // was 0.04 = 40 ns
  int fit_nbin;

  MyFCN(void) {}

  double eval_chi2(const std::vector<double> & x) const {
    Double_t ddata, chi2;
    Double_t time1, time2, p1, p2, mu;
    Double_t xbin[9]={0.000, 0.040, 0.080, 0.160, 0.600, 1.400, 2.400, 3.800, 5.600};
    Int_t    ibin[9]={    0,    40,    80,   160,   600,  1400,  2400,  3800,  5600};

    int neff, offset;

    chi2 = 0.0; neff=0;
    offset = 0;

//     for(int i=0; i<fit_nbin; i++) {
//       time1 = sample*i;
//       time2 = sample*(i+1);
//       p1 = TMath::Exp(-time1/x[1]) - TMath::Exp(-time2/x[1]);
//       p2 = TMath::Exp(-time1/x[2]) - TMath::Exp(-time2/x[2]);
//       mu = x[0]*(x[3]*p1 + (1.0-x[3])*p2);
      
//       if(data[i] > 0.0) {
// 	chi2 += ((mu - data[i])*(mu - data[i]))/mu;
// 	neff++;
//       }

    for(int i=0; i<8; i++) {
      time1 = xbin[i];
      time2 = xbin[i+1];
      p1 = TMath::Exp(-time1/x[1]) - TMath::Exp(-time2/x[1]);
      p2 = TMath::Exp(-time1/x[2]) - TMath::Exp(-time2/x[2]);
      mu = x[0]*(x[3]*p1 + (1.0-x[3])*p2);
      
      ddata = 0.0;
      for(int jj=(the_event.t1+ibin[i]+offset); jj<(the_event.t1+ibin[i+1]+offset); jj++) ddata += adccsum[jj];
      if(ddata > 0.0) {
	chi2 += ((mu - ddata)*(mu - ddata))/mu;
	neff++;
      }
//       cout << (the_event.t1+ibin[i]) << "  " << (the_event.t1+ibin[i+1]) << "  " << mu << "  " << ddata << "  " << neff << endl;
    }

//     for(int jj=the_event.t1-10; jj<the_event.t1+80; jj++) cout << jj << "  " << adccsum[jj] << endl;
    
    return chi2/((double) (neff-4));
   
  } 

  double operator() (const std::vector<double> & x) const {

    double time1, time2, p1, p2, mu;
    double prob;

    prob = 0.0;

    for(int i=0; i<fit_nbin; i++) {
      time1 = sample*i;
      time2 = sample*(i+1);
      p1 = TMath::Exp(-time1/x[1]) - TMath::Exp(-time2/x[1]);
      p2 = TMath::Exp(-time1/x[2]) - TMath::Exp(-time2/x[2]);
      mu = x[0]*(x[3]*p1 + (1.0-x[3])*p2);
      
      prob += mu - data[i]*TMath::Log(mu);
      
    }
    
//     if(iflag == 1) cout << "Prob = " << prob << endl;

    return  prob;

  } 

//   double Up() const { return 1.0; }  // 1.0 for chisquare, 0.5 for likelihood
  double Up() const { return 0.5; }  // 1.0 for chisquare, 0.5 for likelihood

  void set_pointer(double *p) {data = p;}
  void set_sample(double s) {sample = s;}
  void set_nbin(int nn) {fit_nbin = nn;}

};

int istate;

void make_fit(double* dd, double ss, double s1, int nn, float* fit) {

//   const int npar = 5;
//   double p[npar];
  double sum;
  std::vector<double> pp;

  TFitterMinuit * minuit = new TFitterMinuit();

  fit[0] = 0.0;
  fit[1] = 0.0;
  fit[2] = 0.0;
  fit[3] = 0.0;
  fit[4] = 0.0;

  minuit->SetPrintLevel(-1);  // silent....
  //  minuit->SetPrintLevel(2);

  MyFCN fcn;

  fcn.set_pointer(dd);
  fcn.set_sample(ss);
  fcn.set_nbin(nn);

  sum = 0.0;
  std::cout << "fitter: data in first 10 bins = ";
  for(int i=0; i<10; i++) {std::cout << dd[i] << "  "; sum += dd[i];}
  std::cout << std::endl;
  sum *= 3.0;

  minuit->SetMinuitFCN(&fcn);

  minuit->SetParameter(0, "nzero",     s1, 1000.0,   50.0,1000000.0);
  minuit->SetParameter(1, "tfast",     0.01,   0.01,   0.0,      0.07);
  minuit->SetParameter(2, "tslow",     1.4,    0.1,    0.0,      8.0);
  minuit->SetParameter(3, "testp",     0.6,    0.1,    0.0,      1.0);

  // create Minimizer (default is Migrad)

  minuit->CreateMinimizer();

  istate = minuit->Minimize();

  if(istate >= 0) {
    pp.push_back(minuit->GetParameter(0));
    pp.push_back(minuit->GetParameter(1));
    pp.push_back(minuit->GetParameter(2));
    pp.push_back(minuit->GetParameter(3));

    fit[0] = pp[0];
    fit[1] = pp[1];
    fit[2] = pp[2];
    fit[3] = pp[3];
    fit[4] = fcn.eval_chi2(pp);
    std::cout << "fitter: s1     = " << fit[0] << std::endl;
    std::cout << "        T_fast = " << fit[1]*1000 << " ns" << std::endl;
    std::cout << "        T_slow = " << fit[2] << " us" << std::endl;
    std::cout << "        Test_P = " << fit[3] << std::endl;
    std::cout << "      Chi2/dof = " << fit[4] << std::endl;
  }
}
