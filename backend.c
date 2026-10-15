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

/* Writes one list of IDs, e.g. "3,4". Writes "-" if the list is empty. */
static void writeIdList(FILE *fp, const int ids[], int count)
{
    if (count == 0) {
        fputs(EMPTY_LIST_MARK, fp);
        return;
    }
    for (int i = 0; i < count; i++) {
        if (i > 0)
            fputs(LIST_SEP, fp);      /* comma before every ID except the first */
        fprintf(fp, "%d", ids[i]);
    }
}

/* Writes a text field, or "-" if it is empty (strtok would skip an empty field). */
static void writeTextField(FILE *fp, const char *text)
{
    fputs(text[0] == '\0' ? EMPTY_LIST_MARK : text, fp);
}

int writeQuestsInFile(const QuestList *quests)
{
    if (quests == NULL)
        return 0;

    /* 1. Write to the temp file, never straight to the real one. */
    FILE *fp = fopen(QUESTS_TMP_FILE, "w");
    if (fp == NULL) {
        perror("Could not open " QUESTS_TMP_FILE);
        return 0;
    }

    /* 2. One line per quest, in list order:
          questID|questName|questDesc|rewardXP|rewardGold|minLevel|prereqQuestIDs|prereqItemIDs */
    for (const QUEST *q = quests->head; q != NULL; q = q->next) {
        fprintf(fp, "%d" FIELD_SEP, q->questID);
        writeTextField(fp, q->questName);
        fputs(FIELD_SEP, fp);
        writeTextField(fp, q->questDesc);
        fprintf(fp, FIELD_SEP "%d" FIELD_SEP "%d" FIELD_SEP "%d" FIELD_SEP,
                q->rewardXP, q->rewardGold, q->minLevel);
        writeIdList(fp, q->prerequisiteQuestID, q->prerequisiteQuestID_count);
        fputs(FIELD_SEP, fp);
        writeIdList(fp, q->prerequisiteItemsID, q->prerequisiteItemsID_count);
        fputc('\n', fp);
        /* questUnlocked is NOT written: it is rebuilt when the file is loaded. */
    }

    /* 3. Check for write errors. fclose flushes the buffer, so check it too. */
    int failed = ferror(fp);
    if (fclose(fp) != 0)
        failed = 1;

    if (failed) {
        fprintf(stderr, "Error while writing quests; the old file is unchanged.\n");
        remove(QUESTS_TMP_FILE);
        return 0;
    }

    /* 4. Swap the temp file in. Windows rename fails if the target exists. */
#ifdef _WIN32
    remove(QUESTS_FILE);
#endif
    if (rename(QUESTS_TMP_FILE, QUESTS_FILE) != 0) {
        perror("Could not replace " QUESTS_FILE);
        return 0;
    }
    return 1;
}

void viewQuest(const QUEST *q, const ItemList *items){

}

void viewAllQuests(const QuestList *quests, const ItemList *items){

}

QUEST *findQuestByID(const QuestList *quests, int id){

}

void freeQuests(QuestList *quests){

}