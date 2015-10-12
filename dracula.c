// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

//After Round 3
//fixed the case when wrong Teleport was being made
//added the functionality to move towards CD when helth is low (<15 BP)

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Map.h"
#include "Places.h"


static int checkTrail(int trail[TRAIL_SIZE], int bestLoc);
static char* hideOrDB(DracView gameState, int trail[TRAIL_SIZE], int bestLoc);


void decideDraculaMove(DracView gameState)
{
    
    //first move get the locations of all players
    //move to one of possible locations that is not within 1 hop of a current player location
    int numLocations[5];
    int *path[5]={NULL};
    int avLoc[NUM_MAP_LOCATIONS]={[0 ... (NUM_MAP_LOCATIONS-1)] = TRUE};
    int disLoc[NUM_MAP_LOCATIONS]={[0 ... (NUM_MAP_LOCATIONS-1)] = 100};
    int player;
    Map map=newMap();
    int bestLoc=UNKNOWN_LOCATION;
    int tempDist=0;
    int teleported=FALSE;
    int flag=FALSE;
    //get the trail
    
    int trail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
    
    
    /*  for(int i=0;i<TRAIL_SIZE;i++){
     if(trail[i]<=MAX_MAP_LOCATION && trail[i]>= MIN_MAP_LOCATION)
     printf("TRAIL %s\n",idToAbbrev(trail[i]));
     else
     printf("TRAIL %d\n",trail[i]);
     }*/
    
    //printf("Dracula current position: %d\n",whereIs(gameState,PLAYER_DRACULA));
    
    //record locations each player can access in next round
    for(player=0;player<NUM_PLAYERS-1;player++){
        
        path[player]= whereCanTheyGo(gameState, &numLocations[player], player, TRUE, TRUE,TRUE);
        //printf("numLocations:%d\n", numLocations[player]);
        
        //  for(int j=0;j<numLocations[player];j++)
        //      printf("Possible next moves for player %d: %s\n",player,idToName(path[player][j]));
        
    }
    
    
    
    if(giveMeTheRound(gameState) == 0){
        
        //initailly Dracula can choose to go anywhere
        // printf("In here\n");
        //for(int i=0;i<NUM_MAP_LOCATIONS;i++){
        //        avLoc[i]=TRUE;
        //}
        
        //exclude locations where players can go in next turn
        
        for(player=0;player<NUM_PLAYERS-1;player++){
            for(int j=0;j<numLocations[player];j++){
                for (int i=0;i<NUM_MAP_LOCATIONS;i++){
                    if(i==ST_JOSEPH_AND_ST_MARYS || idToType(i)==SEA){
                        avLoc[i]=FALSE;
                    }
                    else if(i==path[player][j]){
                        //   printf("Match: player next Location %s: %s\n",idToName(path[player][j]),idToName(i));
                        avLoc[i]=FALSE;
                    }
                }
            }
            
        }
        
        
        //these are the available safe locations excluding SEA, dont want to start off from SEA
        //get max distance from each location
        for(int i=0;i<NUM_MAP_LOCATIONS;i++){
            if(avLoc[i]==TRUE){
                //printf("Safe Locations %s\n",idToName(i));
                for(player=0;player<NUM_PLAYERS-1;player++){
                    for(int j=0;j<numLocations[player];j++){
                        tempDist=findPathDist(map,i,path[player][j]);
                        //  printf("Distance between %s and player %d %s is %d\n", idToName(i), player, idToName(path[player][j]), tempDist);
                        if(disLoc[i]>tempDist){
                            disLoc[i]=tempDist;
                        }
                    }
                }
            }
            
        }
        
        
        // find distances from each safe location such that
        // MAX distance from any next move of any player: try to be as away as possible from any player
        
        tempDist=0;
        
        for(int i=0;i<NUM_MAP_LOCATIONS;i++) {
            
            if(avLoc[i]==TRUE && disLoc[i]>=tempDist){
                // printf("Checking %d distance %d\n",avLoc[i],disLoc[i]);
                
                tempDist=disLoc[i];
                bestLoc=i;
                
            }
            
        }
        
        // printf("I am Here %d\n",bestLoc);
        
        //  printf("best location %s with distance %d from any dangerous location\n",idToName(bestLoc),tempDist);
        
        
    }else{
        player=PLAYER_DRACULA;
        
        //if health low move towards CD to recoup health
        if(howHealthyIs(gameState, PLAYER_DRACULA)<=15){
            
            printf("ALARM\n");
            
            //int path_distance;
            
            LocationID pathCD[NUM_MAP_LOCATIONS]={[0 ... (NUM_MAP_LOCATIONS-1)] = UNKNOWN_LOCATION};
            
            pathToCD(gameState, whereIs(gameState, PLAYER_DRACULA), pathCD, TRUE, TRUE);
            
            
            if(pathCD[1]!=UNKNOWN_LOCATION){
                bestLoc=pathCD[1];
                flag=TRUE;
                
            }
        }
        
        
        if(flag==FALSE){
            
            
            //if its not the first move, dracula already has a position on the map
            path[player]= whereCanIgo(gameState, &numLocations[player], TRUE, TRUE);
            // printf("I am here %d\n",numLocations[player]);
            if(numLocations[player] > 0){
                
                //  for(int j=0;j<numLocations[player];j++)
                //   printf("Possible next moves for player DRACULA: %s\n",idToName(path[player][j]));
                
                for(player=0;player<NUM_PLAYERS-1;player++){
                    for(int j=0;j<numLocations[player];j++){
                        for (int i=0;i<numLocations[PLAYER_DRACULA];i++){
                            if(path[PLAYER_DRACULA][i]==path[player][j]){
                                //   printf("Match: player next Location %s: %s\n",idToName(path[player][j]),idToName(path[PLAYER_DRACULA][i]));
                                avLoc[path[PLAYER_DRACULA][i]]=FALSE;
                            }
                        }
                    }
                    
                }
                
                
                //if current location is sea and any Dback move already in trail, exclude the current location as next move as you cannot hide at Sea
                
                int myCurrent=whereIs(gameState, PLAYER_DRACULA);
                int i=0;
                while(i < TRAIL_SIZE-1){
                    /*  if(trail[i]<=MAX_MAP_LOCATION && trail[i]>= MIN_MAP_LOCATION)
                     printf("TRAIL %s\n",idToAbbrev(trail[i]));
                     else
                     printf("TRAIL %d\n",trail[i]);
                     */
                    if (trail[i] >= DOUBLE_BACK_1 && trail[i] <= DOUBLE_BACK_5) {
                        
                        if(isSea(myCurrent)==TRUE){
                            
                            //     printf("my current location is SEA at %d\n",myCurrent);
                            avLoc[myCurrent]=FALSE;
                            
                        }
                        
                    }
                    i++;
                }
                
                
                
                
                
                for(int i=0;i<numLocations[PLAYER_DRACULA];i++){
                    if(avLoc[path[PLAYER_DRACULA][i]]==TRUE){
                        // printf("Safe Location %s\n",idToName(path[PLAYER_DRACULA][i]));
                        for(player=0;player<NUM_PLAYERS-1;player++){
                            for(int j=0;j<numLocations[player];j++){
                                tempDist=findPathDist(map,path[PLAYER_DRACULA][i],path[player][j]);
                                //            printf("Distance between %s and %s is %d\n", idToName(path[PLAYER_DRACULA][i]), idToName(path[player][j]), tempDist);
                                if(disLoc[path[PLAYER_DRACULA][i]]>tempDist){
                                    disLoc[path[PLAYER_DRACULA][i]]=tempDist;
                                }
                            }
                        }
                    }
                    
                }
                
                tempDist=0;
                
                for(int i=0;i<numLocations[PLAYER_DRACULA];i++) {
                    
                    if(avLoc[path[PLAYER_DRACULA][i]]==TRUE && disLoc[path[PLAYER_DRACULA][i]]>=tempDist){
                        
                        //    printf("Checking %d TempDist %d\n",path[PLAYER_DRACULA][i],tempDist);
                        
                        tempDist=disLoc[path[PLAYER_DRACULA][i]];
                        bestLoc=path[PLAYER_DRACULA][i];
                        //     printf("Checking %d TempDist %d bestLoc %d\n",path[PLAYER_DRACULA][i],tempDist,bestLoc);
                    }
                    
                }
                
            } else if(numLocations[player] == 0){
                printf("About to Teleport\n");
                teleported=TRUE;
                bestLoc=CASTLE_DRACULA;
            }
            
            if(bestLoc==UNKNOWN_LOCATION && !teleported){
                
                //  printf("OVERRIDE\n");
                bestLoc=path[player][0];
            }
            
            //what if there is no safe location?
            // choose any location that is atleast 1 hop away, dont go to place already occupied by hunter
            // Better to encounter player with least health
            
            
            
            //if bloodpoints < 10 move towards CD
            //need a function (pathToCD) that returns the next hop in path towards Castle Dracula
            //avoid SEA if blood points critically low ( < 4)
        }
        
    }
    
    
    printf("best location %s with distance %d from any dangerous location\n",idToName(bestLoc),tempDist);
    
    
    disposeMap(map);
    
    
    
    //if bestLoc is in trail then check for Hide and DoubleBack
    //else you can set bestLoc as next move
    int inTrail=checkTrail(trail,bestLoc);
    
    if(inTrail==TRUE && !teleported){
        //has to be either Hide or DoubleBack or the first location as bestLoc is in trail
        if(hideOrDB(gameState, trail, bestLoc)!=NULL)
            registerBestPlay(hideOrDB(gameState, trail, bestLoc), "Catch me if you can0");
        else
            registerBestPlay(idToAbbrev(path[player][1]), "Catch me if you can1");    ;
        
    }else if(inTrail==FALSE && !teleported){
        registerBestPlay(idToAbbrev(bestLoc),"Catch me if you can2");
    }
    else{
        registerBestPlay("TP","Catch me if you can");
    }
    
    
}



static int checkTrail(int trail[TRAIL_SIZE], int bestLoc){
    int result = FALSE;
    for (int i = 0; i < TRAIL_SIZE-1; i++) {
        if (bestLoc == trail[i]) {
            result = TRUE;
        }
    }
    return result;
    
}


static char* hideOrDB(DracView gameState,int trail[TRAIL_SIZE], int bestLoc){
    
    //dracula not allowed to hide at SEA CHECK
    
    char* nextMove = NULL;
    int i=0;
    int doubleBack = FALSE;
    int hide = FALSE;
    int foundIt;
    int currentLoc=whereIs(gameState, PLAYER_DRACULA);
    
    
    while(i < TRAIL_SIZE-1){
        /*  if(trail[i]<=MAX_MAP_LOCATION && trail[i]>= MIN_MAP_LOCATION)
         printf("TRAIL %s\n",idToAbbrev(trail[i]));
         else
         printf("TRAIL %d\n",trail[i]);
         */
        if (trail[i] == HIDE) {
            hide = TRUE;
        } else if (trail[i] >= DOUBLE_BACK_1 && trail[i] <= DOUBLE_BACK_5) {
            doubleBack = TRUE;
        }
        i++;
    }
    
    // printf("Location is %d current Loc %d\n",bestLoc, currentLoc);
    
    for (i = 0; i < TRAIL_SIZE-1; i++) {
        if (bestLoc == trail[i]) {
            foundIt = i;
            break;
        }
    }
    
    
    if (doubleBack && isSea(currentLoc)==FALSE) {
        nextMove = "HI";
    } else if (hide) {
        switch (foundIt) {
            case 0:
                nextMove = "D1";
                break;
            case 1:
                nextMove = "D2";
                break;
            case 2:
                nextMove = "D3";
                
                break;
            case 3:
                nextMove = "D4";
                break;
            case 4:
                nextMove = "D5";
                break;
        }
    } else {
        if (trail[0] == bestLoc && isSea(currentLoc)==FALSE) {
            nextMove = "HI";
        } else {
            switch (foundIt) {
                case 0:
                    nextMove = "D1";
                    break;
                case 1:
                    nextMove = "D2";
                    break;
                case 2:
                    nextMove = "D3";
                    printf("I am here\n");
                    break;
                case 3:
                    nextMove = "D4";
                    break;
                case 4:
                    nextMove = "D5";
                    break;
            }
        }
    }
    
    return nextMove;
    
    
    
}






