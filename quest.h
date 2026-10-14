#ifndef QUEST_H
#define QUEST_H

// ---------- Size limits ----------
#define MAX_NAME_LEN        64
#define MAX_DESC_LEN        256
#define MAX_PRE_Q           3
#define MAX_PRE_ITEMS       5
#define MAX_QUEST_UNLOCKED  3
#define MAX_LINE_LEN        1024   // buffer size for reading one line of a data file

// ---------- Data file settings ----------
// Shared by every program so the loader and the writer can never disagree.
#define QUESTS_FILE      "data/quests.txt"
#define QUESTS_TMP_FILE  "data/quests.tmp"   // same folder as the real file so rename() works
#define ITEMS_FILE       "data/items.txt"
#define ITEMS_TMP_FILE   "data/items.tmp"
#define FIELD_SEP        "|"   // separates fields on one line
#define LIST_SEP         ","   // separates ids inside a list field
#define EMPTY_LIST_MARK  "-"   // written in place of an empty list

// Per-player state. NOT stored in QUEST or quests.txt, because the same quest
// can be completed for one player and locked for another. player.c uses this.
typedef enum QuestStatus { QUEST_LOCKED, QUEST_AVAILABLE, QUEST_COMPLETED } QuestStatus;

// ---------- Inventory items ----------
typedef struct item {
    int itemID;
    char itemName[MAX_NAME_LEN];
    struct item *next;
} ITEMS;

// Handle for the whole item list. Create ONE of these in main() and pass its
// address to every item function, so changes to head/tail persist.
typedef struct {
    ITEMS *head;
    ITEMS *tail;
} ItemList;

// ---------- Quests ----------
typedef struct quest {
    int questID;
    char questName[MAX_NAME_LEN];
    char questDesc[MAX_DESC_LEN];

    int rewardXP;
    int rewardGold;
    int minLevel;

    // IDs of quests that must be completed before this one
    int prerequisiteQuestID[MAX_PRE_Q];
    int prerequisiteQuestID_count;

    // IDs of items the player must hold for this quest
    int prerequisiteItemsID[MAX_PRE_ITEMS];
    int prerequisiteItemsID_count;

    // IDs of quests this one unlocks (rebuilt after loading, not stored in the file)
    int questUnlocked[MAX_QUEST_UNLOCKED];
    int questUnlocked_Count;

    // Display order only; this is separate from the prerequisite links above
    struct quest *prev;
    struct quest *next;
} QUEST;

// Handle for the whole quest list (same idea as ItemList)
typedef struct {
    QUEST *head;
    QUEST *tail;
} QuestList;

// ---------- Return value conventions ----------
// load functions : number of records loaded (0 = file missing or empty), -1 = fatal error
// all other int functions : 1 = success, 0 = failure

// ---------- Inventory functions ----------

// Empties the list, then reads ITEMS_FILE into it. Run once at program start.
int loadInventoryItems(ItemList *items);

// Asks the admin for a new item name, assigns the next free ID, appends it to the list.
int createInventoryItems(ItemList *items);

// Writes the whole item list back to ITEMS_FILE. Call right after each successful create.
int updateInventoryItems(const ItemList *items);

// Returns the item with this ID, or NULL if it does not exist.
ITEMS *findItemByID(const ItemList *items, int id);

// Prints every item (ID and name) in aligned columns.
void viewInventoryItems(const ItemList *items);

// Copies the item's name into itemName, or "Unknown item" if the ID is not found.
// itemName MUST point to a buffer of at least MAX_NAME_LEN bytes.
void getItemName(const ItemList *items, int itemID, char *itemName);

// Releases every node and resets the list to empty. Call before exiting.
void freeInventoryItems(ItemList *items);

// ---------- Quest functions ----------

// Empties the list, reads QUESTS_FILE into it, then fills each quest's questUnlocked
// list from the prerequisite links. Run once at program start.
int getQuestsInMemory(QuestList *quests);

// Collects a new quest from the admin and appends it at the end of the list.
// Rejects it if the ID is a duplicate, a prerequisite quest or item does not exist,
// a limit is exceeded, or a prerequisite's questUnlocked list is full.
// Nothing is changed unless every check passes. Does not save; see writeQuestsInFile.
int addQuest(QuestList *quests, const ItemList *items);

// Writes the whole quest list, in order, to QUESTS_FILE. Run when the admin exits.
int writeQuestsInFile(const QuestList *quests);

// Prints one quest in full (item IDs shown as names).
void viewQuest(const QUEST *q, const ItemList *items);

// Prints every quest in list order.
void viewAllQuests(const QuestList *quests, const ItemList *items);

// Returns the quest with this ID, or NULL if it does not exist.
QUEST *findQuestByID(const QuestList *quests, int id);

// Releases every node and resets the list to empty. Call before exiting.
void freeQuests(QuestList *quests);

#endif