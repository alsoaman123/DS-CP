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







/*---------------------------------------*/
/* ---------- Quest functions ---------- */

int getQuestsInMemory(QuestList *quests)
{
    if (quests == NULL)
        return -1;

    quests->head = NULL;
    quests->tail = NULL;

    FILE *fp = fopen(QUESTS_FILE, "r");

    if (fp == NULL) {
        if (errno == ENOENT)
            return 0;

        perror("Error opening quests file");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int loadedCount = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {

        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0')
            continue;

        QUEST temp = {0};

        char *fields[9];
        int fieldCount = 0;

        char *token = strtok(line, FIELD_SEP);

        while (token != NULL && fieldCount < 9) {
            fields[fieldCount++] = token;
            token = strtok(NULL, FIELD_SEP);
        }

        if (fieldCount != 9) {
            fprintf(stderr, "Invalid quest record.\n");
            goto load_error;
        }

        /* Basic numeric fields */
        char *end;

        temp.questID = (int)strtol(fields[0], &end, 10);
        if (*end != '\0' || temp.questID <= 0)
            goto invalid_record;

        if (strcmp(fields[1], EMPTY_LIST_MARK) != 0) {
            if (strlen(fields[1]) >= MAX_NAME_LEN)
                goto invalid_record;

            strcpy(temp.questName, fields[1]);
        }

        if (strcmp(fields[2], EMPTY_LIST_MARK) != 0) {
            if (strlen(fields[2]) >= MAX_DESC_LEN)
                goto invalid_record;

            strcpy(temp.questDesc, fields[2]);
        }

        temp.rewardXP = (int)strtol(fields[3], &end, 10);
        if (*end != '\0' || temp.rewardXP < 0)
            goto invalid_record;

        temp.rewardGold = (int)strtol(fields[4], &end, 10);
        if (*end != '\0' || temp.rewardGold < 0)
            goto invalid_record;

        temp.minLevel = (int)strtol(fields[5], &end, 10);
        if (*end != '\0' || temp.minLevel < 0)
            goto invalid_record;

        /* Prerequisite quest IDs */
        if (strcmp(fields[6], EMPTY_LIST_MARK) != 0) {
            token = strtok(fields[6], LIST_SEP);

            while (token != NULL) {
                if (temp.prerequisiteQuestID_count >= MAX_PRE_Q)
                    goto invalid_record;

                temp.prerequisiteQuestID[
                    temp.prerequisiteQuestID_count
                ] = (int)strtol(token, &end, 10);

                if (*end != '\0' ||
                    temp.prerequisiteQuestID[
                        temp.prerequisiteQuestID_count
                    ] <= 0)
                    goto invalid_record;

                temp.prerequisiteQuestID_count++;
                token = strtok(NULL, LIST_SEP);
            }
        }

        /* Prerequisite item IDs */
        if (strcmp(fields[7], EMPTY_LIST_MARK) != 0) {
            token = strtok(fields[7], LIST_SEP);

            while (token != NULL) {
                if (temp.prerequisiteItemsID_count >= MAX_PRE_ITEMS)
                    goto invalid_record;

                temp.prerequisiteItemsID[
                    temp.prerequisiteItemsID_count
                ] = (int)strtol(token, &end, 10);

                if (*end != '\0' ||
                    temp.prerequisiteItemsID[
                        temp.prerequisiteItemsID_count
                    ] <= 0)
                    goto invalid_record;

                temp.prerequisiteItemsID_count++;
                token = strtok(NULL, LIST_SEP);
            }
        }

        /* Quest IDs unlocked by this quest */
        if (strcmp(fields[8], EMPTY_LIST_MARK) != 0) {
            token = strtok(fields[8], LIST_SEP);

            while (token != NULL) {
                if (temp.questUnlocked_Count >= MAX_QUEST_UNLOCKED)
                    goto invalid_record;

                temp.questUnlocked[
                    temp.questUnlocked_Count
                ] = (int)strtol(token, &end, 10);

                if (*end != '\0' ||
                    temp.questUnlocked[temp.questUnlocked_Count] <= 0)
                    goto invalid_record;

                temp.questUnlocked_Count++;
                token = strtok(NULL, LIST_SEP);
            }
        }

        /* Allocate a node and copy the temporary struct into it */
        QUEST *newQuest = malloc(sizeof(QUEST));

        if (newQuest == NULL) {
            perror("Memory allocation failed");
            goto load_error;
        }

        *newQuest = temp;
        newQuest->prev = quests->tail;
        newQuest->next = NULL;

        if (quests->head == NULL) {
            quests->head = newQuest;
        } else {
            quests->tail->next = newQuest;
        }

        quests->tail = newQuest;
        loadedCount++;

        continue;

    invalid_record:
        fprintf(stderr, "Invalid data in quest record.\n");

    load_error:
        fclose(fp);

        QUEST *current = quests->head;
        while (current != NULL) {
            QUEST *next = current->next;
            free(current);
            current = next;
        }

        quests->head = NULL;
        quests->tail = NULL;

        return -1;
    }

    if (ferror(fp)) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return loadedCount;
}

int addQuest(QuestList *quests, const ItemList *items){

    int i;
    QUEST *new = malloc(sizeof(QUEST));

    printf("Enter Quest ID : ");
    scanf("%d", &(new -> questID));
    printf("\n");

    printf("Enter Quest Name : ");
    gets( (new -> questName ));
    printf("\n");

    printf("Enter Quest Description : ");
    gets( (new -> questDesc ));
    printf("\n");

    printf("Enter Reward XP for Quest : ");
    scanf("%d", &(new -> rewardXP));

    printf("Enter Reward Gold for Quest : ");
    scanf("%d", &(new -> rewardGold));

    printf("Enter minimum level for Quest : ");
    scanf("%d", &(new -> minLevel));

    int count = 0;

    printf("Enter number of prerequisite quests (max = %d): ", MAX_PRE_Q);
    scanf("%d", &count);
    (new -> prerequisiteQuestID_count) = count;

    if(count > MAX_PRE_Q){
        printf("Error: Maximum number of prerequisite quests exceeded.\n");
        return 0;
    }

    for(i = 0; i < count; i++){
        printf("Enter prerequisite quest ID %d: ", i + 1);
        scanf("%d", &(new -> prerequisiteQuestID[i]));
    }

    printf("Enter number of prerequisite items (max = %d): ", MAX_PRE_ITEMS);
    scanf("%d", &count);
    (new -> prerequisiteItemsID_count) = count;
    if(count > MAX_PRE_ITEMS){
        printf("Error: Maximum number of prerequisite items exceeded.\n");
        return 0;
    }

    for(i = 0; i < count; i++){
        printf("Enter prerequisite item ID %d: ", i + 1);
        scanf("%d", &(new -> prerequisiteItemsID[i]));
    }

    printf("Enter number of quests unlocked by this quest (max = %d): ", MAX_QUEST_UNLOCKED);
    scanf("%d", &count);
    (new -> questUnlocked_Count) = count;

    if(count > MAX_QUEST_UNLOCKED){
        printf("Error: Maximum number of quests unlocked exceeded.\n");
        return 0;
    }

    for(i = 0; i < count; i++){
        printf("Enter quest ID unlocked by this quest %d: ", i + 1);
        scanf("%d", &(new -> questUnlocked[i]));
    }

    /* Add the new quest to the list */
    if(quests -> head == NULL){
        quests -> head = new;
        quests -> tail = new;

        quests -> head -> prev = NULL;
        quests -> head -> next = NULL;
    }
    else{
        new -> prev = quests -> tail;
        new -> next = NULL;
        quests -> tail -> next = new;

        quests -> tail = new;
    }

}

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

int writeQuestsInFile(const QuestList *quests){

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
        fprintf(fp, FIELD_SEP "%d" FIELD_SEP "%d" FIELD_SEP "%d" FIELD_SEP, q->rewardXP, q->rewardGold, q->minLevel);
        writeIdList(fp, q->prerequisiteQuestID, q->prerequisiteQuestID_count);
        fputs(FIELD_SEP, fp);
        writeIdList(fp, q->prerequisiteItemsID, q->prerequisiteItemsID_count);
        fputs(FIELD_SEP, fp);
        writeIdList(fp, q->questUnlocked, q->questUnlocked_Count);
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

void viewQuest(int qid, const QuestList *quests, const ItemList *items){

    int i;

    QUEST *q = findQuestByID(quests, qid);
    if (q == NULL) {
        fprintf(stderr, "Quest with ID %d not found.\n", qid);
        return;
    }

    // Implementation for viewing a single quest
    printf("\n");
    printf("Quest ID: %d\n", q->questID);
    printf("Quest Name: %s\n", q->questName);
    printf("Quest Description: %s\n", q->questDesc);
    printf("Reward XP: %d\n", q->rewardXP);
    printf("Reward Gold: %d\n", q->rewardGold);
    printf("Minimum Level: %d\n", q->minLevel);

    printf("Prerequisite Quests: ");
    for(i = 0; i < ( q -> prerequisiteQuestID_count ); i++){
        printf("%d\t", (q -> prerequisiteQuestID[i]));
    }
    printf("\n");

    printf("Prerequisite Items: ");
    for(i = 0; i < (q -> prerequisiteItemsID_count); i++){
        char name[MAX_NAME_LEN];
        getItemName(items, (q -> prerequisiteItemsID[i]), name);
        printf("%s\t", name);
    }
    printf("\n");

    printf("Quests Unlocked: ");
    for(i = 0; i < ( q -> questUnlocked_Count ); i++){
        printf("%d\t", (q -> questUnlocked[i]));
    }
    printf("\n\n");

}

void viewAllQuests(const QuestList *quests, const ItemList *items){

    QUEST *temp = quests -> head;

    while( temp != NULL){
        
        viewQuest((temp -> questID), quests, items);
        temp = temp -> next;

    }
    
}

QUEST *findQuestByID(const QuestList *quests, int id){

    QUEST *temp = quests -> head;
    while (temp != NULL) {
        if (temp -> questID == id) {
            return temp;
        }
        temp = temp -> next;
    }

    return NULL;

}

void freeQuests(QuestList *quests){

}
