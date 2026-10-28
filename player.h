#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include "quest.h"

// ---------- Size limits ----------
#define MAX_PLAYER_NAME 64
#define MAX_INV_ITEMS 32
#define PLAYER_QUEST_COMPLETED 32
#define PLAYER_QUEST_UNLOCKED 32
#define PLAYER_QUEST_LOCKED 32

// ------ Data File Settings --------
#define PLAYER_FILE "players.txt"
#define PLAYER_TMP_FILE "players.tmp"


int QUEST_COUNT; //when getQuestInMemory called, this is updated and used for new player and where necessary;

// ------- Player Structure ---------
typedef struct player {

    char username[MAX_PLAYER_NAME];

    int XP;
    int level;
    int gold;

    int quest_completed[PLAYER_QUEST_COMPLETED]; //Quest Completed
    int quest_completed_count;

    int quest_unlocked[PLAYER_QUEST_UNLOCKED]; //Quest Available to play
    int quest_unlocked_count;

    int quest_locked[PLAYER_QUEST_LOCKED]; //Quest still locked
    int quest_locked_count;

    int inventory[MAX_INV_ITEMS];
    int inventory_count;

    struct player *next;
    struct player *prev;
} PLAYER;

typedef struct PlayerList{
    PLAYER *head;
    PLAYER *tail;
} PlayerList;

// ---- Current Players Quest Status ----
typedef struct PlayerQuestStatus {
    int qid;
    QuestStatus status;

    struct PlayerQuestStatus *next;
} PQS;

typedef struct PQS_List{
    PQS *head;
    PQS *tail;
} PQS_List;



// --------- Functions ------------

int getPlayersInMemory(PlayerList *players); //Will load all players from text file with their data into a DLL (at start of program)

void savePlayersInMemory(PlayerList *player); //Will save entire Players DLL into players.txt again (at end of program)

/*
Will find player by entered username in the DLL and store in a single seprate structure for current use.
This will also remove that player from DLL.
And when player exits, the struct will be appended at start of DLL again and everything saved in file (different function).
*/
void getCurrentPlayer(PlayerList *player, char username[], PLAYER *current); 

/*
Called when player wants to exit.
Players data inserted at start of DLL and struct cleared.
*/
void addCurrentPlayerInMemory(PlayerList *player, PLAYER *current);

/*
Will initialise a new player with necessary fields.
addCurrentPlayerInMmemory will be called when this new player wants to exit.
*/
void newPlayer(PLAYER *current);

/*
Will create SLL in sequence of quests DLL (created from getQuestInMemory from quest.h) and 
alongside will check in which array (locked, unlocked, completed) from the current player structure does the qid belong and 
update the enum status of that quest accordingly.

Called when getCurrentPlayer or newPlayer called (at end of those functions when struct is created)
*/
void generatePlayerQuestList(PLAYER *current, PQS_List *pqs_list, QuestList *quests); 

/*
Called at start of users playing and everytime he completes a quest.
Will show player XP, LEVEL, GOLD, Inventory Items and next available quest.
*/
void showPlayerData(PLAYER *current, PQS_List *pqs_list);

/*
Called when player wants to start next quest as per his available quests. 
If completed or locked qid given, we tell user so otherwise let him play by first checking all pre req things are present.

This function gets quests details from quests DLL as per qid and updates current player struct content (all things that are needed) and PQS_List once quest done.
Basically this function does the main work of updating quests status in PQS_List and change arrays in player struct, update inv items in player struct etc.
Calls showPlayerData at end.

Back to menu driven system of player once done.
*/
void PlayNextQuest(PLAYER *current, PQS_List *pqs_list, QuestList *quests, int qid);

void freePlayers(PlayerList *player);
void freePQSList(PQS_List *pqs_list);


#endif