// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include "Map.h"
#include <time.h>

static int moveRandomly(HunterView gameState);
LocationID lastLocationOfDracula(HunterView gameState);

/*Intial Basic Stratergies:
 *-Obtain the latest known dracula move to a legit place(not just 'SEA' or 'LAND')
 *-Find the closest city to that place from our whereCanIgo locations trail
 *-After evry 6 turns all the hunters rest and research to find dracula's location
 *-Try to post some hunters near castle dracula to catch him as he comes out
 *-Try to find immature vampires before they mature
 *-messagesss???
 */

void decideHunterMove(HunterView gameState)
{
    // TODO ...
    // Replace the line below by something better
    int player = whoAmI(gameState);
    int round = giveMeTheRound(gameState);
    int my_trail[TRAIL_SIZE];
    int drac_trail[TRAIL_SIZE];
    int moveTo=UNKNOWN_LOCATION;
    Map map=newMap();
    srand(time(NULL));
    char *msg="";
    LocationID currentLocation = whereIs(gameState, player);
    
    giveMeTheTrail(gameState,player,my_trail);
    giveMeTheTrail(gameState,PLAYER_DRACULA,drac_trail);
    LocationID lastDracLocation= lastLocationOfDracula(gameState);
    
    
    
    if (round == 0) {
        msg="Starting\n";
        if(player == PLAYER_LORD_GODALMING)
        {moveTo = MADRID;}
        else if(player == PLAYER_DR_SEWARD)
        {moveTo = BERLIN;}
        else if(player == PLAYER_VAN_HELSING)
        {moveTo = PARIS;}
        else if(player == PLAYER_MINA_HARKER)
        {moveTo = BELGRADE;}
        
    }else if (round % 6 == 0 || (round % 6==1 && player==PLAYER_LORD_GODALMING){ //resting sequence starts from the second player, so that when the (6th) location is revealed it is available for atleast 4 players before it falls off the trail
        moveTo=currentLocation;
        msg="Resting";
    }
    else {
        
        int numLocations =TRAIL_SIZE;
        int *possibleLocations = whereCanIgo(gameState,&numLocations,TRUE,TRUE,TRUE);
        
        if (lastDracLocation!=UNKNOWN_LOCATION) {
            moveTo=closestCity(map, lastDracLocation, possibleLocations, TRAIL_SIZE);
            msg="Following Dracula";
        }
        else {
            moveTo=moveRandomly(gameState);
            msg="Random";
        }
    }
    
    // printf("health of player %d is %d\n",player,howHealthyIs(gameState, player));
    if(howHealthyIs(gameState, player)>0){
        registerBestPlay(idToAbbrev(moveTo),msg);
    }
    else{
        registerBestPlay("JM","OOuch");
        
    }
    
}

LocationID lastLocationOfDracula(HunterView gameState) {
    
    
    LocationID draculaTrail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, draculaTrail);
    
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (draculaTrail[i] >= MIN_MAP_LOCATION && draculaTrail[i] <= MAX_MAP_LOCATION) {
            return draculaTrail[i];
        }
    }
    return UNKNOWN_LOCATION;
}

static int moveRandomly(HunterView gameState)
{
    int numLocations;
    int *locs = whereCanIgo(gameState,&numLocations,TRUE,TRUE,TRUE);
    int r = (rand()%(numLocations-1))+1;
    //   printf("Random Location %d\n",locs[r]);
    return locs[r];
}
