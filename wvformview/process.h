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
#include <TGraph.h>


/**
Main processing class. Will have to add slot stuff later.

	@author andrzej szelc <szelc@lngs.infn.it>
*/
class process{
public:
    process(int nchans);
    ~process();

int read_channel(int n);
int read_channels(int run,int evt);
void write_ser_windows(int channel);
//int fit_channel(int n);

// void save_results_in_channel(TF1 * func,int channel);

//double fitfunc(double *x, double *par);
channel * get_channel(int n);
channel * get_intchannel(int n);
channel * get_sum_channel(int n);
channel * get_raw_channel(int n);
// int get_fitting_start(TH1F * myhist);

//void set_NGAUSS(int ng);
// void get_params_from_file();
// void get_param_arrays(double *pval,double *plow,double *phigh);
// void set_param_arrays(double *pval,double *plow,double *phigh);

static int NEXP;
static int NGAUSS;


private:
channel ** ser_chan;
channel ** integ_chan;
channel ** sum_chan;
channel ** raw_chan;
const int NCHANNELS;



double *plow;
double *phigh;
double *pval;

TFile * inin;

//TH1F * fHist[38];
TGraph * fHist[38];
TGraph * fIntHist[38];
TGraph * fRawHist[38];
TH1F * fsumHist[3];


//TGraph * fsumHist[3];

void set_function_pars(TF1 * func,double * pval,double * plow,double * phigh);
void Fill_events(int run,int event,char * fname);

};

#endif
