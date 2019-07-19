/***************************************************************************
 *   Copyright (C) 2009 by andrzej szelc   *
 *   szelc@lngs.infn.it   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PROCESS_H
#define PROCESS_H

#include "channel.h"
#include <fstream>
#include <TFile.h>
#include <TStyle.h>
//#include "../mcrbee10/sql_interface.h"
#include <vector>
#include <string>
#include <map>
/**
Main processing class. Will have to add slot stuff later.

	@author andrzej szelc <szelc@lngs.infn.it>
*/
class process{
public:
    process(int nchans);
    ~process();

int read_channel(int n);
//int read_channels(int run);
int read_channels(int run,std::vector<std::string> filelist);
int fit_channel(int n);
void set_ser_params(std::map<std::string,double> params);
void set_ser_params(float, int, int, int);   //temporary for backwards compatibility


void save_results_in_channel(TF1 * func,int channel);

//double fitfunc(double *x, double *par);
channel * get_channel(int n);
channel * get_ampchannel(int n);
channel * get_lengthchannel(int n);

int get_fitting_start(TH1F * myhist);

void set_NGAUSS(int ng);
void get_params_from_file();
void get_param_arrays(double *pval,double *plow,double *phigh);
void set_param_arrays(double *pval,double *plow,double *phigh);

void save_results_toDB(int runin,int * onoff);
void save_results_to_file(int runin,int onoff[]);


static int NEXP;
static int NGAUSS;


private:
channel ** ser_chan;
channel ** amp_chan;
channel ** leng_chan;
const int NCHANNELS;

//int intStat;  //checks if the root files exist. 0 - root file found, else not found.

double *plow;
double *phigh;
double *pval;

TFile * inin;

//sql_interface *sql;

void set_function_pars(TF1 * func,double * pval,double * plow,double * phigh);
void Fill_events(int run,char * fname,std::vector<std::string> filelist);

};

#endif
