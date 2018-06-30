#include <stdlib.h>
#include <fstream>

#include "wed.h"
#include "wep.h"

#include "processor_code.h"

ofstream fout;

int main(int argc, char **argv) {

  fout.open("analysis.out",ios::out | ios::app);

  wep ppp;

  ppp.set_start(start_ev);

  ppp.add_function(get_baseline);
  ppp.add_function(process_pulse);
//   ppp.add_function(fit_s1);

//    ppp.add_function(dump_files);
   //ppp.add_function(get_singlephe);

  //ppp.add_function(recog_peak);
  //ppp.add_function(eval_s1_nofield);

  //  ppp.set_end(end_ev);

  TApplication theApp("App", &argc, argv);

  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  WEDMainFrame mainWindow(&ppp, gClient->GetRoot(), 600, 600);

  theApp.Run();

  fout.close();

  return 0;
}
