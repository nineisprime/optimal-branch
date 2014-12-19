#include<math.h>

#include <yvals.h>

#if (_MSC_VER >= 1600)
#define __STDC_UTF_16__
#endif

#include<matrix.h>
#include<mex.h>

#include "OptimumBranching.hpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){

	mxArray *weights_in_m;
	double *weights, *tree_out;
	int n;
	int s,t,i;
	const mwSize *dims;
	
	weights_in_m = mxDuplicateArray(prhs[0]);
	dims = mxGetDimensions(prhs[0]);
	n = (int) dims[0]; 
	
	
	plhs[0] = mxCreateDoubleMatrix(n-1,2,mxREAL);
	
	weights = mxGetPr(weights_in_m);
	tree_out = mxGetPr(plhs[0]);
	
	mexPrintf("Creating weight_nested_vec \n");
	
	// convert "weights" to vector<vector<double>>
	vector<vector<double> > weights_nested_vec;	 //*d
	vector<double> cur_vec;  // *d
	for (s=0; s<n; s++){
		
		for (t=0; t<n; t++){
			//if (s==t)
			//	continue;
			
			cur_vec.push_back(weights[t*n+s]);
		}
		weights_nested_vec.push_back(cur_vec);
		cur_vec.clear();
	}
	
	/*
	for (s=0; s<n; s++){
		for (t=0; t<n;t++){
			if (s==t)
				continue;	
			mexPrintf("weights[%d][%d]: %f\n", s+1, t+1, weights_nested_vec[s][t]);
		}
	}
	*/
	
	mexPrintf("Calling optimumBranching \n");
	
	// call optimumBranching
	list<pair<int,int> > opt_branch;
	optimumBranching(weights_nested_vec, n, opt_branch);
	
	// convert list<pair<int,int>> to tree_out
	list<pair<int,int> >::iterator it1;
	for (it1=opt_branch.begin(),i=0; it1!=opt_branch.end(); it1++, i++){
		tree_out[0*(n-1)+i] =  it1->first + 1;
		tree_out[1*(n-1)+i] =  it1->second + 1;
		mexPrintf("%d (%d %d)\n",i, it1->first, it1->second);
	}
}