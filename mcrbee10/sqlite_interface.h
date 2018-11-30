#ifndef __MY_SQLITE__
#define __MY_SQLITE__ 

#include <sqlite3.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "results.h"
//extern struct MYSQL;
//struct st_mysql;

class sqlite_interface
{

	private:
		sqlite3 * fConn;

		int get_last_ser_entry();
		int get_last_param_entry();

		
	public:
		sqlite_interface(char * DBname);
		//int get_pmt_sers(double * sers,int * onoff,int nofmpts,int nrun,int vscale=0);
		//int get_pmt_sers_with_vscale(double * sers,int * onoff,int nofpmts,int nrun,int vscale);
		//int get_pmt_sigmas(double * sigmas,int nofpmts,int nrun);
		int get_pmt_results(results * sers,int *onoff,int nofpmts,int nrun);		
		static int getpmt_callback(void* data, int argc, char** argv, char** azColName);


		int insert_pmt_sers(double * sers,int * onoff,int nofmpts,int nrun,int flag,int scale);
		int insert_pmt_sers(results * sers,int * onoff,int nofmpts,int nrun,int flag,int scale);
		int insert_ser_parameters(int runin,float threshold,int integr_window, int chan_before, int chan_after);
		
		int check_ser_entry_exists(int nrun,int * ser_id);
		int check_run_entry_exists(int nrun,int * ser_id);
		int check_ser_parameters_exists(int nrun);
		static int check_callback(void* data, int argc, char** argv, char** azColName);

		int insert_new_run(int nrun,int nofpmts,int scale);
		int insert_new_sers_entry(int ser_id,int nofpmts);
		int insert_new_ser_parameters_entry(int run_id);
		
		
		
		int get_ser_parameter_results(int nrun,float *pars);
		static int getparameter_callback(void* data, int argc, char** argv, char** azColName);			

		~sqlite_interface();


		
		
		


};


#endif
