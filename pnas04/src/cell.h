#ifndef CELL_H
#define CELL_H

#include "node.h"
#include "reaction.h"
#include "scorefunction.h"
#include <vector>
#include <iostream>
#include <cmath>

class Cell {

    public:

		//	no default constructor, for construction, number of inducers and proteins should be provided
		Cell (const int& _numind, const int& _numprot);
        Cell (Cell &cell);//copy constructor

		//	deconstructor
		~Cell ();

		//	call for six submutations successively
        void mutation();

        //  get score and change its own currScore value 
        void getScore (ScoreFunc&, double** targetData,int numNodes, int x, bool print);
    
        // return current score generated by getScore
        double getCurrScore();
    
        //description method
        void description(int time);
    
        //cell's equation operator
        bool operator== (Cell& aCell);
    
    
        //generate time courses and put them in currData
        void generateTimeCourses(double** targetData,int numTargetNodes, int x);
    
    
        //calculating correlation matrix elements
        double corMatrixElements(double *timecourse1,double *timecourse2,int points);
    
        //calculating correlation matrix
        void correlationMatrix(int steps);

    
        // calculate current variation between two time points
        void getVariation(int time);
    
        // construct a matrix of variation condition
        void fitnessVariation(int time);
    
        //get nodes vector
        std::vector<Node*>* getNodesVector();
    
        //get rlist vector
        std::vector<Reaction*>* getRlistVector();
    
    private:

        //  The degradation rate of a protein is modified
        void mut_deg_prot();

		//	A kinetic constant of one action is modified
		void mut_kin_const();

		//	A new gene is created
		void mut_add_gene();

		//	A new interaction between a protein and a gene or gene/protein complex is introduced
		void mut_add_regu();

		//	A post transcriptional modification is added
		void mut_add_postmod();

    private:
    
        //number of inducer
        int numInducer;
        
        //current data generated by Runge-Kutta method
        double** currData;

        //matrix that contains two sets of correlation data
        double*** corMatrix;
    
        //current variation between two time points
        int** currDataVariation;
    
        //variation condition matrix of cell
        double** variationCondition;
    
        //durrent score generated with currData using Score function
        double currScore;

		//	check if a node has been added previously
		bool existsNode (const Node& node);
		
		//	check if a reaction has been added previously
		bool existsReaction (const Reaction& rxn);

        //	Global Storage
        vector<Node*> nodes;//contain all the species within a cell
        vector<Reaction*> rlist;	//	document operations made to develop the network
		vector<int> inputIndice;   //indice of input sequences, including inducers and proteins, for scoring function
};

#endif
