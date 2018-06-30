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
#include "channel.h"
#include <iostream>

channel::channel()
{
ser_channel=NULL;
inside=0;
//ser_channel=new TH1F("pmt channel","pmt channel",100,1,100);
}


channel::channel(TObject * in)
{
ser_channel=in;
inside=0;
}


channel::~channel()
{

if(ser_channel!=NULL)
	{delete ser_channel;
	}	

}


int channel::set_histo(TObject * ser_chan)
{
ser_channel=ser_chan;
return 1;
}

TObject * channel::get_histo()
{
return ser_channel;
}


int channel::write_results(results in)
{

inside=in;
return 1;
}




results channel::get_results()
{
return inside;
}









