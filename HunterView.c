// HunterView.c ... HunterView ADT implementation
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include <string.h>


//Rimmal

struct hunterView {
    
    GameView gameView;
    
};


//Rimmal
// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[])
{
    assert(messages !=NULL);
    assert(pastPlays != NULL);
    
    HunterView hView = malloc(sizeof(struct hunterView));
    assert(hView != NULL);
    hView->gameView = newGameView(pastPlays, messages);
    assert(hView->gameView != NULL);
    
    return hView;
}

//Rimmal -done
// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    
    assert(toBeDeleted != NULL);
    disposeGameView(toBeDeleted->gameView);
    free( toBeDeleted );
    
}


//// Functions to return simple information about the current state of the game
//Vivian - done
// Get the current round
Round giveMeTheRound(HunterView currentView)
{
    return getRound(currentView->gameView);
}
//Yukti -- done
// Get the id of current player
PlayerID whoAmI(HunterView currentView)
{
    return getCurrentPlayer(currentView->gameView);
}

//Jessie -- done
// Get the current score
int giveMeTheScore(HunterView currentView)
{
    return getScore(currentView->gameView);
}


//Jessie -- done
// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player)
{
    assert(player < NUM_PLAYERS);
    return getHealth(currentView->gameView, player);
}

//Yukti -- done
// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player)
{
    assert(player < NUM_PLAYERS);
    LocationID loc;
    
    loc=getLocation(currentView->gameView, player);
    
    // printf("HunterView location %d\n",loc);
    
    /* if(player==PLAYER_DRACULA && loc != UNKNOWN_LOCATION){
     if(idToType(loc) == SEA){
     loc = SEA_UNKNOWN;
     }else if (idToType(loc) == LAND) {
     loc = CITY_UNKNOWN;
     }
     }*/
    
    return loc;
}


//// Functions that return information about the history of the game

//Yukti -- done
// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player,
                    LocationID trail[TRAIL_SIZE])
{
    assert(player < NUM_PLAYERS);
    getHistory(currentView->gameView, player, trail);
}

//// Functions that query the map to find information about connectivity


//Vivian -- done
// What are my (player's) possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail, int sea)
{
    PlayerID player = whoAmI(currentView);
    LocationID from = whereIs(currentView, player);
    Round round = giveMeTheRound(currentView);
    return connectedLocations(currentView->gameView, numLocations, from, player, round, road, rail, sea);
}

//Vivian -- done
// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    LocationID from = whereIs(currentView, player);
    Round round = giveMeTheRound(currentView);
    if(player==PLAYER_DRACULA)
    {
        return connectedLocations(currentView->gameView, numLocations, from, player, round, 0, 0, 0);
    }
    else
    {
        return connectedLocations(currentView->gameView, numLocations, from, player, round, road, rail, sea);
    }
}











