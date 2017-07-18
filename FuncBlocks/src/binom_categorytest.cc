
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "go_groups_binom.h"

#include <Rcpp.h>
using namespace Rcpp;

//[[Rcpp::export]]
void binom_category_test(std::string directory, int cutoff, std::string root, bool silent)
{

	/*************
         * parsing arguments, creating in and outstreams
         ************/
	string ranout = directory + "_randset_out";
	istream *in ;
	in = new ifstream( ranout.c_str() ) ;
	if ( ! *in ) {
		Rcpp::Rcerr << "Cannot open " << ranout << endl ;
	}
	
	string outfile = directory + "_category_test_out";
	std::ofstream out( outfile.c_str() ) ;
	if ( ! out ) {
		Rcpp::Rcerr << "Cannot open " << outfile << endl ;
	}

	//string root_go ;
	//{ 
		//istringstream ppp( argv[5] ) ;
		//ppp >> root_go ; // steffi: just use root from argument
	//}

	/*************
         * start reading from randomset-file  
         ************/
	string groups ;
	getline( *in, groups ) ; // GO IDs
	if ( groups == "" ) {
		Rcpp::stop("Error reading random sets");
	}

	string genes_per_go_file = directory + "_ngenes_per_go";
	ifstream in_genespergroup( genes_per_go_file.c_str() ) ;
	if ( ! in_genespergroup ) {
		Rcpp::Rcerr << "Cannot open " << genes_per_go_file << endl ;
	}

	/*************
         * go_groups handles parsing and analysis of dataset and randset lines
         ************/
	go_groups_binom gos( groups, &in_genespergroup, cutoff, root ) ;

	string data ;
	getline( *in, data ) ; // real data 

	// returns number of significant groups for 0.1, 0.05, 0.01, 0.001
	int *realdata = gos.calculate_data( data ) ;

	int sum_randdata[10] ; // sum significant groups across all randomsets
	for ( int i=0 ; i < 10 ; ++i ) sum_randdata[i] = 0 ;
	int nr_groups_ge[10] ; // sum of randsets where randdata[i] >= realdata[i]
	for ( int i=0 ; i < 10 ; ++i ) nr_groups_ge[i] = 0 ;
	int num_randdata = 0 ;
	// randomsets
	if (!silent){
		Rcpp::Rcout << endl << "Evaluating randomsets: " << endl;
		Rcpp::Rcout << "No. of significant ontology nodes for" << endl;
		Rcpp::Rcout << "high-freq-B\t\t\t\thigh-freq-A" << endl ;
		Rcpp::Rcout << "at p-value thresholds" << endl ;
		Rcpp::Rcout << "0.1\t0.05\t0.01\t0.001\t0.0001\t0.1\t0.05\t0.01\t0.001\t0.0001" << endl ;
	}
	
	while ( *in ) {
		getline( *in, data ) ;
		if ( data == "" ) { break ; } 
		int *randdata = gos.calculate_rand( data ) ;
		for ( int i=0 ; i<10 ; ++i ) {
			sum_randdata[i] += randdata[i] ;
			if ( randdata[i] >= realdata[i] ) {
				nr_groups_ge[i]++ ;
			}
		}
		if (!silent){
			for ( int i=0 ; i < 10 ; ++i ) {
				Rcpp::Rcout << randdata[i] << "\t" ;
			}
			Rcpp::Rcout << "\n" ;
		}
		delete[] randdata ;
		num_randdata++ ;
	}
	gos.print_pvals( num_randdata, out ) ;
	
	// write summary to console
	if (!silent){
		Rcpp::Rcout << "Randomsets: " << num_randdata << endl << endl ;
		// number of significant groups for 0.1, 0.05, 0.01, 0.001, 0.0001 for under- and over-rep -> 10 values
		Rcpp::Rcout << "Real data:" << endl ;
		Rcpp::Rcout << "No. of significant ontology nodes for" << endl;
		Rcpp::Rcout << "high-freq-B\t\t\t\thigh-freq-A" << endl ;
		Rcpp::Rcout << "of candidate genes at p-value thresholds" << endl ;
		Rcpp::Rcout << "0.1\t0.05\t0.01\t0.001\t0.0001\t0.1\t0.05\t0.01\t0.001\t0.0001" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << realdata[i] << "\t" ;
		Rcpp::Rcout << endl ;
		// Mean number of significant nodes across all randomsets for different p-value cutoffs 0.1, 0.05, 0.01, 0.001, 0.0001; under- and overrep
		Rcpp::Rcout << endl << "mean No. of significant groups in randomsets:" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << sum_randdata[i]/static_cast<double>(num_randdata) << "\t" ;
		Rcpp::Rcout << endl ;
		Rcpp::Rcout << "# p value" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << nr_groups_ge[i]/static_cast<double>(num_randdata) << "\t" ;
		Rcpp::Rcout << endl << endl ;
	}

	// steffi: Fuer R-package nicht noetig:
	// write outfile
	//out << "Randomsets: " << num_randdata << endl ;
	//out << "chimp\t\t\t\thuman" << endl ;
	//out << "# sig. groups dataset" << endl ;
	//for ( int i = 0 ; i < 10 ; ++i ) 
		//out << realdata[i] << "\t" ;
	//out << endl ;
	//out << "# sig. groups mean randomsets" << endl ;
	//for ( int i = 0 ; i < 10 ; ++i ) 
		//out << sum_randdata[i]/static_cast<double>(num_randdata) << "\t" ;
	//out << endl ;
	//out << "# p value" << endl ;
	//for ( int i = 0 ; i < 10 ; ++i ) 
		//out << nr_groups_ge[i]/static_cast<double>(num_randdata) << "\t" ;
	//out << endl ;
	
	delete in;
	delete[] realdata;
}