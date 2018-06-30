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
#ifndef CHANNEL_H
#define CHANNEL_H

#include "TH1F.h"

/**
This classcorresponds to one channel of the data It should store the histogram, results and such.

	@author andrzej szelc <szelc@lngs.infn.it>
*/


struct results{
double xo;
double xo_err;

double sigma;
double sigma_err;

double N;
double N_err;

double chisq;

results& operator=(const results &rhs)
{
if(this != &rhs)
	{this->xo=rhs.xo;
	this->xo_err=rhs.xo_err;

	this->sigma=rhs.sigma;
	this->sigma_err=rhs.sigma_err;

	this->N=rhs.N;
	this->N_err=rhs.N_err;

	this->chisq=rhs.chisq;
	

	}
return *this;
}

results& operator=(const double rhs)
{

	this->xo=rhs;
	this->xo_err=rhs;

	this->sigma=rhs;
	this->sigma_err=rhs;

	this->N=rhs;
	this->N_err=rhs;

	this->chisq=rhs;

	
return *this;
}





};


class channel{
public:
    channel();
    channel(TObject * ser_chan);

    ~channel();

int set_histo(TObject * ser_chan);
TObject * get_histo();

int write_results(results in);
//int write_results(TF1 * func);
results get_results();


private:
TObject * ser_channel;

results inside;

};

#endif
