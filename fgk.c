#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>


//The algorithm was studied in class as part of the formation as Software Engineer.
//If you have any doubt on how the algorithm works, i suggest to do some research first.

//Node Structure
typedef struct node {
    unsigned char ch;       //The character contained by the node
    unsigned int value;    //The node's Weight
    int rank;               //The node's order
    struct node *left;      //Pointer to the left son
    struct node *right;     //Pointer to the right son
    struct node *father;    //Pointer to the father
}Node;


//Global variables
FILE *fileCompresso;        //Italian for "Compressed file". Pointer to the file we'll be creating if coding
FILE *fileInput;            //Pointer to the File given as input
FILE *fileDecompresso;      //Italian for "Uncompressed(?) file". Pointer to the file  we'll be creating if decoding
Node *array[257];           //Array Containing every possible node
Node *zero;					//The zero node
Node *radice;				//Italian for "root". This is the root node of the Tree.


//Italian for "Create Tree"
void creaAlbero() {
    radice = malloc(sizeof(Node));  
    radice->rank = 1000;  
    radice->value = 0;    

    zero = malloc(sizeof(Node));
    zero->rank = 998;
    zero -> value = 0;
}

//Italian for "Chain to the left"
void collegaSinistra(Node* p1, Node*p2){
    p1->left = p2;      
    p2->father =p1;     
}

//Italian for "Chain to the right"
void collegaDestra(Node* p1, Node* p2){
    p1->right = p2;     
    p2->father = p1;    
}

//Italian for "Is Right son"
int figlioDestro(Node* pn){
    if(pn->father==NULL){
        return 0;
    }

    if(pn->father->left==pn){
        return 0;
    } else {
        return 1;
    }
}

//Italian for "Is Left son"
int figlioSinistro(Node* pn){
    if(pn->father==NULL){
        return 0;
    } else {
        if(figlioDestro(pn)==true){
            return 0;
        } else {
            return 1;
        }
    }
}

//Italian for "New Node". Creates a node that is added in the Tree by splitting the zero node.
Node* nuovoNodo(Node* root, unsigned char ch){
    Node* tempZero = array[256];                    //Recupero del nodo zero.

    Node* nuovo = (Node*)malloc(sizeof(Node));      //Creazione di un nuovo nodo (Padre del nodo il cui carattere � appena apparso e del nodo zero)
    Node* foglia = (Node*)malloc(sizeof(Node));     //Creazione del nodo il cui carattere � appena apparso.

    nuovo->ch = -1;                                 //Poniamo il carattere del nodo non foglia con un valore invalido.
    nuovo->rank = tempZero->rank;                   //L'ordine del nodo non foglia sar� lo stesso che ha avuto sin'ora il nodo zero.
    nuovo->value = 0;                               //Il peso del nuovo nodo � posto a zero, verr� poi aggiornato in seguito.

    foglia->ch = ch;                                //Il carattere del nodo nuovo � il carattere nuovo appena trovato.
    foglia->rank = tempZero->rank-1;                //L'ordine del nodo foglia � uno inferiore rispetto a quello del nodo zero sino ad ora.
    foglia->value= 0;                               //Il peso del nuovo nodo � posto a zero, verr� poi aggiornato in seguito.
    tempZero->father->left =  nuovo;                //Il vecchio padre del nodo zero diventa padre del nuovo nodo non foglia.
    nuovo->father = tempZero->father;
    tempZero->rank -= 2;                            //Diminuzione dell'ordine del nodo zero di 2. Questa procedura garantisce il mantenimento degli ordini.
    collegaSinistra(nuovo, tempZero);               //Collegamento del nodo non foglia col nodo zero
    collegaDestra(nuovo, foglia);                   //Collegamento del nodo non foglia col nodo nuovo.
    return foglia;
}

//Italian for "Switch Nodes". Given two nodes, this method switches their position.
void scambiaNodi(Node* nodo1, Node* nodo2) {
    if(figlioDestro(nodo1)) {                       //Il secondo nodo 'prende il posto' del primo per quanto riguarda la paternit�.
        nodo1->father->right = nodo2;
    } else {
        nodo1->father->left = nodo2;
    }

    if(figlioDestro(nodo2)) {                       //Il primo nodo 'prende il posto' del secondo per quanto riguarda la paternit�.
        nodo2->father->right = nodo1;
    } else {
        nodo2->father->left = nodo1;
    }

    int tempRank = nodo1->rank;                     //Gli ordini precedenti allo scambio vengono mantenuti nella posizione originale.
    nodo1->rank = nodo2->rank;
    nodo2->rank = tempRank;

    Node* temp = nodo1->father;                     //Scambio dei padri dei due nodi.
    nodo1->father = nodo2->father;
    nodo2->father = temp;
}

//Italian for "Find node to swap". Method that tells us wich node has to be exchanged in the Tree
Node* trovaNodoDaScambiare(Node *current, Node *radice) {
    Node *temp = current;
    if(radice->value > temp->value && radice->left != NULL && radice->right != NULL) {
        Node* maggioreSx = trovaNodoDaScambiare(temp, radice->left);
        if (maggioreSx != NULL) {
            temp = maggioreSx;
        }
        Node* maggioreDx = trovaNodoDaScambiare(temp, radice->right);
        if (maggioreDx != NULL) {
            temp = maggioreDx;
        }
    } else if (radice->value == temp->value && radice->rank > temp->rank) {
        temp = radice;
    }
    if (temp != current){
        return temp;
    } else {
        return NULL;
    }
}

//Italian for "Elaborate Tree". Checks if there are nodes that have to be switched and switches them.
void elaboraAlbero(Node* current, Node* radice) {
    while(current != radice) {
        Node* nodoDaScambiare = trovaNodoDaScambiare(current, radice); //Cerco se c'è un nodo con cui effettuare lo scambio

        if(nodoDaScambiare && nodoDaScambiare->right != current) { // Se viene trovato un nodo da scambiare e questo non è il padre (caso in cui i figli siano il nodo zero + nodo corrente)
            scambiaNodi(current, nodoDaScambiare); //Si effettua lo scambio tra i due nodi
        }
        (current->value)++; //Incremento del peso del nodo corrente
        current = current->father; // si sale di un livello puntando al padre del nodo corrente

    }
    (radice->value)++;
}



//Italian for "Read Bit". Method that allows to read a file bit by bit.
int leggiBit(FILE* file, int numeroBitFinali, unsigned int dimensioneFile){
    static char ch; // char in cui viene salvato il byte in lettura
    static int contatore = -1; //contatore che indica quanti bit si devono leggere ancora da 'ch'.
    int rit;
    if(contatore == -1){ //Nel caso in cui contatore == -1 significa che si è letto per intero il byte salvato in ch, quindi bisogna effettuare una nuova lettura.
        if(fread(&ch,sizeof(char),1,file)){
            contatore = 7;
        } else {
            return -1; //se fread non viene eseguito con successo significa che si è giunti alla fine del file.
        }
    }
    /*
    Quando in lettura del file viene raggiunto il penultimo byte del file compresso (dimensioneFile == ftell(file))
    e il numero di bit scritti (7-contatore) è maggiore o uguale al numero di bit "terminali" viene interrotta la lettura.
    numeroBitFinali equivale al numero di bit rilevanti scritti alla fine della compressione (bit "terminali").
    In alternativa si controlla con ftell che il cursore non oltrepassi il penultimo byte, in tal caso si ritorna -1
     */

    if((dimensioneFile == ftell(file) && numeroBitFinali <= (7 - contatore)) || (ftell(file) > dimensioneFile)) {
      return -1;
    }


    rit = (ch >> contatore) & 0x01; // rit contiene il valore del bit da ritornare (0 o 1) che viene ricavato shiftando a destra ch del valore di contatore e effettua un mask con 0x01

    contatore--;
    return rit;
}

//Italian for "Read Char". Method that allows us to read the file char by char.
int* leggiChar(char ch){
  int* c = malloc(sizeof(int)*8);
    for(int i = 7; i >= 0; i--) {
        c[7-i] = (ch >> i) & 0x01; // con il ciclo for si legge un bit per volta e lo si 'posiziona' all'interno di un int.
    }
    return c;
}

//Method useful to find the depth of a certain node.
int findDepth(Node* pn){
    int counter=0;
    Node* temp = pn;

    while(temp->father!=NULL){              //Ciclo che incrementa un contatore ogni volta che il nodo ha un padre.
        temp = temp->father;                //Il contatore aumenta per ogni padre.
        counter++;
    }

    return counter;                         //Il contatore � la profondit� del nodo.
}

//Method that returns the path of a node given the node and his depth
int* findPath(Node* n, int depth){                  // Passiamo come argomento il nodo del quale trovare il percorso e la profondità a cui si trova, trovata con il metodo findPath
    int* path = malloc(sizeof(int)*depth);          //Allochiamo lo spazio necessario per contenere il percorso del file, che avr� tanti int quanta la sua profondit�.
    int i = 0;

    Node* temp = n;
    while(temp->father!=NULL){                      //Partendo dal nodo, risaliamo l'albero fino alla radice (father == NULL), e creiamo il codice del percorso al contrario.
        if(figlioDestro(temp)==1){                  //Utilizziamo i metodi figlioDestro/sinistro per verificare se un nodo è figlio destro o sinistro del padre.
            path[i]=1;
        } else if(figlioSinistro(temp)==1){
            path[i]=0;
        }

        if(temp->father!=NULL){
            temp = temp->father;
        }
        i++;
    }

    for(int z = 0; z<depth/2; z++){                 //Una volta giunti alla radice, rigiriamo il percorso, in modo tale da averlo partendo dalla radice fino alla foglia e non al contrario.
        int t = path[z];
        path[z] = path[depth-1-z];
        path[depth-1-z] = t;
    }

    return path; //ritorniamo il percorso corretto dalla radice al nodo passato come argomento
}


//Italian for "Create Output Code". Method responsable for writing the paths when coding.
void creaCodiceOutput(int path[], int dimension, int finale){
    static unsigned char daScrivere; //char statico in cui vengono salvati i bit da scrivere
    static int n = 8; //valore di int che indica di quanto shiftare a sx il bit da scrivere.
    static unsigned char bit; //bit da inserire in daScrivere
    if(finale) { //flag che indica se si è giunti all'ultima scrittura, quindi se si deve fare la scrittura anche se il char daScrivere non è completa
        for (int i = 0; i < dimension; i++) {

            n--;
            bit = ((unsigned char) path[i]) << (n); // si shifta a sinistra di un valore pari a n per "incodare" i bit. Se si hanno 0 bit in daScrivere, n = 7, allora si shifta il carattere in path[i] di 7 a sinistra.
            daScrivere = daScrivere | bit;
            /* Con un 'OR' aggiorno il char daScrivere aggiungendo il bit nuovo.
            Quando n==0 allora avremo daScrivere 'pieno' e si effettua la scrittura su file, rimettendo daScrivere a 0 e riportando n = 8.
            */
            if (n == 0) {
                n = 8;
                fwrite(&daScrivere, sizeof(unsigned char), 1, fileCompresso);
                daScrivere = 0x00;
            }
        }
    } else {
      // Si effettua l'ultima scrittura. Si scrive un byte contenente gli ultimi bit 'a sinistra' rilevanti e poi un ulteriore byte il cui valore sarà pari al numero di bit rilevanti scritti nel byte precedente.
        int quantitaBit = 8-n;
        fwrite(&daScrivere, sizeof(unsigned char), 1, fileCompresso);
        fwrite(&quantitaBit, sizeof(unsigned char), 1, fileCompresso);
    }
    //Al termine della chiamata la memoria occupata dall'array path viene liberata
    free(path);
}

//Italian for "Create Char Output Code".
void creaCodiceOutputCarattere(int* path, int dimension, char* ch){
    creaCodiceOutput(path, dimension, -1);
    int* c;
    c = leggiChar(*ch);
    creaCodiceOutput(c, 8, -1);
}

//Italian for "Deallocate Tree". Method resposible for deleting the tree safely.
void deallocaAlbero(Node* current) {
  if (current == NULL) {
    return;
  }

    deallocaAlbero(current->left);
    deallocaAlbero(current->right);
    free(current);
  }




//Main
//Il programma verifica dapprima che gli argomenti siano 3 (oltre il nome del file). Si verifica che il file passato esista, che l'opzione selezionata sia corretta ecc...
//In seguito, il programma esegue la compressione/decompressione.
int main(int argc, char**argv){
  clock_t t;
  t= clock();
    if(argc!=4){
        printf("The program requires 3 arguments: \n '-c' coding or '-d' decoding \n input file name \n output file name.");
    } else {
        if((fileInput = fopen(argv[2],"rb") )!= NULL){  //Verifichiamo se il file passato come argomento esiste
            //fileInput = fopen(argv[2],"rb");

            if(strcmp(argv[1],"-c")==0){ //Se file esiste verifichiamo se procedere alla compressione o alla decompressione
                printf("Coding...\n");
                fileCompresso = fopen(argv[3] ,"wb"); //Il file compresso viene creato

                /* Al nodo radice attribuiamo come rank il valore arbitrario 1000. Il nodo 0, essendo figlio sinistro, avrà rank 998
                 *
                 */

                creaAlbero();
                array[256] = zero;

                unsigned char attuale; //char in cui salviamo il byte letto

                while(fread(&attuale,sizeof(unsigned char),1,fileInput)){
                    if(array[attuale]!=NULL){
                        //Se esiste già un nodo che contiene il carattere letto scriviamo il percorso del nodo ed in seguito adattiamo l'albero.

                        Node* temp = array[attuale];                  //Identificazione del nodo

                        int tempDepth = findDepth(temp);                  //Identificazione della profondità di tale nodo
                        int* percorso = findPath(temp, tempDepth);
                        creaCodiceOutput(percorso,tempDepth, -1);  //Scrittura del percorso sul file

                        //Adattamento dell'albero


                        elaboraAlbero(temp, radice);

                    } else {
                        //Se il carattere è nuovo, scriviamo il percorso per il nodo zero ed in seguito il carattere.
                        int tempDepth = findDepth(array[256]);  //Troviam la profondità del nodo zero.

                        if(tempDepth==0){ //Se si tratta del primo carattere incontrato creiamo il nodo, lo colleghiamo a destra della radice, alla quale colleghiamo anche il nodo 0
                            fwrite(&attuale,sizeof(char),1,fileCompresso);

                            Node* primo = (Node*)malloc(sizeof(Node));
                            primo->ch = attuale;
                            primo->rank = 999;
                            collegaDestra(radice, primo);
                            collegaSinistra(radice,zero);
                            elaboraAlbero(primo,radice);
                            array[attuale] = primo;
                        } else {
                            /*
                             * Se non si tratta del primo carattere (albero con 3 nodi almeno: radice, zero e carattere) troviamo il percorso del nodo 0
                             * Salviamo su fileCompresso il percorso del nodo zero e del carattere da aggiungere, creiamo poi un nuovo nodo contente il carattere salvato in attuale.
                             * Procediamo all'elaborazione dell'albero partendo dal nuovo nodo
                             */
                            int* percorso = findPath(zero, tempDepth);

                            unsigned char* ptAttuale = &attuale;
                            creaCodiceOutputCarattere(percorso,tempDepth,ptAttuale);

                            Node* temp = nuovoNodo(radice,attuale);
                            array[attuale] = temp;
                            elaboraAlbero(temp, radice);
                        }

                    }
                }
                creaCodiceOutput(0, 0, 0);
                deallocaAlbero(radice);
                printf("\nFile coded successfully\n");
            } else if(strcmp(argv[1],"-d")==0){
                printf("Decoding...\n");
                fseek(fileInput, -1, SEEK_END);
                unsigned int dimensioneFile = ftell(fileInput);                 //Conoscere la dimensione del file è essenziale per poter scrivere i bit 'avanzanti' dalla compressione.
                int numeroBitAllaFine;                                          //L'ultimo byte ha un valore decimale che va' da 0 a 8. Indica quanti bit dell'penultimo byte si devono considerare.
                fread(&numeroBitAllaFine, sizeof(unsigned char), 1, fileInput); //Grazie a queste 3 linee di codice, sappiamo dove dobbiamo fermare la decompressione.
                rewind(fileInput);
                fileDecompresso = fopen(argv[3],"wb");

                //Inizializzazione dell'albero con radice, nodo zero e nodo contenente il primo carattere letto.
                creaAlbero();
                array[256] = zero;

                unsigned char attuale;
                fread(&attuale,sizeof(unsigned char),1,fileInput);
                fwrite(&attuale,sizeof(unsigned char),1,fileDecompresso);
                Node* primo = (Node*)malloc(sizeof(Node));
                primo->ch = attuale;
                primo->rank = 999;
                radice->rank = 1000;
                collegaDestra(radice,primo);
                collegaSinistra(radice,zero);
                primo->value = 1;
                primo->father->value = 1;
                array[attuale] = primo;

                int bitLetto = 0;                   //Variabile che conterrà l'ultimo bit letto (grazie alla funzione leggiBit())

                while(bitLetto>=0){                 //La lettura continua fino alla fine del file (che non avverrà mai in realtà siccome sappiamo dove fermarci)
                    Node* tmp = radice;

                    //Discesa nell'albero sino a trovare il nodo contenente il prossimo carattere.
                    while(tmp->left!=NULL && tmp->right!=NULL){
                        bitLetto = leggiBit(fileInput, numeroBitAllaFine, dimensioneFile);  //Lettura del bit

                        if(bitLetto==1){
                            tmp = tmp->right;           //Spostamento a destra dell'albero se il bit letto è un uno
                        } else if (bitLetto==0){
                            tmp = tmp->left;            //Spostamento a sinistra dell'albero se il bit letto è uno zero
                        } else {
                            break;                      //Caso in cui il file sia giunto alla fine (impossibile)
                        }
                    }


                    //Verifichiamo se il percorso fornitoci porta al nodo zero o ad un nodo conosciuto, ed agiamo di conseguenza.
                    if(bitLetto>=0){                    //Nel caso in cui il file non sia giunto alla fine.
                        if(tmp==array[256]){            //Se il nodo trovato è il nodo zero, procediamo a scrivere su file il prossimo carattere e lo aggiungiamo all'albero
                            attuale = 0x00;
                            for(int w = 0; w<8; w++){
                                attuale = attuale | (leggiBit(fileInput, numeroBitAllaFine, dimensioneFile) << (7-w));  //Lettura dei prossimi 8 bit
                            }

                            Node* nuovo = nuovoNodo(radice,attuale);                        //Creazione del nuovo nodo contenente il carattere.
                            array[attuale] = nuovo;
                            fwrite(&attuale,sizeof(unsigned char),1,fileDecompresso);

                            elaboraAlbero(nuovo,radice);                                    //Elaborazione dell'albero, che potrebbe mutare data l'aggiunta del nuovo nodo.
                        } else {
                            fwrite(&(tmp->ch),sizeof(unsigned char),1,fileDecompresso);     //Se il nodo trovato non è il nodo zero, significa che possiamo scrivere il carattere di tale nodo.
                            elaboraAlbero(tmp,radice);
                        }
                    }
                }

                deallocaAlbero(radice);
                printf("\nDecoding successful.\n");
            } else {
                printf("Invalid option, please select '-c' if coding or '-d' if decoding\n");
            }
        } else {
            printf("File not found.\n");
        }
    }
    t = clock() -t;
    double timetaken = ((double)t)/CLOCKS_PER_SEC;
    fprintf(stderr, "Seconds of execution %f\n", timetaken);
}