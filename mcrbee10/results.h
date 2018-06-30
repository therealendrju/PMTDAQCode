#ifndef __MY_RESULTS__
#define __MY_RESULTS__



struct results{
double xo;
double xo_err;

double sigma;
double sigma_err;

double N;
double N_err;

double chisq;

double exp_t;
double exp_a;

double valley;

double val_of_valley;
double val_of_peak;

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
	this->exp_t=rhs.exp_t;
	this->exp_a=rhs.exp_a;

	this->valley=rhs.valley;

	this->val_of_valley=rhs.val_of_valley;
	this->val_of_peak=rhs.val_of_peak;

	}
return *this;
}

// to zero the whole structure
results& operator=(const double rhs)
{

	this->xo=rhs;
	this->xo_err=rhs;

	this->sigma=rhs;
	this->sigma_err=rhs;

	this->N=rhs;
	this->N_err=rhs;

	this->chisq=rhs;

	this->exp_t=rhs;
	this->exp_a=rhs;

	this->valley=rhs;

	this->val_of_valley=rhs;
	this->val_of_peak=rhs;

return *this;
}







};


// ostream& operator<<(ostream& output,results a)
// {
// 	
// 	output << setw(7) << a.xo <<setw(7)<< " " << a.sigma << setw(7) <<  " " << a.N << " " << setw(7) << a.chisq;
// 	
// 	
// 	return output;	
// }



#endif
