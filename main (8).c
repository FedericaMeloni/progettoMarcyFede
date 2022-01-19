#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define	MAXX		60	/* Dimensione dello schermo di output (colonne) */
#define MAXY		20	/* Dimensione dello schermo di output (righe)   */
#define DELAY 80000		/* Ritardo nel movimento delle vespe (da adattare) */

//dimensioni matrice sprite
#define DIM_ORIZZ 4	//3+ un terminatore
#define DIM_VERT 3		//non necessitC  di un terminatore
#define SPRITENULL "   "	//per eliminare lo sprite, attraverso sovrascrizione


//id per distinguere le entitC 
/*id per identificare che cosa C( lo sprite*/
#define LISTALASER 10
#define ID_BOMBA 'b'
#define ID_ASTRONAVE 'a'
#define ID_LASER 'l'
#define ID_NEMICO 'n'
#define MISSILE_UP 3
#define MISSILE_DOWN -3

typedef struct pos
{

  int x;
  int y;
  char sprite[DIM_VERT][DIM_ORIZZ];
  char id;
  pid_t pid;
  int vite;
} Pos;


void CreateLaser(int pipeout, int posizione_astronave,int spostamento_v, int pidParametro);
void Astronave (int pipeout);
void azzera (Pos dato_letto);
void stampaOggetto (Pos oggetto);
void Controllo(int pipein);

int main()
{
  initscr();    /* Inizializza lo schermo */
  noecho();     /* Disabilita la visualizzazione dei tasti premuti */
  keypad(stdscr, 1);  /* Abilita la gestione dei tasti freccia */
  curs_set(0);  /* Nasconde il cursore */

 // getmaxyx(stdscr, maxy, maxx); /* Ottiene dimensioni schermo */
 //int x = maxx/2, y = maxy/2; /* Centro dello schermo */
  int pidA;
    int p[2]; 
    int p1[2]; 
    pipe(p);
    pipe(p1);

    //creao le pipe per la comunicazione dei processi
    
		/* Creo il primo processo figlio NEMICI */
		pidA= fork();
		switch(pidA){
  
		/* Se il pid == 0 -> si tratta del processo 'Vespa' */
		case 0:
				close(p[0]); /* chiusura del descrittore di lettura in lo user dia un input, non vogliamo leggere in questo processo*/
    		    Astronave(p[1]);   
    		  
    		  	
		
    	
    		default:
					/* Sono ancora nel processo padre */
			
					close(p[1]); /* chiusura del descrittore di scrittura */
			        Controllo(p[0]);  /* controllo deve ricevere in input ciò che gli scrivono gli altri processi*/  
     		}    	

  //	kill(pidC,1);
		/* Termino il gioco ed esco dal programma */
		printf("\n\n\nGAME OVER\n\n\n");	

		return 0;			    
}



void Controllo(int pipein)
{
struct pos astronave,  bomba, laser, dato_letto, previous;
int numero_nemicix=6;
int numero_nemiciy=6;
struct pos Nemici[numero_nemicix][numero_nemiciy];
int collision=0;

/* Visualizzo le vite iniziali*/

int r=0;
int c=0;
int indice=0;
int i=0;
int idInizialeLaser=0;
struct pos listaLaser[10];

//CICLO PRINCIPALE
/* Il ciclo si ripete finchè le vite dell'astronave terminano */
 do {
		/* Leggo dalla pipe */
    read(pipein,&dato_letto,sizeof(dato_letto));

    switch(dato_letto.id){
        	

        case 'a':
           /* Cancello il precedente carattere visualizzato */
           
           /* Aggiorno le coordinate relative alla nuova posizione */
           azzera(previous);
           astronave= dato_letto;
            stampaOggetto (astronave);
            previous= dato_letto;
        
           /* Visualizzo le vite rimaste astronave*/
	       mvprintw(0,1,"%d",dato_letto.vite);	
           break;
           
        case MISSILE_DOWN: 
            c=0;
            for(i=0; i<LISTALASER; i++){
              if(listaLaser[i].pid==laser.pid){
            
                  mvprintw(previous.y, previous.x, " ");
                  mvprintw(dato_letto.y, dato_letto.x, "O");
                  laser = dato_letto;
                  previous=dato_letto;
              }
              else{
                  c++;
              }
            }
            if(c==LISTALASER){
                listaLaser[indice]=laser;
                indice++;
            }
           
            break; 
            
        case MISSILE_UP:
            c=0;
            for(i=0; i<LISTALASER; i++){
              if(listaLaser[i].pid==laser.pid){
                  
                  mvprintw(previous.y, previous.x, " ");
                  mvprintw(dato_letto.y, dato_letto.x, "O");
                  laser = dato_letto;
                  previous=dato_letto;
              }
              else{
                  c++;
              }
            }
            if(c==LISTALASER){
                listaLaser[indice]=laser;
                indice++;
            }
    }
    
    
    /* Aggiorno lo schermo di output per visualizzare le modifiche */
       refresh();

     /* Segnalo collisione */
     
				if(astronave.vite < 1) collision=1; 

  

  }while(!collision);

}






void Astronave(int pipeout){
  struct pos astronave, laser, laserDown;
  astronave.x = 0;
  astronave.y = (MAXY / 2);
  astronave.id = ID_ASTRONAVE;	/*per distinguere i valori letti */
  astronave.vite = 3;
  astronave.pid = getpid ();

  strcpy (astronave.sprite[0], " | ");
  strcpy (astronave.sprite[1], "+++");
  strcpy (astronave.sprite[2], " | ");

  /* Comunico al processo padre le coordinate iniziali del contadino */
  write (pipeout, &astronave, sizeof (astronave));	/*posizione di partenza */
 


while(true){
    int c = getch();
 if (c == KEY_UP && astronave.y > 4)
	{
	   //previous= astronave;  
	  astronave.y -= 1;
	}

      if (c == KEY_DOWN && astronave.y < MAXY - 4)
	{
	   // previous= astronave;
	  astronave.y += 1;
	}
	

	 if(c==' '){
        laser.pid=fork();
        if(laser.pid==0){
            CreateLaser(pipeout, astronave.y, MISSILE_UP, laser.pid);
        }
        laserDown.pid=fork();
        if(laserDown.pid==0){
            CreateLaser(pipeout, astronave.y, MISSILE_DOWN, laserDown.pid);
        }
      
    }

	  usleep(500000);
    write (pipeout, &astronave, sizeof (astronave));		/* Inserisco una pausa per rallentare il movimento */
   
     
 
  }
}

void CreateLaser(int pipeout, int posizione_astronave, int spostamento_v, int pidParametro){
    int bordo=5;

    struct pos laser;
    laser.y = posizione_astronave+1;
    laser.x = 7;
    laser.id = spostamento_v;
    laser.pid = pidParametro;

    int i;
    while(laser.x!=MAXX-bordo && laser.y!=MAXY-bordo || laser.y!=bordo){
        
        
        laser.x++;
        if(laser.x%10==0){
        laser.y= laser.y-spostamento_v;
        }
        
        usleep(500000);
        write(pipeout, &laser, sizeof(laser));
    }
    exit(0);
}


void stampaOggetto(Pos o){

    int i=0;
   for(i = 0 ; i <3; i++)
        mvprintw(o.y+i, o.x, o.sprite[i]);
    

}
void azzera(Pos o){
  int i=0;
   for(i = 0 ; i <3; i++){
      mvprintw(o.y+i, o.x,"   ");
   }
}


