#include "sqlite_interface.h"
#include <cstdio>


sqlite_interface::sqlite_interface(char * DBname)
{
	//static MYSQL *conn;

	//check if host is not the one on which the database is setup.
	
	
	//char *user = "phpin";
	//char *password = "wimp2005";
	//char *database = DBname;
	char *zErrMsg = 0;
	int rc;

	/* Open database */
	rc = sqlite3_open(DBname, &fConn);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

}



sqlite_interface::~sqlite_interface()
{
mysql_close(fConn);

}


int sqlite_interface::get_pmt_sers(double * sers,int * onoff,int nofpmts,int nrun,int vscale)
{
//return 0;

//default option - uses the sers for this exact run, with no use of vscale. 
    if(!vscale || vscale==50)
	{
	
   	/* Create SQL statement */
	char *zErrMsg = 0;	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT pmt_b,pmt_a,sers.onflag FROM sers WHERE sers.run_nr = %d ORDER BY pmt",nrun );
	//std::cout << query << std::endl;

	/* Execute SQL statement */
          rc = sqlite3_exec(db, query, callback, (void*)data, &zErrMsg);
   
   	if( rc != SQLITE_OK ) {
      	fprintf(stderr, "SQL error: %s\n", zErrMsg);
      	sqlite3_free(zErrMsg);
	} else {
	fprintf(stdout, "Operation done successfully\n");
	}


	//std::cout << " in ser data" << std::endl;
	int exists=0;
	char query[300];
	
	
	




	// get results
	res = mysql_use_result(fConn);
	int i=0;
	while ( (row = mysql_fetch_row(res)) != NULL)
	{ exists=1;
	
	//default values stored in DB give errors:
	//if(!atof(row[0]) || (!atof(row[1])) )
	//	{return 0;}

	//sanity check
	if(i>=(nofpmts))
		break;	

	sers[i]=( atof(row[0])+atof(row[1]) )/2;
	onoff[i]=atoi(row[2]);
	
	
//	std::cout<< i << " " << atof(row[0]) << " " << atof(row[1]) << " " << atoi(row[2] )<< " " <<onoff[i] << std::endl;
	i++;	

	
	}
	mysql_free_result(res);


	//for(int i=0;i<nofpmts)
	
	if(!exists)
	{
	  //std::cout << "no entry in DB" <<std::endl;
		return 0;
	}
}
else
	{
	int exists=get_pmt_sers_with_vscale(sers,onoff,nofpmts,nrun,vscale);
	
	
	return exists;
	
	}	

//std::cout << "Returning in the standard manner" << std::endl;
	return 1;	
	
	
}



int sqlite_interface::get_pmt_sigmas(double * sigmas,int nofpmts,int nrun)
{
//return 0;

//default option - uses the sers for this exact run, with no use of vscale. 
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	//std::cout << " in ser data" << std::endl;
	int exists=0;
	char query[300];
	
	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT sigma FROM runs, sers WHERE sers.run_nr=runs.run_nr AND sers.run_nr = %d ORDER BY pmt",nrun );
	//std::cout << query << std::endl;

	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}


	// get results
	res = mysql_use_result(fConn);
	int i=0;
	while ( (row = mysql_fetch_row(res)) != NULL)
		{ exists=1;
	//default values stored in DB give errors:
	//if(!atof(row[0]) || (!atof(row[1])) )
	//	{return 0;}
		//sanity check
		if(i>=(nofpmts))
			break;

		sigmas[i]= atof(row[0]);
		//onoff[i]=atoi(row[2]);
		i++;
	
	//	std::cout << atof(row[0]) << " " << std::endl;
	
	
		}
	mysql_free_result(res);


	//for(int i=0;i<nofpmts)
	
	if(!exists)
	{
  //std::cout << "no entry in DB" <<std::endl;
		return 0;
	}

return 1;	
	
	
}



int sqlite_interface::get_pmt_results(results * sers,int * onoff,int nofpmts,int nrun)
{

      MYSQL_RES *res;
        MYSQL_ROW row;
        
//std::cout << "*** mysql add " << res << std::endl; 

        char query[500];
        
        //creating query to read ser values     
        
        //loop on pmts selected
       
                sprintf(query," ");
                sprintf(query,"SELECT pmt_b, pmt_b_err, sigma, sigma_err, ampl, ampl_err,chisq,exp_t,exp_a,valley,val_of_peak,val_of_val,onflag FROM  sers WHERE run_nr = %d ORDER BY pmt",nrun );
                std::cout << query << std::endl;
        //execute query
                if (mysql_query(fConn, query)) {
                        fprintf(stderr, "%s\n", mysql_error(fConn));
                        return(0);
                }
        
	int exists=0;

        // get results
                res = mysql_use_result(fConn);
       int np=0;        
       while ( (row = mysql_fetch_row(res)) != NULL)
        { 
           //printf("last pmt %d entry in run %d \n", np,atoi( row[0]));       
        exists=1;
	if(np>=(nofpmts))
			break;
        sers[np].xo = atof(row[0]);
        sers[np].xo_err=atof(row[1]);

        sers[np].sigma=atof(row[2]);
        sers[np].sigma_err=atof(row[3]);

        sers[np].N=atof(row[4]);
        sers[np].N_err=atof(row[5]);

        sers[np].chisq=atof(row[6]);

	sers[np].exp_t=atof(row[7]);
	sers[np].exp_a=atof(row[8]);

	sers[np].valley=atof(row[9]);

	sers[np].val_of_peak=atof(row[10]);
	sers[np].val_of_valley=atof(row[11]);

	onoff[np]=atoi(row[12]);
         np++; 
	
        //default values stored in DB give errors:
        }
        mysql_free_result(res);
	if(!exists)        
		return 0;
        
        return 1;


}
	











int sqlite_interface::get_pmt_sers_with_vscale(double * sers,int * onoff,int nofpmts,int nrun,int vscale)
{

double *sersb= new double[nofpmts];
double *sersa= new double[nofpmts];

int *onoffb= new int[nofpmts];
int *onoffa= new int[nofpmts];

for(int i=0;i<nofpmts;i++)
{
sersb[i]=0;sersa[i]=0;
onoffb[i]=0;onoffa[i]=0;

}

//division factor for runs different than 50;
double div_fact=vscale/50;

MYSQL_RES *res;
	MYSQL_ROW row;
	
	int bexists=0;
	char query[300];
	
//check if sers exist before:"

	sprintf(query," ");
	sprintf(query,"SELECT run_nr,vscale FROM runs WHERE run_nr BETWEEN %d AND %d AND vscale = 50 ORDER BY run_nr",nrun-10,nrun );
	//std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}

	// get results
	res = mysql_use_result(fConn);
	int run_bef=0;
	while ( (row = mysql_fetch_row(res)) != NULL)
	{ bexists=1;
		
	run_bef=atoi(row[0]);
	
	std::cout << atof(row[0]) << " " << atof(row[1]) << " " << std::endl;
	//sanity check
	//if(i>(nofpmts+1))
	//	break;
	}
	mysql_free_result(res);

/////////////////// get run after ser if it exists.


	int aexists=0;
	
	
//check if sers exist after:"
	sprintf(query," ");
	sprintf(query,"SELECT run_nr,vscale FROM runs WHERE run_nr BETWEEN %d AND %d AND vscale = 50 ORDER BY run_nr DESC",nrun,nrun+10 );
	//std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}

	// get results
	res = mysql_use_result(fConn);
	int aft_run=0;
	while ( (row = mysql_fetch_row(res)) != NULL)
	{ aexists=1;
		
	aft_run=atoi(row[0]);
	
	std::cout << atof(row[0]) << " " << atof(row[1]) << " " << std::endl;
	//sanity check
	//if(i>(nofpmts+1))
	//	break;
	}
	mysql_free_result(res);



if(!bexists && !aexists)
	{
	std::cout << "no 50mV runs found before or after the desired run" << std::endl;
	return 0;

	}
	
//use overloaded old function to get actual sers
int divs=0;
if(bexists)
{
divs+=get_pmt_sers(sersb,onoffb,nofpmts,run_bef,0);

}

if(aexists)
{
divs+=get_pmt_sers(sersa,onoffa,nofpmts,aft_run,0);

}

if(!divs)
	{
	std::cout << "error retrieving 50mV runs" << std::endl;
	return 0;
	
	}	
	
for(int i=0;i<nofpmts;i++)
	{
	sers[i]=(sersb[i]+sersa[i])/divs/div_fact;
	std::cout << sersb[i] << " " << sersa[i]<< " "<< divs << " "<< div_fact << " "<<sers[i] <<std::endl;
	if(aexists && bexists)
	onoff[i]= (int)(onoffa[i] && onoffb[i]);
	else
	onoff[i]= (int)(onoffa[i] || onoffb[i]);
	
	}



delete [] sersb;
delete [] sersa;
delete [] onoffa;
delete [] onoffb;

return 1;
}


///////////////////////////////////
int sqlite_interface::get_ser_parameter_results(int nrun,float *pars)
{

if(check_ser_parameters_exists(nrun))
	{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int exists=0;
	char query[300];
	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT th_multiplier,chan_before,chan_after,window FROM ser_parameters WHERE run_nr = %d ORDER BY run_nr",nrun );
	
	
	std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}


	// get results
	res = mysql_use_result(fConn);
	

	while ( (row = mysql_fetch_row(res)) != NULL)
	{ exists=1;
	 pars[0]=atof(row[0]);
 	 pars[1]=atoi(row[1]);
 	 pars[2]=atoi(row[2]);
 	 pars[3]=atoi(row[3]);
	 std::cout << "+++parameter results " << row[0] << " " <<row[1] << " " << row[2] <<
	 std::endl;
	//default values stored in DB give errors:
	}
	mysql_free_result(res);

	return exists;
	
	
	}
else
	{
	pars[0]=0.;
	pars[1]=0.;
	pars[2]=0.;
	pars[3]=0.;
	return 0;
	}

}



int sqlite_interface::insert_pmt_sers(double * sers,int *onoff,int nofpmts,int nrun,int flag,int scale)
{
//sers array must be filled with nofpmts+1 fields. I.e. 4 pmts = double[5]
// flag is 0 for before run, 1 for after run.

//MYSQL_RES *res;
//MYSQL_ROW row;
	char query[500];
	int serid=0;

	char pmt_id[6];

	if(flag)
		sprintf(pmt_id,"pmt_b");
	else
		sprintf(pmt_id,"pmt_a");


	if( check_ser_entry_exists(nrun,&serid))
//entry exists - must update	
	{

		for(int np=1;np<=nofpmts;np++)
		{
			sprintf(query,"UPDATE sers SET %s=%lf, onflag=%d  WHERE run_nr=%d AND pmt= %d",pmt_id,sers[np],onoff[np-1],serid,np);

			std::cout << query << std::endl;
	//execute query
			if (mysql_query(fConn, query)) {
				fprintf(stderr, "%s\n", mysql_error(fConn));
				return(0);
			}


		}

	}
	else
//entry doesn't exist - must insert
	{

		std::cout << "entry doesn't exist in DB. Error, exiting. \n Fix using ..."<< std::endl;
		insert_new_run(nrun,nofpmts,scale);	
		insert_pmt_sers(sers,onoff,nofpmts,nrun,flag,scale); //recurrent call.
	//std::exit(-1);


	}
	return 0;
}


int sqlite_interface::insert_pmt_sers(results * sers,int * onoff,int nofpmts,int nrun,int flag,int scale)
{
//sers array must be filled with nofpmts+1 fields. I.e. 4 pmts = double[5]
// flag is 0 for before run, 1 for after run.

//MYSQL_RES *res;
//MYSQL_ROW row;
	char query[500];
	int serid=0;

	char pmt_id[6];

	if(flag)
		sprintf(pmt_id,"pmt_b");
	else
		sprintf(pmt_id,"pmt_a");


	if( check_ser_entry_exists(nrun,&serid))
//entry exists - must update	
	{

		for(int np=1;np<=nofpmts;np++)
		{
			sprintf(query,"UPDATE sers SET onflag=%d, %s=%lf, pmt_b_err = %lf, sigma = %lf, sigma_err = %lf, ampl = %lf,  ampl_err=%lf,chisq=%lf,exp_a = %lf, exp_t=%lf,valley=%lf,val_of_peak=%lf,val_of_val=%lf  WHERE run_nr=%d AND pmt= %d",onoff[np-1],pmt_id,sers[np].xo,sers[np].xo_err,sers[np].sigma,sers[np].sigma_err,sers[np].N,sers[np].N_err,sers[np].chisq,sers[np].exp_a,sers[np].exp_t,sers[np].valley,sers[np].val_of_peak,sers[np].val_of_valley,serid,np);

			std::cout << query << std::endl;
	//execute query
			if (mysql_query(fConn, query)) {
				fprintf(stderr, "%s\n", mysql_error(fConn));
				return(0);
			}


		}

	}
	else
//entry doesn't exist - must insert
	{

		std::cout << "entry doesn't exist in DB. Error, exiting. \n Fix using ..."<< std::endl;
		insert_new_run(nrun,nofpmts,scale);	
		insert_pmt_sers(sers,onoff,nofpmts,nrun,flag,scale); //recurrent call.
	//std::exit(-1);


	}
	return 0;








}



int sqlite_interface::insert_new_run(int nrun,int nofpmts,int scale)
{
	int ser_id;
	int test=check_run_entry_exists(nrun,&ser_id);
	char query[300]=" ";

	std::cout << "after check run entry " << test << std::endl;

	if(test)
//run exists
	{
		std::cout << "Run already in DB! not inserting" << std::endl;
		if(check_ser_entry_exists(nrun,&ser_id))
	//ser also exists
		{
			std::cout << "PMT entries already also in DB! exiting" << std::endl;
			std::exit(-1);
		}
		else
	//run exists, but pmt entries do not.
		{
			insert_new_sers_entry(nrun,nofpmts);
		}

	}
	else
//run doesn't exist.
	{
		std::cout << "Run not in DB! inserting" << std::endl;
	//int ser=get_last_ser_entry()+1; //sers should be created new
	//std::cout << "last ser" << std::endl;
	//int par=get_last_param_entry(); // pars uses the last parameter set. 
	//std::cout << "last param" << std::endl;

		sprintf(query,"INSERT INTO runs(run_nr,onflag,vscale) VALUES(%d,1,%d)",nrun,scale);
		printf("%s \n",query);
		
if (mysql_query(fConn, query)) {
			fprintf(stderr, "%s\n", mysql_error(fConn));
			return(0);
		}

		insert_new_sers_entry(nrun,nofpmts);


	}
return 0;
}




int sqlite_interface::get_last_ser_entry()
{

	MYSQL_RES *res;
	MYSQL_ROW row;

	int ser_id;

	char query[300]="SELECT run_nr FROM sers ORDER BY run_nr";

	std::cout << query << std::endl;

	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}
	res = mysql_use_result(fConn);

	while ( (row = mysql_fetch_row(res)) != NULL)
	{ 
		ser_id= atoi(row[0]);
	//default values stored in DB give errors:
	}
	mysql_free_result(res);

	return ser_id;
}



// int sqlite_interface::get_last_param_entry()
// {
// 
// MYSQL_RES *res;
// MYSQL_ROW row;
// 
// int ser_id;
// 
// char query[300]="SELECT p_id FROM params ORDER BY p_id";
// if (mysql_query(fConn, query)) {
// 			fprintf(stderr, "%s\n", mysql_error(fConn));
// 			return(0);
// 			}
// res = mysql_use_result(fConn);
// while ( (row = mysql_fetch_row(res)) != NULL)
// 	{ 
// 	ser_id= atoi(row[0]);
// 	//default values stored in DB give errors:
// 	}
// 	mysql_free_result(res);
// 
// return ser_id;
// }







int sqlite_interface::insert_new_sers_entry(int ser_id,int nofpmts)
{
	char query[300];

	for(int np=1;np<=nofpmts;np++)
	{
		sprintf(query,"INSERT INTO sers(run_nr,onflag,pmt) VALUES(%d,1,%d)",ser_id,np);
		std::cout << query << std::endl;
	//execute query
		if (mysql_query(fConn, query)) {
			fprintf(stderr, "%s\n", mysql_error(fConn));
			return(-1);
		}

	}

	return 0;
}



int sqlite_interface::insert_new_ser_parameters_entry(int run_id)
{
	char query[300];

		sprintf(query,"INSERT INTO ser_parameters(run_nr) VALUES(%d)",run_id);
		std::cout << query << std::endl;
	//execute query
		if (mysql_query(fConn, query)) {
			fprintf(stderr, "%s\n", mysql_error(fConn));
			return(-1);
		}

	

	return 0;
}






int sqlite_interface::check_ser_entry_exists(int nrun,int * ser_id)
{

	MYSQL_RES *res;
	MYSQL_ROW row;
	int exists=0;
	char query[300];
	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT sers.run_nr FROM runs, sers WHERE sers.run_nr=runs.run_nr AND sers.run_nr = %d ORDER BY pmt",nrun );
	
	
	std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}


	// get results
	res = mysql_use_result(fConn);
	

	while ( (row = mysql_fetch_row(res)) != NULL)
	{ exists=1;
	*ser_id= atoi(row[0]);
	//default values stored in DB give errors:
	}
	mysql_free_result(res);

	return exists;
}




int sqlite_interface::check_ser_parameters_exists(int nrun)
{

	MYSQL_RES *res;
	MYSQL_ROW row;
	int exists=0;
	char query[300];
	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT run_nr FROM ser_parameters WHERE run_nr = %d ORDER BY run_nr",nrun );
	
	
	std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}


	// get results
	res = mysql_use_result(fConn);
	

	while ( (row = mysql_fetch_row(res)) != NULL)
	{ exists=1;
	
	//default values stored in DB give errors:
	}
	mysql_free_result(res);

	return exists;
}






int sqlite_interface::check_run_entry_exists(int nrun,int * ser_id)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int exists=0;
	char query[300];
	
	//creating query to read ser values	
	sprintf(query," ");
	sprintf(query,"SELECT run_nr FROM runs  WHERE run_nr = %d",nrun );
	std::cout << query << std::endl;
	//execute query
	if (mysql_query(fConn, query)) {
		fprintf(stderr, "%s\n", mysql_error(fConn));
		return(0);
	}


	// get results
	res = mysql_use_result(fConn);
	
	while ( (row = mysql_fetch_row(res)) != NULL)
	{ exists=1;
	*ser_id= atoi(row[0]);
	//default values stored in DB give errors:
	}
	mysql_free_result(res);
printf(" ££££ run exists %d ",exists);
	return exists;
}

int  sqlite_interface::insert_ser_parameters(int runin,float threshold,int integr_window, int chan_before, int chan_after)
{
	//MYSQL_RES *res;
	//MYSQL_ROW row;
	int exists=0;
	char query[300];
	
	exists=check_ser_parameters_exists(runin);
	
	
	if(exists)
	{
	sprintf(query,"UPDATE ser_parameters SET th_multiplier=%lf,chan_before = %d,chan_after = %d, window = %d  WHERE run_nr=%d", threshold, chan_before, chan_after, integr_window,runin);

		std::cout << query << std::endl;
	//execute query
		if (mysql_query(fConn, query)) {
			fprintf(stderr, "%s\n", mysql_error(fConn));
			return(0);
		}


	

	}
	else
//entry doesn't exist - must insert
{

	std::cout << "entry doesn't exist in DB. Error, exiting. \n Fix using ..."<< std::endl;
	insert_new_ser_parameters_entry(runin);	
	insert_ser_parameters(runin,threshold,integr_window,chan_before,chan_after); //recurrent call.
	//std::exit(-1);
	}

return 1;
}
	
