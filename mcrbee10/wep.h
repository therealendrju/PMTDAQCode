#ifndef _MY_WEP_
#define _MY_WEP_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum TypeIdentifiers {
  M_TYPE_INT,
  M_TYPE_FLOAT
};

class parameter {
 public:
  int    type;
  string param;
  string name;
  parameter(int a, string b, string s) {type = a; param = b; name = s;}
  void print() {cout << type << "\t" << param << "\t" << name << endl;}
};

class wep {

 public:

  bool debug, first_event;
  int *data;
  int nPMT, nSAMPLES;
  int nRUN, nEVENT;
  int vSCALE;
  int event_time;
  int preTRIG;
  float sampling; // in ns
  string filename;
  parameter *p;
  vector<parameter*> parameter_vector;
  vector<parameter*> result_vector;
  vector<int (*)(wep*)> functions;
  int (* start_event) (void);
  int (* end_event) (void);
  vector<bool> isPMTOn;
  vector<double> PMTSer;
  vector<double> PMTSigmaSer;
  vector<double> PMTValley;
  bool is_ser_found;
  bool fUseDatabase;

  bool isSer;
  bool isGUI;
  bool isAverage;
  bool DBread;
  int DBrunread;  

  
  bool is_CAEN;
  //for setting the ser window
  int chan_before;
  int chan_after;
  int integr_window;
  float threshold;
  int onset;
  int duration;
  
  
/*  public: */

  wep(void);
  ~wep(void);

  int open_file(string s);
  int close_file(void);
  int get_event(int run, int event);
  int readevent(int n=0);
  int get_database();
  int get_nsamples(void) {return nSAMPLES;}
  float get_sampling(void) {return sampling;}

  int process_event(void);
  int add_function(int (*)(wep*));
  int set_start(int (*)(void));
  int set_end(int (*)(void));
  int get_result(int ncount, char* names[], ...);

  void add_param(int a, string b, string s);
  void print_param(int i) {parameter_vector[i-1]->print();}
  void get_param(int i, void* r);
  int get_param_type(int i) {return parameter_vector[i-1]->type;}

  void refine_baseline(bool val){fRefineBaseline = val;};
  bool use_refine_baseline(){return fRefineBaseline;};
 
  //added by Andrzej  
  void clear_functions();
  
 private:
  bool fRefineBaseline;

};

#endif
