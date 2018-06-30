#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fstream>

#include "wep.h"
#include "processor_code.h"

int runsel(const struct dirent *a) {
  if(strncmp("Run",a->d_name,3) == 0) return 1;
  return 0;
}

int main(int argc, char **argv) {

  int ifile, iargc, ret;

  char filename[400];
  struct dirent **namelist;
  int i,n;
  
  wep ppp;
  
  init(argv[1],&ppp);

  ppp.debug = false;

  ppp.set_start(start_ev);
  ppp.add_function(get_baseline);
  ppp.add_function(process_pulse);
 // ppp.add_function(fit_s1);
  ppp.add_function(fill_values);
  ppp.set_end(end_ev);

  ifile = 2;
  iargc = argc-2;

  n = scandir(argv[2], &namelist, runsel, alphasort);
  if (n < 0)
    do {
      ppp.open_file(argv[ifile++]);
      ppp.get_database();
      do {
	ret = ppp.readevent();
	if(ret == 0) ppp.process_event();
      } while (ret >= 0);
      ppp.close_file();
    } while(--iargc > 0);
  else {
    for(i=0; i<n; i++) {
      strcpy(filename, argv[2]);
      strcat(filename, "/");
      strcat(filename, namelist[i]->d_name);
//       std::cout << filename << " ------ " << namelist[i]->d_name << endl;
      ppp.open_file(filename);
      ppp.get_database();
      do {
	ret = ppp.readevent();
	if(ret == 0) ppp.process_event();
      } while (ret >= 0);
      ppp.close_file();
      free(namelist[i]);
    }
    free(namelist);
  }

  term(&ppp);

  return 0;
}
  
