// DracView.c ... DracView ADT implementation

//After Round 5
//Fixed case when first move is TP and DB is already there, still you can hide at CD

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h"
#include <string.h>
#include "queue.h"
//... if you decide to use the Map ADT


typedef int MinionType;

#define IMMATUREVAMP 0
#define TRAP 1

typedef struct Minion{
    LocationID location;
    MinionType type;
}Minion;

//Rimmal
struct dracView {
    
    GameView gameView;
    Minion minions[TRAIL_SIZE];
};


//Rimmal -- done
// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    assert(messages != NULL);
    assert(pastPlays != NULL);
    
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->gameView = newGameView(pastPlays, messages);
    
    
    //Minions Info
    //Intialising minion list
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        dracView->minions[i].location = NOWHERE;
        dracView->minions[i].type = NOWHERE;
        
    }
    
    //fill minion list with current state of game
    int pos,index=0;
    LocationID trail[TRAIL_SIZE];
    
    giveMeTheTrail(dracView, PLAYER_DRACULA,trail);
    
    for (i = 0; i < TRAIL_SIZE; i++){
        if(trail[i]!=UNKNOWN_LOCATION)
            index=i;
        printf("whats in trail: %d index %d\n",trail[i], index);
    }
    int lpos=0;
    char location[3];
    for (pos=0; pos < strlen(pastPlays); pos+=8) {
        if(pastPlays[pos]=='D'){
            location[0] = pastPlays[pos+1];
            location[1] = pastPlays[pos+2];
            location[2] = '\0';
            
            if(!strncmp(location, "HI", 2)){
                lpos=pos-40;
                location[0] = pastPlays[lpos+1];
                location[1] = pastPlays[lpos+2];
                location[2] = '\0';
            }else if(location[0]=='D' && location[1]!='U') {//to avoid DUBLIN
                
                int n=location[1]-'0';
                if(n<=5 && n>=1){
                    lpos=pos-n*40;
                    location[0] = pastPlays[lpos+1];
                    location[1] = pastPlays[lpos+2];
                    location[2] = '\0';
                    
                }
                
                
            }
            
            
            for (int scan=3;scan<5;scan++){
                switch(pastPlays[pos+scan]){
                    case 'T':
                        //     printf("TRAP at %d\n",abbrevToID(location));
                        dracView->minions[index].type = TRAP;
                        dracView->minions[index].location = abbrevToID(location);
                        break;
                    case 'V':
                        //     printf("VAMP at %d\n",abbrevToID(location));
                        dracView->minions[index].location = abbrevToID(location);
                        dracView->minions[index].type = IMMATUREVAMP;
                        break;
                    default:
                        //  printf("Nothing at %d\n",abbrevToID(location));
                        dracView->minions[index].location = abbrevToID(location);
                        
                }
            }
            index--;
            if(index==0)
                index=5;
        }
        
    }
    
    
    /*  for (i = 0; i < TRAIL_SIZE; i++) {
     printf("Minion Location %d\n",dracView->minions[i].location);
     printf("Minion Type %d\n",dracView->minions[i].type);
     
     }*/
    
    
    
    
    return dracView;
}

//Rimmal -- done
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    if(toBeDeleted != NULL){
        
        disposeGameView(toBeDeleted->gameView);
        free( toBeDeleted );
    }
}

//Vivian -- done
//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return getRound(currentView->gameView);
}

//Jessie -- done
// Get the current score
int giveMeTheScore(DracView currentView)
{
    return getScore(currentView->gameView);
}


//Jessie -- done
// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return getHealth(currentView->gameView, player);
}

//Yukti--done
// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    
    LocationID loc=getLocation(currentView->gameView,player);
    
    
    if (player==PLAYER_DRACULA && loc >= HIDE && loc <= TELEPORT) {
        LocationID trail[TRAIL_SIZE];
        getHistory(currentView->gameView, PLAYER_DRACULA, trail);
        switch (loc) {
            case HIDE:
                loc = trail[1];
                if(trail[1]>HIDE && trail[1]<=TELEPORT)
                {
                    switch(loc){
                            
                        case DOUBLE_BACK_1:
                            loc = trail[2];
                            break;
                        case DOUBLE_BACK_2:
                            loc=trail[3];
                            break;
                        case DOUBLE_BACK_3:
                            loc = trail[4];
                            break;
                        case DOUBLE_BACK_4:
                            loc = trail[5];
                            break;
                        case TELEPORT:
                            loc = CASTLE_DRACULA;
                            break;
                            
                    }
                    
                    
                }
                break;
            case DOUBLE_BACK_1:
                loc = trail[1];
                break;
            case DOUBLE_BACK_2:
                loc=trail[2];
                break;
            case DOUBLE_BACK_3:
                loc = trail[3];
                break;
            case DOUBLE_BACK_4:
                loc = trail[4];
                break;
            case DOUBLE_BACK_5:
                loc = trail[5];
                break;
            case TELEPORT:
                loc = CASTLE_DRACULA;
                break;
        }
    }
    return loc;
    
    
    
}


//Yukti -- done
// Get the most recent move of a given player
//first location in trail is the most recent
void lastMove(DracView currentView, PlayerID player,
              LocationID *start, LocationID *end)
{
    LocationID trail[TRAIL_SIZE];
    getHistory(currentView->gameView, player, trail);
    *start=trail[1];
    *end=trail[0];
    return;
}


//Jessie --done
// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                int *numTraps, int *numVamps)
{
    int i=0;
    *numTraps=0;
    *numVamps=0;
    while (i<TRAIL_SIZE) {
        if (currentView->minions[i].location==where) {
            if (currentView->minions[i].type==TRAP)
                *numTraps+=1;
            else if (currentView->minions[i].type==IMMATUREVAMP)
                *numVamps+=1;
        }
        i++;
    }
}

//// Functions that return information about the history of the game

//Yukti-done
// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                    LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->gameView, player, trail);
}

//// Functions that query the map to find information about connectivity


//Vivian
// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    //get all connected locations
    LocationID from = whereIs(currentView, PLAYER_DRACULA);
    Round round = giveMeTheRound(currentView);
    int rail = 0;
    int totalLocs = 0;
    LocationID *cLocations;
    
    cLocations= connectedLocations(currentView->gameView, numLocations, from, PLAYER_DRACULA, round, road, rail, sea);
    totalLocs = *numLocations;
    // printf("numLocations1:%d\n", *numLocations);
    // printf("edges[1]:%d\n", edges[1]);
    
    //for(int q=0;q<totalLocs;q++){
    
    //   printf("Total Available Locations: %s\n", idToName(cLocations[q]));
    // }
    
    //get the Dracula Trail
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(currentView, PLAYER_DRACULA, trail);
    
    
    //check whether there is HIDE or double Back used in Trail
    //if both are used no location from the trail is valid
    //if double back then only current location in trail is valid as you can still hide there
    //else all locations in trail are valid as you can hide at current or call double back
    int doubleBack = FALSE;
    int hide = FALSE;
    
    LocationID *updatedLocations = malloc(NUM_MAP_LOCATIONS*sizeof(LocationID));
    
    int i = 0, j = 0;
    while (i < TRAIL_SIZE-1) {
        if (trail[i] >= DOUBLE_BACK_1 && trail[i] <= DOUBLE_BACK_5) {
            doubleBack = TRUE;
        }else if (trail[i] == HIDE) {
            hide = TRUE;
        }
        i++;
    }
    
    // if both Hide and doubleBack in trail
    if (hide == TRUE && doubleBack == TRUE) {
        for (i = 0; i < totalLocs; i++) {
            if (cLocations[i] != trail[0] && cLocations[i] != trail[1] && cLocations[i] != trail[2] && cLocations[i] != trail[3] && cLocations[i] != trail[4] && cLocations[i] != ST_JOSEPH_AND_ST_MARYS) {
                updatedLocations[j] = cLocations[i];
                j++;
            }
        }
        (*numLocations) = j;
        cLocations = updatedLocations;
        // doubleBack TRUE, Hide false: still can hide at current location if not sea
    } else if (doubleBack == TRUE) {
        // If the first move in the trail is a double back, that poistion is available for hiding
        if (trail[0] >= DOUBLE_BACK_1 && trail[0] <= DOUBLE_BACK_5) {
            int backOff = trail[0]-102;
            
            for (i = 0; i < totalLocs; i++) {
                if (cLocations[i] == trail[backOff]) {
                    if (idToType(trail[backOff] != SEA)) {
                        updatedLocations[j] = cLocations[i];
                        j++;
                    }
                } else if (cLocations[i] != trail[1] && cLocations[i] != trail[2] && cLocations[i] != trail[3] && cLocations[i] != trail[4] && cLocations[i] != ST_JOSEPH_AND_ST_MARYS) {
                    updatedLocations[j] = cLocations[i];
                    j++;
                }
            }
            
        } else if(trail[0]==TELEPORT) {
            //  printf("I am Here\n");
            
            for (i = 0; i < totalLocs; i++) {
                if (cLocations[i] == CASTLE_DRACULA) {
                    
                    updatedLocations[j] = cLocations[i];
                    j++;
                    
                } else if (cLocations[i] != trail[1] && cLocations[i] != trail[2] && cLocations[i] != trail[3] && cLocations[i] != trail[4] && cLocations[i] != ST_JOSEPH_AND_ST_MARYS) {
                    updatedLocations[j] = cLocations[i];
                    j++;
                }
            }
            
        } else {
            //double back in trail excpet first move: you can only hide at current position if not sea
            for (i = 0; i < totalLocs; i++) {
                if (cLocations[i] != trail[1] && cLocations[i] != trail[2] && cLocations[i] != trail[3] && cLocations[i] != trail[4] && cLocations[i] != ST_JOSEPH_AND_ST_MARYS) {
                    if (cLocations[i] == trail[0]) {
                        if (idToType(trail[0]) != SEA) {
                            updatedLocations[j] = cLocations[i];
                            j++;
                        }
                    } else {
                        updatedLocations[j] = cLocations[i];
                        j++;
                    }
                }
            }
        }
        (*numLocations) = j;
        cLocations = updatedLocations;
    } else {
        for (i = 0; i < totalLocs; i++) {
            if (cLocations[i] != ST_JOSEPH_AND_ST_MARYS) {
                updatedLocations[j] = cLocations[i];
                j++;
            }
        }
        (*numLocations) = j;
        cLocations = updatedLocations;
        
    }
    
    return cLocations;
}


//Vivian -- done

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    LocationID from = whereIs(currentView, player);
    Round round = giveMeTheRound(currentView);
    return connectedLocations(currentView->gameView, numLocations, from, player, round+1, road, rail, sea);
}




int pathToCD(DracView currentView, int src, int *path, int road, int sea){
    
    
    //cannot go through the hospital
    int j=0;
    int visiting = src;
    int tmp_distance = 0;
    int path_distance = 0;
    int doubleBack=FALSE;
    
    
    Queue cityQ = newQueue();
    enterQueue(cityQ, src);
    
    int visited[NUM_MAP_LOCATIONS] = {0};
    int prev[NUM_MAP_LOCATIONS] = {[0 ... (NUM_MAP_LOCATIONS-1)] = UNKNOWN_LOCATION};
    
    
    //get dracula trail and check whether there is a DB move made within the last 5
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(currentView, PLAYER_DRACULA, trail);
    
    while (j < TRAIL_SIZE-1) {
        if (trail[j] >= DOUBLE_BACK_1 && trail[j] <= DOUBLE_BACK_5) {
            doubleBack = TRUE;
        }
        j++;
    }
    
    
    
    while (emptyQueue(cityQ) == FALSE && visiting != CASTLE_DRACULA) {
        visiting = leaveQueue(cityQ);
        
        int i;
        int num_locs;
        int *locs = connectedLocations(currentView->gameView, &num_locs,visiting, PLAYER_DRACULA, giveMeTheRound(currentView),road,0,sea);
        
        
        //if DBack is in trail and the first hop are included in the TRAIL exclude that from search
        
        for (i=0;i<num_locs;i++) {
            for(j=0;j<TRAIL_SIZE-1;j++){
                if (doubleBack && locs[i]== trail[j] && locs[i] != ST_JOSEPH_AND_ST_MARYS) {
                    //      printf("excluding %d matches in trail\n",locs[i]);
                    
                    visited[locs[i]] = TRUE;
                }
            }
        }
        
        
        for (i=0;i<num_locs;i++) {
            if (!visited[locs[i]] && locs[i] != ST_JOSEPH_AND_ST_MARYS) {
                enterQueue(cityQ, locs[i]);
                prev[locs[i]] = visiting;
                visited[locs[i]] = TRUE;
            }
        }
        
        if (visiting == CASTLE_DRACULA) {
            prev[locs[i]] = visiting;
            int index = locs[i];
            while (index != src) {
                index = prev[index];
                path_distance++;
            }
            
            // Building path array, storing destination first
            tmp_distance = path_distance-1;
            path[tmp_distance] = CASTLE_DRACULA;
            tmp_distance--;
            
            // Storing rest of array
            index = prev[CASTLE_DRACULA];
            while (tmp_distance >= 0) {
                path[tmp_distance] = index;
                index = prev[index];
                tmp_distance--;
            }
            break;
        }
    }
    
    
    //  printf("path->");
    
    //  for(j=0;j<path_distance;j++) {
    //      printf("%d->",path[j]);
    //  }
    //  printf("x\n");
    
    return path_distance;
}














