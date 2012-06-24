#include "cell.h"



/*constructor:
 input: _numind(number of inducers); _numprot(number of proteins)
 */
Cell::Cell(const int& _numind, const int& _numprot) {
	int currIndex = nodes.size();
	for(int im = 0; im < _numind; im++) {
		Node* inducer = new Node(currIndex, 1);
        nodes.push_back(inducer);
		inputIndice.push_back(currIndex);             //push back inducer's index
        currIndex++;		
	}
	for(int ip = 0; ip < _numprot; ip++) {
		Node* gene = new Node(currIndex, 2);//Type 2 is gene
        nodes.push_back(gene);
        currIndex++;
		Node* prot = new Node(currIndex, 3);//type 3 is protein
        nodes.push_back(prot);
		inputIndice.push_back(currIndex);             //push back protein's index
        currIndex++;

		Reaction *r0 = new Reaction();         //add transcription reaction
		r0->setReversible(false);
		r0->initForwardRateRandomly();
		r0->addReactant(gene);
		r0->addProduct(prot);

		Reaction* r1 = new Reaction();        //add degradation reaction
		r1->setReversible(false);
		r1->initForwardRateRandomly();
		r1->addReactant(prot);

		rlist.push_back(r0);
		rlist.push_back(r1);
	}
}


//duplicate itself
Cell::Cell(Cell &cell){
    
    //copy every node in the cell
    std::vector<Node*>::iterator iter_node = cell.nodes.begin();
    std::vector<Node*>::iterator iter_node_end = cell.nodes.end();
    while (iter_node != iter_node_end) {
        Node* nodeCopy = new Node(*(*iter_node));
        nodes.push_back(nodeCopy);
        iter_node++;
    }
    
    /*constructing the relationships of these cells, 
     *that is implementing components vector in each cell
     */
    int nodesSize = cell.nodes.size();
    for (int indexNinCell = 0; indexNinCell < nodesSize; indexNinCell++) {
        int componentsSize = cell.nodes[indexNinCell]->getNsize();
        for (int componentIndex = 0; componentIndex < componentsSize; componentIndex++) {
            int aComponentIndex = cell.nodes[indexNinCell]->getNode(componentIndex)->getNindex();//store the index of the ith components of this node in the cell
            Node* correspondingComponent = this->nodes[aComponentIndex];
            this->nodes[indexNinCell]->pushNode(correspondingComponent);
        }
    }
    
    
    
    //copy every reaction in the cell
    std::vector<Reaction*>::iterator iter_reaction = cell.rlist.begin();
    std::vector<Reaction*>::iterator iter_reaction_end = cell.rlist.end();
    while (iter_reaction != iter_reaction_end) {
        Reaction* reactionCopy = new Reaction(*(*iter_reaction));
        /*reconstructing the relationships of nodes and rlist,
         *based on the those relationships in the old cell called "cell"
         */
        int reactantsSize = (*iter_reaction)->getReactantsSize();
        for (int indexInReactants = 0; indexInReactants < reactantsSize; indexInReactants++) {
            int aNodeIndex = (*iter_reaction)->getReactant(indexInReactants)->getNindex();
            Node* coorespondingNode = this->nodes[aNodeIndex];
            reactionCopy->addReactant(coorespondingNode);
        }
        int modifiersSize = (*iter_reaction)->getModifiersSize();
        for (int indexInModifiers = 0; indexInModifiers < modifiersSize; indexInModifiers++) {
            int aNodeIndex = (*iter_reaction)->getModifier(indexInModifiers)->getNindex();
            Node* coorespondingNode = this->nodes[aNodeIndex];
            reactionCopy->addModifier(coorespondingNode);
        }
        int productsSize = (*iter_reaction)->getProductsSize();
        for (int indexInProducts = 0; indexInProducts < productsSize; indexInProducts++) {
            int aNodeIndex = (*iter_reaction)->getProduct(indexInProducts)->getNindex();
            Node* coorespondingNode = this->nodes[aNodeIndex];
            reactionCopy->addProduct(coorespondingNode);
        }
        
        rlist.push_back(reactionCopy);
        iter_reaction++;
    }
    
    
    
    
    
}







//destructor
Cell::~Cell() {}



bool Cell::existsNode (const Node& aNode) {
	vector<Node*>::iterator iter = nodes.begin();
    vector<Node*>::iterator iter_end = nodes.end();
	while (iter != iter_end) {
		if(*(*iter) == aNode) {return true;}
		iter ++;
	}
	return false;
}

bool Cell::existsReaction (const Reaction& aReaction) {
	vector<Reaction*>::iterator iter = rlist.begin();
    vector<Reaction*>::iterator iter_end = rlist.end();
	while (iter != iter_end) {
		if(*(*iter) == aReaction) {return true;}
		iter++;
	}
	return false;
}

/*
 *five types of mutation:
 *1. deg_prot: change protein degradation 
 *2. kin_const: change kinetic constant
 *3. add_gene: create a new gene
 *4. add_regu: add new interaction between protein and a gene or a gene-protein complex
 *5. add_postmod: add post transcriptional modification
 */

//mut_deg_prot: change the protein degradation rate
void Cell::mut_deg_prot () {

	if(!rlist.size()) return;               //no reaction in the rlist, no modification

    //  the degradation rate of a protein to be modified
    int numOfDegReaction = 0;               //contain number of degradation reactions
    int indexOfDegReaction = 0;             //index of a certain reaction
    vector<int> indice;                     //contain the indice of degradation reactions

    //  scan <rlist> and choose one degradation reaction (protein)
    std::vector<Reaction*>::iterator iter = rlist.begin();
    std::vector<Reaction*>::iterator iter_end = rlist.end();
    while (iter != iter_end) {
        if((*iter)->getRtype() == 1) {      //#1 is protein degradation
            numOfDegReaction++;
            indice.push_back(indexOfDegReaction);
        }
        indexOfDegReaction++;
        iter ++;
    }
	
	if(!numOfDegReaction) return;           //no protein degradation reaction

    srand((unsigned int)time(NULL));
    int opIndex = indice[rand()%numOfDegReaction];//opIndex contains a certain degradation reaction

    //  modify its degradation rate
    Reaction* currR = rlist[opIndex];
    currR->modifyForwardRate ();

    return;
}


//mut_kin_const: change the kinect constant of a reaction
void Cell::mut_kin_const () {

	if(!rlist.size()) return;
	
	//	a kinetic constant of one reaction to be modified
	
	srand((unsigned int)time(NULL));
	Reaction* currR = rlist[rand()%rlist.size()];

	if((double)rand()/RAND_MAX <= 0.5 || !currR->isReversible()) {
		currR->modifyForwardRate();
	}
	else {
		currR->modifyReverseRate();
	}

	return;
}


//mut_add_gene: add new gene and its protein into nodes, and transcription and protein degradation reaction into rlist
void Cell::mut_add_gene () {            //	add a gene
	
	//	create new nodes representing this gene and its protein
	int currNI = nodes.size();

	Node* gene = new Node(currNI,2);	//	gene
	nodes.push_back(gene);
    
	Node* prot = new Node(currNI+1,3);	//	protein
	nodes.push_back(prot);
	
	//	create Reaction r0, transcription
	Reaction* r0 = new Reaction ();
	r0->setReversible(false);// irreversible and no reactants
	r0->initForwardRateRandomly();
	r0->addModifier(gene);
	r0->addProduct(prot);

	//	create Reaction r1, protein degradation
	Reaction* r1 = new Reaction ();
	r1->setReversible(false);
	r1->initForwardRateRandomly();
	r1->addReactant(prot);
	if(!existsReaction(*r0) && !existsReaction(*r1)){
		rlist.push_back(r0);
		rlist.push_back(r1);
	}
	else{
		delete r0;
        delete r1;
	}
	return;
}


//mut_add_regu: add interaction
void Cell::mut_add_regu () {
	
	//A new interaction between a protein and a gene or a gene/protein complex is introduced
    int numPro = 0;                         //number of protein
    int numGenePro = 0;                     //number of gene/protein complex
	int index = 0;                          //index
    vector<int> protIndice;
	vector<int> cplxIndice;

    //  scan <nodes> and choose  gene and one gene/promoter complex
    std::vector<Node*>::iterator iter1 = nodes.begin();
    std::vector<Node*>::iterator iter1_end = nodes.end();
    while (iter1 != iter1_end) {
        if((*iter1)->getNtype() == 3) {     //   #3 is a protein
            numPro++;
            protIndice.push_back(index);
        }
		
		if((*iter1)->getNtype() == 5||(*iter1)->getNtype() == 2) {     //   #5 is gene/protein complex  and #2 is gene
			numGenePro++;
			cplxIndice.push_back(index);
		}

        index++;
        iter1++;
    }

	if(!numPro || !numGenePro) return; // no gene or gene/protein complex

	//	sample random number
    srand((unsigned int)time(NULL));
    int opIndex1 = protIndice[rand()%numPro];//contain index of a protein
    int opIndex2 = cplxIndice[rand()%numGenePro];//contain index of a gene or a gene/protein complex
	Node* exGene = nodes[opIndex2]->extractFirstGene();
	if(exGene == NULL) return;
	
	//	find the index of the protein created by this extracted gene
	Node* exProt = NULL;
    std::vector<Reaction*>::iterator iter2 = rlist.begin();
    while (iter2 != rlist.end()) {
        if((*iter2)->getRtype() == 0) { //   #0 is gene transcription
			Node* sr = (*iter2)->getModifier(0);
			if(sr != NULL && (sr->getNindex() == exGene->getNindex())) {
				exProt = (*iter2)->getProduct(0);
				break;
			}
        }
        iter2 ++;
    }
	if(exProt == NULL) return;

	//	create a new node as the product
	Node* ncomplex = new Node(nodes.size(), 5, nodes[opIndex1], nodes[opIndex2]);
	nodes.push_back(ncomplex);

	//	create reaction 0, transcription
	Reaction* r0 = new Reaction ();
	r0->setReversible(false);
	r0->initForwardRateRandomly();
	r0->addModifier(ncomplex);
	r0->addProduct(exProt);

	//	create reaction 1, binding/unbinding between protein and gene or gene/protein complex
	Reaction* r1 = new Reaction ();
	r1->setReversible(true);
	r1->initForwardRateRandomly();
	r1->initReverseRateRandomly();
	r1->addReactant(nodes[opIndex1]);
	r1->addReactant(nodes[opIndex2]);
	r1->addProduct (ncomplex);

	if(!existsReaction(*r1) && !existsReaction(*r1)){
		rlist.push_back(r0);
		rlist.push_back(r1);
}
	else {
		delete r0;
		delete r1;
	}

	return;
}



//mut_add_postmod: add a post-transcriptional regulation to the network
void Cell::mut_add_postmod () {
	//	a post modification is add
	
    srand((unsigned int)time(NULL));
	 /* a protein is to be chosen from the existing ones, and a modified version of it is to be introduced */
	if((double)rand()/RAND_MAX <= 0.5) {                 //	single protein case
        int indexOfProt=0;
		int numOfProt=0;
		vector<int> protIndice;

		vector<Node*>::iterator iter = nodes.begin();
		vector<Node*>::iterator iter_end = nodes.end();
		while (iter !=iter_end){
			if((*iter)->getNtype() == 3){   //if this node is single protein
			 protIndice.push_back(indexOfProt);
			 numOfProt++;
			}
			indexOfProt++;
			iter++;
		}
		if(!numOfProt) return;   //no single protein
		int opIndex=protIndice[rand()%numOfProt];  //choose a random single protein
		Node* modProt = new Node(nodes.size(),3);
		nodes.push_back(modProt);

		Reaction* r0=new Reaction();    //add modification reaction
		r0->setReversible(false);
		r0->initForwardRateRandomly();
		r0->addReactant(nodes[opIndex]);
		r0->addProduct(modProt);

		Reaction* r1=new Reaction();   //add degradation reaction of the modified protein
		r1->setReversible(false);
		r1->initForwardRateRandomly();
		r1->addReactant(modProt);

		if(!existsReaction(*r0) && !existsReaction(*r1)){
			rlist.push_back(r0);
			rlist.push_back(r1);
		}
		else{
			delete r0;
			delete r1;
		}
		return;
	}
	else {//	two proteins (protein complex) case
		
		int numOfProt = 0;
		int indexOfProt = 0;
		int numOfSingProt=0;
		int numOfCompProt=0;

		vector<int> protIndice;
		vector<int> singProtIndice;
		vector<int> compProtIndice;
			
		vector<Node*>::iterator iter = nodes.begin();
        vector<Node*>::iterator iter_end = nodes.end();
		while (iter != iter_end) {
			if((*iter)->getNode(0) == NULL) { 
				numOfProt++;
				protIndice.push_back(indexOfProt);
				if((*iter)->getNtype()==3){
					numOfSingProt++;
					singProtIndice.push_back(indexOfProt);
				}
				if((*iter)->getNtype()==6){
					numOfCompProt++;
					compProtIndice.push_back(indexOfProt);
				}
			}
			indexOfProt++;
			iter ++;
		}

		if(!numOfProt) return;	//	no protein or protein complex
		
		double possibility = (double)rand()/RAND_MAX;

		/*case 1, A+B->AB,dimerization*/

		if(possibility < 1/3) {
			int opIndex1 = protIndice[rand()%numOfProt];	//	protein 1
		    int opIndex2 = protIndice[rand()%numOfProt];	//	protein 2
			Node* dimer = new Node (nodes.size(), nodes[opIndex1], nodes[opIndex2]);
			Reaction* dimerization = new Reaction ();
			dimerization->setReversible(true);
			dimerization->initForwardRateRandomly();
			dimerization->initReverseRateRandomly();
			dimerization->addReactant(nodes[opIndex1]);
			dimerization->addReactant(nodes[opIndex2]);
			dimerization->addProduct(dimer);
			if(!existsReaction(*dimerization)) {
				nodes.push_back(dimer);
				rlist.push_back(dimerization);
			}
			else {
				delete dimer;
				delete dimerization;
			}
		}

		/*case 2, A+B->A, partial degration*/

		else if(possibility < 2/3) {      
			if(!numOfSingProt) return;  //no single protein.
			int opIndex1=singProtIndice[rand()%numOfSingProt];   //single protein 1
			int opIndex2=singProtIndice[rand()%numOfSingProt];   //single protein 2
			Reaction* partialDeg = new Reaction ();
			partialDeg->setReversible(false);
			partialDeg->initForwardRateRandomly();
			partialDeg->addReactant(nodes[opIndex1]);
			partialDeg->addReactant(nodes[opIndex2]);
			if(rand()%2==0){                               //equally choose a protein to be degraded
				partialDeg->addProduct(nodes[opIndex1]);}
			else{
				partialDeg->addProduct(nodes[opIndex2]);}
			if(!existsReaction(*partialDeg)){
			    rlist.push_back(partialDeg);}
			else
				delete partialDeg;
		}

		/*case 3, AB+C->A, complex degradation*/

		else {
			if(!numOfCompProt || !numOfSingProt)  return;  //no protein complex
            int opIndex1=compProtIndice[rand()%numOfCompProt];    //complex protein 1
			int opIndex2=singProtIndice[rand()%numOfSingProt];    //single protein 2
			Reaction* compDeg=new Reaction();
			compDeg->setReversible(false);
			compDeg->initForwardRateRandomly();
			compDeg->addReactant(nodes[opIndex1]);
			compDeg->addReactant(nodes[opIndex2]);
			int randComp=rand()%(nodes[opIndex1]->getNsize()-1)+1;
		    compDeg->addProduct(nodes[opIndex1]->getNode(randComp));   //choose the protein in the complex
			if(!existsReaction(*compDeg)){
			   rlist.push_back(compDeg);
			}
			else
				delete compDeg; 
		}

	}
	
	return;
}

#define PROB1 1
#define PROB2 1
#define PROB3 1
#define PROB4 1
#define PROB5 0.0


//overall mutation method
void Cell::mutation(){
    srand((unsigned int)time(NULL));
    if (rand() < RAND_MAX*PROB1) {
        mut_deg_prot();
    }
    if (rand() < RAND_MAX*PROB2) {
        mut_kin_const();
    }
    if (rand() < RAND_MAX*PROB3) {
        mut_add_gene();
    }
    if (rand() < RAND_MAX*PROB4) {
        mut_add_regu();
    }
    if (rand() < RAND_MAX*PROB5) {
        mut_add_postmod();
    }
}

//runge-kutta method
/*input requirement: first colomn of data[][] is the initial value
 *
 */
void runge_kutta(double **data,vector<Node*> nodes,vector<Reaction*> rlist ,int series, int steps)
{//100 is steps
	
	int currSerie,currStep;
	int i;
    double k1,k2,k3,k4;         //parameter in the runge-kutta method
    double *y = new double[series], *tempY = new double[series];
	
	
	
	//runge_kutta method:
	for(currStep = 0; currStep < steps-1; currStep++){
		//initial values assigned to y[]
		for(currSerie = 0; currSerie < series; currSerie++){
			y[currSerie] = data[currSerie][currStep];
		}
		for(currSerie = 0; currSerie < series; currSerie++){
            
			k1 = (nodes[currSerie]->ode)(rlist,y,currStep);
			for(i = 0; i < series; i++){
				tempY[i] = y[i] + k1/2.;
			}
			k2 = (nodes[currSerie]->ode)(rlist,tempY,currStep+0.5);
			for(i = 0; i < series; i++){
				tempY[i] = y[i] + k2/2.;
			}
			k3 = (nodes[currSerie]->ode)(rlist,tempY,currStep+0.5);
			for(i = 0; i < series; i++){
				tempY[i] = y[i] + k3;
			}
			k4 = (nodes[currSerie]->ode)(rlist,tempY,currStep+1);
			data[currSerie][currStep+1] = y[currSerie] + 1/6.0*(k1+2*k2+2*k3+k4);
		}
    }
    delete [] y;
    delete [] tempY;
}



/*get score using the sfunc as score function and change its own currScore member
 *fist: using Runge-Kutta method to generate the time course and store all them in currData
 *second: using the sfunction as a score function, and passing currData and targetData as parameters to calculate score
 *third: assign the score to currScore
 
 
 
 *prerequirements: nodes in the cell's "nodes" vector should be sorted by indice
 */
void Cell::getScore(ScoreFunc& sfunc, double** targetData, int numTargetNodes, int time){
   
    int size = nodes.size();//  how many nodes in this cell
    /* initialization: store the initial value in the first column of
     * currData, and for coloumn with index greater than number of target
     * nodes, initial value is 0
     */
	for (int i = 0; i < size; i++) {           
        currData[i][0] = 0.;
    }
    for (int i = 0; i < numTargetNodes; i++) {
        currData[inputIndice[i]][0] = targetData[i][0];    //the initial value of inducers and proteins are the same as the input data.
    }

    /* runge_kutta method, store the results in currData */
    runge_kutta(currData, nodes, rlist, size, time);
    
    /* calculate total score uses the ScoreFunc sfunc: only numTargetNodes nodes
     * are calculated because there only those number nodes in targetData
     * therefore, numTargetNodes = numind + numprot
     */
    double totalScore = 0;
    for (int i = 0; i < numTargetNodes; i++) {
        totalScore += sfunc.getScore(currData[inputIndice[i]],targetData[i],time);  //compare the RK data and the input data, using score function.
    }
    
    currScore = totalScore;
}

	
//return the currSore generated by getScore method
double Cell::getCurrScore(){
    std::cout << currScore << std::endl;
    return currScore;
}
















