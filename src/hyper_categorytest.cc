/*
 * code modified from:
 * FUNC - Functional Analysis of Gene Expression Data
 * Copyright (C) 2002  Bjoern Muetzel, Kay Pruefer
 */

#include <vector>
#include <string>
#include <fstream>
#include "go_groups_hyper.h"


#include <Rcpp.h>
using namespace Rcpp;


//[[Rcpp::export]]
void hyper_category_test(std::string directory, int cutoff, std::string root, bool silent)

{
	
	/*************
         * parsing arguments, creating in and outstreams
         ************/
	string input = directory + "_randset_out";
	istream *in ;
	in = new ifstream( input.c_str() ) ;
	if ( ! *in ) {
		Rcpp::Rcerr << "Cannot open " << input << endl ;
	}
	string output = directory + "_category_test_out";
	ofstream out( output.c_str( )) ;
	if ( ! out ) {
		Rcpp::Rcerr << "Cannot open " << output << endl ;
	}
	string out_min_p = directory + "_min_p";
	ofstream min_p( out_min_p.c_str( )) ;
	if ( ! out ) {
		Rcpp::Rcerr << "Cannot open " << out_min_p << endl ;
	}

	/*************
         * start reading from randomset-file  
         ************/
	// ignore header-lines
	string dummy ;
	getline( *in, dummy ) ;  // #Genes
	getline( *in, dummy ) ;  // #annotated GOs

	string groups ;	
	getline( *in, groups ) ; // GO IDs
	if ( groups == "" ) {
		Rcpp::stop("Error reading randomsets") ;
	}
	
	string detected, changed ;
	// ignore detected-values (reading it from profile instead)
	getline( *in, detected ) ;  // #annotated genes (candidate and background)
	getline( *in, changed ) ;  // #annotated candidate genes

	/*************
         * go_groups handles parsing and analysis of dataset and randset lines
         ************/
	go_groups_hyper gos( groups, detected, changed, root, cutoff ) ;

	// returns number of significant groups for 0.1, 0.05, 0.01, 0.001, 0.0001
	// for under and over respresentation -> 10 values, also printed to console below
	int *realdata = gos.calculate_data( ) ;

	int sum_randdata[10] ; // sum significant groups across all randomsets 
	for ( int i=0 ; i < 10 ; ++i ) sum_randdata[i] = 0 ;
	int nr_groups_ge[10] ; // sum of randsets where randdata[i] >= realdata[i] 
	for ( int i=0 ; i < 10 ; ++i ) nr_groups_ge[i] = 0 ;
	int num_randdata = 0 ;
	// randomsets
	if (!silent){
		Rcpp::Rcout << endl << "Evaluating randomsets: " << endl;
		Rcpp::Rcout << "No. of significant ontology nodes for" << endl;
		Rcpp::Rcout << "under-representation\t\t\tover-representation" << endl ;
		Rcpp::Rcout << "of candidate genes at p-value thresholds" << endl ;
		Rcpp::Rcout << "0.1\t0.05\t0.01\t0.001\t0.0001\t0.1\t0.05\t0.01\t0.001\t0.0001" << endl ;
	}
	string data ;
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
	// FWERs
	gos.print_pvals( num_randdata, out ) ;
	// save min_p to file for FWER-to-pval-interpolation in refinement
	gos.print_min_p( min_p ) ;

	// write summary to console
	if (!silent){
		Rcpp::Rcout << "Randomsets: " << num_randdata << endl << endl ;	
		// number of significant groups for 0.1, 0.05, 0.01, 0.001, 0.0001 for under- and over-rep -> 10 values
		Rcpp::Rcout << "Real data:" << endl ;
		Rcpp::Rcout << "No. of significant ontology nodes for" << endl;
		Rcpp::Rcout << "under-representation\t\t\tover-representation" << endl ;
		Rcpp::Rcout << "of candidate genes at p-value thresholds" << endl ;
		Rcpp::Rcout << "0.1\t0.05\t0.01\t0.001\t0.0001\t0.1\t0.05\t0.01\t0.001\t0.0001" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << realdata[i] << "\t" ;
		Rcpp::Rcout << endl ;
		// Mean number of significant nodes across all randomsets for different p-value cutoffs 0.1, 0.05, 0.01, 0.001, 0.0001; under- and overrep
		//Rcpp::Rcout << "# sig. groups mean randomsets" << endl ;
		Rcpp::Rcout << endl << "mean No. of significant groups in randomsets:" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << sum_randdata[i]/static_cast<double>(num_randdata) << "\t" ;
		Rcpp::Rcout << endl ;
		// proportion of randomsets where number of significant nodes at p 0.1, 0.05, 0.01, 0.001, 0.0001 is >= in original data
		Rcpp::Rcout << "p value" << endl ;
		for ( int i = 0 ; i < 10 ; ++i ) 
			Rcpp::Rcout << nr_groups_ge[i]/static_cast<double>(num_randdata) << "\t" ;
		Rcpp::Rcout << endl << endl ;
	}

	delete in;
	delete[] realdata;
}
