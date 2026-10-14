#include <stdio.h>
#include "quest.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/* ---------- Inventory functions ---------- */

int loadInventoryItems(ItemList *items)
{
    /* Read items.txt and build the linked list. */
}

int createInventoryItems(ItemList *items)
{
    /* Get item details, create a node, and save. */
}

int updateInventoryItems(const ItemList *items)
{
    /* Save the linked list to items.txt safely. */
}

ITEMS *findItemByID(const ItemList *items, int id)
{
    /* Search for an item by ID. */
}

void viewInventoryItems(const ItemList *items)
{
    /* Display all inventory items. */
}

void getItemName(const ItemList *items, int itemID, char *itemName)
{
    /* Look up an item's name using its ID. */
}

void freeInventoryItems(ItemList *items)
{
    /* Release allocated inventory nodes. */
}



/* ---------- Quest functions ---------- */

int getQuestsInMemory(QuestList *quests){

}

int addQuest(QuestList *quests, const ItemList *items){

}

int writeQuestsInFile(const QuestList *quests){

}

void viewQuest(const QUEST *q, const ItemList *items){

}

void viewAllQuests(const QuestList *quests, const ItemList *items){

}

QUEST *findQuestByID(const QuestList *quests, int id){

}

void freeQuests(QuestList *quests){

}