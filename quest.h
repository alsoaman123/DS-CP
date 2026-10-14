#ifndef QUEST_H
#define QUEST_H

#define MAX_NAME_LEN 64
#define MAX_DESC_LEN 256
#define MAX_PRE_Q 3
#define MAX_PRE_ITEMS 5
#define MAX_QUEST_UNLOCKED 3

typedef enum QuestStatus {QUEST_LOCKED, QUEST_AVAILABLE, QUEST_COMPLETED} QuestStatus;

typedef struct item{
    int itemID;
    char itemName[MAX_NAME_LEN];
} ITEMS ;

typedef struct quest {
    int questID;
    char questName[MAX_NAME_LEN];
    char questDesc[MAX_DESC_LEN];

    int rewardXP;
    int rewardGold;
    int minLevel;

    // stores the ID needed to unlock this quest
    int prerequisiteQuestID[MAX_PRE_Q];
    int prerequisiteQuestID_count;
    int prerequisiteItemsID[MAX_PRE_ITEMS];
    int prerequisiteItemsID_count;
 
    //  stores the ID this quest unlocks
    int questUnlocked[MAX_QUEST_UNLOCKED];
    int questUnlocked_Count;

    //will store address in sequential order. (ex. questID 2 will store questIQ 1 address in prev and of 3 in next)
    struct quest *prev;
    struct quest *next;

} QUEST;

//The add quest function will create node of the quest and link it appropriately in linked list. will first get all quest details then search the linked list to add new quest at the end.
void addQuest (QUEST *start, QUEST *end);

//Always run at start of backend.c and get all the quests from text file in proper linked list. will return the start pointer of quests (1st quest address). *h given to this function will be temporary work variable which will initially be NULL.
QUEST* getQuestsInMemory ();

//runs when admin wants to exit backend.c and writes the linked list in order to the quests.txt file
void writeQuestsInFile (QUEST *start, QUEST *end);

//gives a nice structured output for the admin to see all the quests
void viewAllQuests (QUEST *start, QUEST *end);

//for admin to see a particular quest they want.
QUEST* findQuestByID (QUEST *start, int id, QUEST *end);


//Updates inventory file (if new item is added)
void updateInventoryItems (ITEMS *items_start, ITEMS *items_end);

//get inventory items from file into a linked list (at start of backend.c)
void loadInventoryItems (ITEMS *items_start, ITEMS *items_end);

//create new inventory items in the linked list
void createInventoryItems (ITEMS *items_start, ITEMS *items_end);

//will be used while showing quests to player and backend by using itemID to get the itemName from the inventory linked list.
void getItemName (ITEMS *items_start, int itemID, char *itemName);

#endif 
