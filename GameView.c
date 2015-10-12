// GameView.c ... GameView ADT implementation

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "queue.h"
#include <string.h>
static void addTrail(GameView g, PlayerID p, LocationID loc);  //Handy func thanks to 'bradywatkinson'
static void addEncounter(GameView g, LocationID loc);
static void remEncounter(GameView g, PlayerID p);
static void findRailLoc(Map map,Queue found, LocationID *toExplore,LocationID *done,int valid, int numExplore);

//Rimmal
typedef struct Players{
    int healthPoints;
    LocationID currLocation;
    int trail[TRAIL_SIZE];
    
}Players;
struct gameView {
    
    Map gameMap;
    int currentTurn;
    Players players[NUM_PLAYERS];
    int currentScore;
    
    
    // PlayerID currentPlayer;
    //  PlayerMessage messages[];
    
    LocationID encounters[TRAIL_SIZE];
    LocationID Vamp;
};

//Updates the trail array AND curLocation for given player
static void addTrail(GameView g, PlayerID p, LocationID loc)
{
    int i;
    for (i = TRAIL_SIZE-2; i>=0; i--) {
        g->players[p].trail[i+1] = g->players[p].trail[i];
    }
    g->players[p].trail[0] = loc;
    g->players[p].currLocation=loc;
}


static void addEncounter(GameView g, LocationID loc)
{
    int i;
    for (i = TRAIL_SIZE-2; i>=0; i--) {
        g->encounters[i+1] = g->encounters[i];
    }
    
    g->encounters[0] = loc;
}


static void remEncounter(GameView g, PlayerID p)
{
    int i;
    for (i=0; i<TRAIL_SIZE; i++) {
        if(g->encounters[i] == g->encounters[0]) {
            g->encounters[i] = NOWHERE;
            break;
        }
    }
}




//Rimmal
// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gameView = malloc(sizeof(struct gameView));
    assert(gameView != NULL);
    gameView->gameMap = newMap();
    
    gameView->currentScore = GAME_START_SCORE;
    //gameView->currentRound = 0;
    gameView->currentTurn = 0;
    
    //health stats
    gameView->players[PLAYER_LORD_GODALMING].healthPoints = GAME_START_HUNTER_LIFE_POINTS;
    gameView->players[PLAYER_DR_SEWARD].healthPoints = GAME_START_HUNTER_LIFE_POINTS;
    gameView->players[PLAYER_VAN_HELSING].healthPoints = GAME_START_HUNTER_LIFE_POINTS;
    gameView->players[PLAYER_MINA_HARKER].healthPoints = GAME_START_HUNTER_LIFE_POINTS;
    gameView->players[PLAYER_DRACULA].healthPoints = GAME_START_BLOOD_POINTS;
    
    char *oldLoc[4]={NULL};
    
    
    //initially everthing is UNKNOWN
    
    int i;
    gameView->Vamp=UNKNOWN_LOCATION;
    
    for (i = 0; i < NUM_PLAYERS; i++) {
        int j;
        for (j=0; j<TRAIL_SIZE; j++) {
            gameView->players[i].trail[j] = UNKNOWN_LOCATION;
            gameView->encounters[i]=NOWHERE;
        }
        gameView->players[i].currLocation = UNKNOWN_LOCATION;
    }
    
    
    //figuring out(filling up) trail array for each player from pastPlays String
    
    
    int pos;
    for (pos=0; pos<strlen(pastPlays); pos+=8) {
        //figuring the player
        PlayerID player;
        switch (pastPlays[pos]) {
            case 'G':
                player = PLAYER_LORD_GODALMING;
                break;
            case 'S':
                player = PLAYER_DR_SEWARD;
                break;
            case 'H':
                player = PLAYER_VAN_HELSING;
                break;
            case 'M':
                player = PLAYER_MINA_HARKER;
                break;
            case 'D':
                player = PLAYER_DRACULA;
                break;
            default:
                fprintf(stderr, "Wrong Player id. Faulty pastPlays trail\n");
                exit(0);
        }
        
        //if Hunter in Hospital, he gains full life points
        
        if(gameView->players[player].healthPoints < 1)
        {
            gameView->players[player].healthPoints = GAME_START_HUNTER_LIFE_POINTS;
        }
        
        
        
        //extracting the location from pastPlay string and updating trail and location
        char abbrev[3];
        abbrev[0] = pastPlays[pos+1];
        abbrev[1] = pastPlays[pos+2];
        abbrev[2] = '\0';
        
        // printf("*****************player %s\n",abbrev);
        if (player==PLAYER_DRACULA)
        {
            //add points for HI and DB moves if already in CD TO BE DONE
            
            if (!(strncmp(abbrev, "C?", 2))) {
                addTrail(gameView, PLAYER_DRACULA, CITY_UNKNOWN);
            }
            else if (!strncmp(abbrev, "S?", 2)) {
                addTrail(gameView, PLAYER_DRACULA, SEA_UNKNOWN);
                gameView->players[PLAYER_DRACULA].healthPoints -=2;
            }
            else if (abbrev[0]=='D' && abbrev[1]!='U') {//to avoid DUBLIN
                
                int n=abbrev[1]-'0';
                if(n<=5 && n>=1){
                    addTrail(gameView, PLAYER_DRACULA, 102+n);
                    LocationID lol=gameView->players[PLAYER_DRACULA].trail[n];
                    if(lol == SEA_UNKNOWN){
                        gameView->players[PLAYER_DRACULA].healthPoints -=2;
                    }
                    else if(lol>= MIN_MAP_LOCATION && lol <= MAX_MAP_LOCATION)
                    {
                        if(idToType(lol)==SEA){
                            gameView->players[PLAYER_DRACULA].healthPoints -=2;
                        }
                        else if(lol==CASTLE_DRACULA){
                            gameView->players[PLAYER_DRACULA].healthPoints +=LIFE_GAIN_CASTLE_DRACULA;
                            
                        }
                        
                    }
                    else if(lol==HIDE && n<5){
                        if(gameView->players[PLAYER_DRACULA].trail[n+1]==CASTLE_DRACULA){
                            gameView->players[PLAYER_DRACULA].healthPoints +=LIFE_GAIN_CASTLE_DRACULA;
                        }
                    }
                    
                }
                else {
                    fprintf(stderr, "Wrong Double back value. Faulty pastPlays trail\n");
                    exit(0);
                }
            }
            else if (!strncmp(abbrev, "TP", 2)) {
                
                addTrail(gameView, PLAYER_DRACULA, TELEPORT);
                gameView->players[PLAYER_DRACULA].healthPoints +=LIFE_GAIN_CASTLE_DRACULA;
            }
            else if (!strncmp(abbrev, "HI", 2)) {
                addTrail(gameView, PLAYER_DRACULA, HIDE);
                if(gameView->players[PLAYER_DRACULA].trail[1]==CASTLE_DRACULA){
                    gameView->players[PLAYER_DRACULA].healthPoints +=LIFE_GAIN_CASTLE_DRACULA;
                }
            }
            else{
                addTrail(gameView, PLAYER_DRACULA, abbrevToID(abbrev));
                if(idToType(abbrevToID(abbrev))==SEA){
                    gameView->players[PLAYER_DRACULA].healthPoints -=2;
                }
                if(!strncmp(abbrev,"CD",2)){
                    gameView->players[PLAYER_DRACULA].healthPoints +=LIFE_GAIN_CASTLE_DRACULA;
                }
                
            }
        }
        else{
            addTrail(gameView, player, abbrevToID(abbrev));
        }
        
        //read next 4 characters from Pastplays and advance the game
        
        if(player==PLAYER_DRACULA){
            for(int scan=3;scan<7;scan++){
                
                switch(pastPlays[pos+scan]){
                    case '.':
                        break;
                    case 'M':
                        break;
                    case 'T':
                        addEncounter(gameView,gameView->players[PLAYER_DRACULA].trail[0]);
                        break;
                    case 'V':
                        if(scan==5){
                            gameView->currentScore -= SCORE_LOSS_VAMPIRE_MATURES;
                            
                        }
                        else {
                            gameView->Vamp=gameView->players[PLAYER_DRACULA].trail[0];
                        }
                        break;
                        
                }
                
            }
            // printf("Round %d Health of player %d is %d and score %d\n\n",getRound(gameView),player,gameView->players[player].healthPoints, gameView->currentScore);
        }
        else {
            //for all other players
            
            
            for(int scanner=3;scanner<7;scanner++){
                
                switch(pastPlays[pos+scanner]){
                    case '.'://Nothing interesting
                        break;
                    case 'V'://VAMP cleared
                        gameView->Vamp=NOWHERE;
                        break;
                    case 'T'://Trap encounter
                        remEncounter(gameView,player);
                        gameView->players[player].healthPoints -= LIFE_LOSS_TRAP_ENCOUNTER;
                        break;
                    case 'D': //Dracula Encounter
                        gameView->players[player].healthPoints -= LIFE_LOSS_DRACULA_ENCOUNTER;
                        gameView->players[PLAYER_DRACULA].healthPoints -= LIFE_LOSS_HUNTER_ENCOUNTER;
                        
                        break;
                }
            }
            // printf("Health of player %d is %d and score %d\n",player,gameView->players[player].healthPoints, gameView->currentScore);
            if(gameView->players[player].healthPoints <=0){
                
                gameView->players[player].healthPoints = 0;
                gameView->currentScore -= SCORE_LOSS_HUNTER_HOSPITAL;
                gameView->players[player].currLocation = ST_JOSEPH_AND_ST_MARYS;
                
            }
            
            //if hunter has sufficient health after encounters
            //Check whether resting
            //printf("current location player %d %s oldLoc %s\n",player, abbrev, oldLoc[player]);
            if(oldLoc[player]!=NULL){
                if(!strcmp(abbrev,oldLoc[player])){
                    // printf("Resting %s %s\n",oldLoc[player],abbrev);
                    
                    gameView->players[player].healthPoints += LIFE_GAIN_REST;
                    if(gameView->players[player].healthPoints >=9){
                        gameView->players[player].healthPoints=9;
                    }
                    
                    
                }
                //   printf("Inserting abbrev %d  %s in %s\n",player,abbrev,oldLoc[player]);
                oldLoc[player]=malloc(strlen(abbrev));
                strcpy(oldLoc[player],abbrev);
                // for(int m=0;m<4;m++)
                //   printf("All state %d %s\n",m ,oldLoc[m]);
            }else if(oldLoc[player]==NULL){
                // printf("Inserting abbrev %d  %s in %s\n",player,abbrev,oldLoc[player]);
                oldLoc[player]=malloc(strlen(abbrev));
                strcpy(oldLoc[player],abbrev);
                //  for(int k=0;k<4;k++)
                //  printf("All state %d %s\n",k ,oldLoc[k]);
            }
            
            
            
            
        }
        
        if(player==PLAYER_DRACULA){
            gameView->currentScore=gameView->currentScore-SCORE_LOSS_DRACULA_TURN;
        }
        gameView->currentTurn++;
        
    }
    
    return gameView;
}

//Rimmal
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    assert(toBeDeleted != NULL);
    disposeMap(toBeDeleted->gameMap);
    //  free( toBeDeleted->players->trail);
    //  free( toBeDeleted->players);
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

//Vivian
// Get the current round
Round getRound(GameView currentView)
{
    assert(currentView!=NULL);
    return currentView->currentTurn/5;
}

//Vivian
// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    assert(currentView!=NULL);
    return (currentView->currentTurn)%5;
}

//Jessie
// Get the current score
int getScore(GameView currentView)
{
    assert(currentView!=NULL);
    return currentView->currentScore;
}


//jessie
// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    assert(currentView != NULL);
    assert(player < NUM_PLAYERS);
    return currentView->players[player].healthPoints;
}

//Yukti
// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    assert(player < NUM_PLAYERS);
    
    return currentView->players[player].currLocation;
}
//// Functions that return information about the history of the game


//Yukti
// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                LocationID trail[TRAIL_SIZE])
{
    int i;
    for (i=0; i<TRAIL_SIZE; i++) {
        trail[i]=currentView->players[player].trail[i];
    }
}

//// Functions that query the map to find information about connectivity

// connectedLocations() returns an array of LocationID that represent
//   all locations that are connected to the given LocationID.
// road, rail and sea are connections should only be considered
//   if the road, rail, sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// Your function must take into account the round and player id for rail travel
// Your function must take into account that Dracula can't move to
//   the hospital or travel by rail but need not take into account Dracula's trail
// The destination 'from' should be included in the array

//Rimmal
LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    LocationID *connectedLocations = malloc(NUM_MAP_LOCATIONS * sizeof(LocationID));
    assert(connectedLocations != NULL);
    
    //initialize and set all locations to
    int index;
    for (index=0;index<NUM_MAP_LOCATIONS;index++) {
        connectedLocations[index] = UNKNOWN_LOCATION;
    }
    
    LocationID to = 0;
    Queue found = newQueue();
    enterQueue(found,from);
    
    TransportID type[MAX_TRANSPORT];  int i, total;
    //check connection from to all vertices in gameMap
    for(to=0;to<numV(currentView->gameMap);to++){
        
        //printf("Between %s and %s ...\n", idToName(from), idToName(to));
        
        total = connections(currentView->gameMap, from, to, type);
        
        // printf("*********************total link %d to %s \n",total, idToName(to));
        if (total == 0){
            // printf("No direct connection\n");
            // do nothing
            //numLocations=0;
            //return connectedLocations;
        }
        
        else {
            for (i = 0; i < total; i++) {
                switch (type[i]) {
                        //for road all directly connected valid for all players
                    case ROAD:
                        //only usable if road is set as TRUE && not already in Queue
                        if(road && dupCheck(found,to)){
                            // printf("******************road link to %s \n",idToName(to));
                            if(player==PLAYER_DRACULA && to != ST_JOSEPH_AND_ST_MARYS){
                                //  printf("******************road link to %s \n",idToName(to));
                                enterQueue(found,to);}
                            else if(player != PLAYER_DRACULA)
                                enterQueue(found,to);
                            
                            //printf("******%d\n",to);
                        }
                        break;
                        
                        //for rail have to check round+player id
                        //PLAYER_DRACULA cannot use rail
                        //other may use rail but with 1 to max of three hops ???
                    case RAIL:
                        //only usable if rail is set as TRUE and player not equal to Dracula
                        if(rail && player != PLAYER_DRACULA){
                            
                            int valid = (round + player) % 4;
                            
                            //if valid == 0, cannot use rail
                            if(valid){
                                //if valid ==1 only directly connected rail connections
                                if(valid==1 && dupCheck(found,to)){
                                    enterQueue(found,to);
                                } else if(valid<=3)
                                {
                                    //recursively find all rail links that are 2 or 3 hops from "from"
                                    //record the first
                                    if(dupCheck(found,to))
                                        enterQueue(found,to);
                                    
                                    //call function pass valid to it
                                    
                                    LocationID *done = malloc(NUM_MAP_LOCATIONS*sizeof(int));
                                    done[from]=TRUE;
                                    done[to]=TRUE;
                                    LocationID *toExplore = malloc(NUM_MAP_LOCATIONS*sizeof(int));
                                    toExplore[0]=to;
                                    
                                    findRailLoc(currentView->gameMap,found,toExplore,done,valid,1);
                                    
                                }
                                
                            }
                            
                            
                        }
                        break;
                        
                        //port city to sea
                        //sea to sea
                        //sea to port
                        //port city to port city if on same sea
                    case BOAT:
                        //only usable if sea is set as TRUE
                        if(sea && dupCheck(found,to)){
                            enterQueue(found,to);
                        }
                        
                        break;
                        
                        //stderror
                    default:
                        printf("Weird connection\n");
                        break;
                }
            }
        }
        
        
        
    }//for ends
    
    
    //set *numLocations to all connections from this location for this player
    
    *numLocations=countQueue(found);
    
    
    //get all entries from Queue and add to the array of connectedLocations
    
    int kk = 0;
    while(!emptyQueue(found)) {
        LocationID loc = leaveQueue(found);
        connectedLocations[kk] = loc;
        //printf("%d\n",locs[kk]);
        kk++;
    }
    
    assert(kk==*numLocations);
    //printf("*********************Number of locations %d\n",kk);
    return connectedLocations;
}


static void findRailLoc(Map map,Queue found,LocationID *toExplore,LocationID *done,int valid, int numExplore){
    int sub,too,j,k,num=0;
    LocationID *moreExplore = malloc(NUM_MAP_LOCATIONS*sizeof(int));
    TransportID type[MAX_TRANSPORT];
    for(k=0;k<numExplore;k++)
        for(too=0;too<numV(map);too++){
            if(done[too]==TRUE)
                continue;
            sub = connections(map, toExplore[k], too, type);
            if(sub){
                for (j = 0; j < sub; j++) {
                    if(type[j]==RAIL){
                        if(dupCheck(found,too)){
                            enterQueue(found,too);
                            moreExplore[num]=too;
                            num++;
                            done[too]=TRUE;
                        }
                        
                    }
                    
                }
                
            }
            
        }
    valid--;
    if(valid==2){
        findRailLoc(map,found,moreExplore,done,valid,num);
    }
    
}









