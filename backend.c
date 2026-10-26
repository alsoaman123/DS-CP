#include <stdio.h>
#include "quest.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/* ---------- Inventory functions ---------- */

int loadInventoryItems(ItemList *items)
{
    if (items == NULL)
        return -1;

    items->head = NULL;
    items->tail = NULL;

    FILE *fp = fopen(ITEMS_FILE, "r");

    if (fp == NULL) {
        if (errno == ENOENT)
            return 0;

        perror("Error opening items file");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int loadedCount = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {

        /* Remove newline characters */
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0')
            continue;

        /* Split the line into item ID and item name */
        char *separator = strchr(line, '|');

        if (separator == NULL || separator == line) {
            fprintf(stderr, "Invalid item record.\n");
            goto load_error;
        }

        *separator = '\0';

        char *idText = line;
        char *nameText = separator + 1;

        /* Validate the item ID */
        char *end;
        errno = 0;
        long id = strtol(idText, &end, 10);

        if (errno != 0 || *end != '\0' ||
            id <= 0 || id > INT_MAX) {
            fprintf(stderr, "Invalid item ID.\n");
            goto load_error;
        }

        /* Validate the item name */
        if (nameText[0] == '\0' ||
            strlen(nameText) >= MAX_NAME_LEN) {
            fprintf(stderr, "Invalid item name.\n");
            goto load_error;
        }

        /* Check for duplicate item IDs */
        for (ITEMS *current = items->head;
             current != NULL;
             current = current->next) {

            if (current->itemID == (int)id) {
                fprintf(stderr, "Duplicate item ID.\n");
                goto load_error;
            }
        }

        /* Create and initialize the new item node */
        ITEMS *newItem = malloc(sizeof(ITEMS));

        if (newItem == NULL) {
            perror("Memory allocation failed");
            goto load_error;
        }

        newItem->itemID = (int)id;
        strcpy(newItem->itemName, nameText);
        newItem->next = NULL;

        /* Append the node to the linked list */
        if (items->head == NULL) {
            items->head = newItem;
            items->tail = newItem;
        } else {
            items->tail->next = newItem;
            items->tail = newItem;
        }

        loadedCount++;
    }

    if (ferror(fp)) {
        perror("Error reading items file");
        goto load_error;
    }

    fclose(fp);
    return loadedCount;

load_error:
    fclose(fp);

    /* Free nodes already loaded if an error occurs */
    ITEMS *current = items->head;

    while (current != NULL) {
        ITEMS *next = current->next;
        free(current);
        current = next;
    }

    items->head = NULL;
    items->tail = NULL;

    return -1;
}

int createInventoryItems(ItemList *items)
{
    if (items == NULL)
        return 0;

    ITEMS *newItem = malloc(sizeof(ITEMS));
    if (newItem == NULL) {
        perror("Memory allocation failed");
        return 0;
    }

    printf("\nEnter Inventory Item ID: ");
    if (scanf("%d", &newItem->itemID) != 1 || newItem->itemID <= 0) {
        free(newItem);
        printf("Invalid item ID.\n");
        return 0;
    }

    printf("Enter Inventory Item Name: ");
    if (scanf(" %63[^\n]", newItem->itemName) != 1) {
        free(newItem);
        printf("Invalid item name.\n");
        return 0;
    }

    newItem->next = NULL;

    if (items->head == NULL) {
        items->head = newItem;
        items->tail = newItem;
    } else {
        items->tail->next = newItem;
        items->tail = newItem;
    }

    return 1;
}

int updateInventoryItems(const ItemList *items)
{
    if (items == NULL)
        return 0;

    /* Write to a temporary file first to protect the original file. */
    FILE *fp = fopen(ITEMS_TMP_FILE, "w");

    if (fp == NULL) {
        perror("Could not open temporary items file");
        return 0;
    }

    /* Write each item as: itemID|itemName */
    const ITEMS *current = items->head;

    while (current != NULL) {
        if (fprintf(fp, "%d|%s\n",
                    current->itemID,
                    current->itemName) < 0) {
            fprintf(stderr, "Error writing inventory items.\n");
            fclose(fp);
            remove(ITEMS_TMP_FILE);
            return 0;
        }

        current = current->next;
    }

    /* Check for errors while flushing and closing the file. */
    int failed = ferror(fp);

    if (fclose(fp) != 0)
        failed = 1;

    if (failed) {
        fprintf(stderr, "Error saving inventory items.\n");
        remove(ITEMS_TMP_FILE);
        return 0;
    }

    /* Replace the original file with the completed temporary file. */
#ifdef _WIN32
    remove(ITEMS_FILE);
#endif

    if (rename(ITEMS_TMP_FILE, ITEMS_FILE) != 0) {
        perror("Could not replace items file");
        return 0;
    }

    return 1;
}

ITEMS *findItemByID(const ItemList *items, int id)
{
    /* Search for an item by ID. */

    if (items == NULL)
        return NULL;

    ITEMS *temp = items->head;
    int flag = 0;

    while(temp != NULL){

        if(temp -> itemID == id){
            printf("Item Found!\n");
            printf("Item ID : %d", (temp -> itemID));
            printf("Item Name : %s", (temp -> itemName));
            flag = 1;
            break;
        }

        temp = temp -> next;

    }

    if(flag == 0){
        printf("Item Not Found!\n");
        return NULL;
    }
    else{
        return temp;
    }

}

void viewInventoryItems(const ItemList *items)
{
    /* Display all inventory items. */

    if (items == NULL)
        return;

    ITEMS *temp = items -> head;

    while(temp != NULL){
        printf("Item ID : %d\n", (temp -> itemID));
        printf("Item Name : %s\n", (temp -> itemName));
        printf("\n");

        temp = temp -> next;
    }

}

void getItemName(const ItemList *items, int ID, char *itemName)
{
    if (itemName == NULL)
        return;

    itemName[0] = '\0';
    if (items == NULL)
        return;

    ITEMS *temp = items->head;
    while (temp != NULL) {
        if (temp->itemID == ID) {
            strcpy(itemName, temp->itemName);
            return;
        }
        temp = temp->next;
    }

    strcpy(itemName, "Unknown item");
}

void freeInventoryItems(ItemList *items)
{
    if (items == NULL)
        return;

    ITEMS *current = items->head;
    while (current != NULL) {
        ITEMS *next = current->next;
        free(current);
        current = next;
    }

    items->head = NULL;
    items->tail = NULL;
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

        char *fields[10];
        int fieldCount = 0;

        char *token = strtok(line, FIELD_SEP);

        while (token != NULL && fieldCount < 10) {
            fields[fieldCount++] = token;
            token = strtok(NULL, FIELD_SEP);
        }

        if (fieldCount != 10) {
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

        /* Inventory Items awarded to player by this quest */
        /* Items awarded when this quest is completed */
        if (strcmp(fields[9], EMPTY_LIST_MARK) != 0) {
            token = strtok(fields[9], LIST_SEP);

            while (token != NULL) {
                if (temp.inventoryItemsGiven_count >= MAX_ITEMS_ADDED)
                    goto invalid_record;

                temp.inventoryItemsGiven[
                    temp.inventoryItemsGiven_count
                ] = (int)strtol(token, &end, 10);

                if (*end != '\0' ||
                    temp.inventoryItemsGiven[
                        temp.inventoryItemsGiven_count
                    ] <= 0)
                    goto invalid_record;

                temp.inventoryItemsGiven_count++;
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

int addQuest(QuestList *quests, const ItemList *items)
{
    (void)items; /* Reserved for validating prerequisite item IDs. */

    if (quests == NULL)
        return 0;

    QUEST *newQuest = calloc(1, sizeof(QUEST));
    if (newQuest == NULL) {
        perror("Memory allocation failed");
        return 0;
    }

    printf("Enter Quest ID: ");
    if (scanf("%d", &newQuest->questID) != 1 || newQuest->questID <= 0)
        goto input_error;

    /* Reject duplicate quest IDs. */
    for (QUEST *current = quests->head; current != NULL; current = current->next) {
        if (current->questID == newQuest->questID) {
            printf("A quest with this ID already exists.\n");
            goto input_error;
        }
    }

    printf("Enter Quest Name: ");
    if (scanf(" %63[^\n]", newQuest->questName) != 1)
        goto input_error;

    printf("Enter Quest Description: ");
    if (scanf(" %255[^\n]", newQuest->questDesc) != 1)
        goto input_error;

    printf("Enter Reward XP: ");
    if (scanf("%d", &newQuest->rewardXP) != 1 || newQuest->rewardXP < 0)
        goto input_error;

    printf("Enter Reward Gold: ");
    if (scanf("%d", &newQuest->rewardGold) != 1 || newQuest->rewardGold < 0)
        goto input_error;

    printf("Enter minimum level: ");
    if (scanf("%d", &newQuest->minLevel) != 1 || newQuest->minLevel < 0)
        goto input_error;

    int count;
    printf("Enter number of prerequisite quests (max = %d): ", MAX_PRE_Q);
    if (scanf("%d", &count) != 1 || count < 0 || count > MAX_PRE_Q)
        goto input_error;
    newQuest->prerequisiteQuestID_count = count;

    for (int i = 0; i < count; i++) {
        printf("Enter prerequisite quest ID %d: ", i + 1);
        if (scanf("%d", &newQuest->prerequisiteQuestID[i]) != 1 ||
            newQuest->prerequisiteQuestID[i] <= 0)
            goto input_error;
    }

    printf("Enter number of prerequisite items (max = %d): ", MAX_PRE_ITEMS);
    if (scanf("%d", &count) != 1 || count < 0 || count > MAX_PRE_ITEMS)
        goto input_error;
    newQuest->prerequisiteItemsID_count = count;

    for (int i = 0; i < count; i++) {
        printf("Enter prerequisite item ID %d: ", i + 1);
        if (scanf("%d", &newQuest->prerequisiteItemsID[i]) != 1 ||
            newQuest->prerequisiteItemsID[i] <= 0)
            goto input_error;
    }

    printf("Enter number of quests unlocked (max = %d): ", MAX_QUEST_UNLOCKED);
    if (scanf("%d", &count) != 1 || count < 0 || count > MAX_QUEST_UNLOCKED)
        goto input_error;
    newQuest->questUnlocked_Count = count;

    for (int i = 0; i < count; i++) {
        printf("Enter unlocked quest ID %d: ", i + 1);
        if (scanf("%d", &newQuest->questUnlocked[i]) != 1 ||
            newQuest->questUnlocked[i] <= 0)
            goto input_error;
    }

    printf("Enter number of reward items (max = %d): ", MAX_ITEMS_ADDED);

    if (scanf("%d", &count) != 1 || count < 0 || count > MAX_ITEMS_ADDED)
        goto input_error;
    newQuest->inventoryItemsGiven_count = count;

    for (int i = 0; i < count; i++) {
        printf("Enter reward item ID %d: ", i + 1);

        if (scanf("%d", &newQuest->inventoryItemsGiven[i]) != 1 ||
            newQuest->inventoryItemsGiven[i] <= 0)
            goto input_error;
    }

    newQuest->prev = quests->tail;
    newQuest->next = NULL;

    if (quests->head == NULL)
        quests->head = newQuest;
    else
        quests->tail->next = newQuest;

    quests->tail = newQuest;
    return 1;

input_error:
    printf("Invalid input. Quest was not added.\n");
    free(newQuest);
    return 0;
}

/* Writes one list of IDs, or "-" if the list is empty. */
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
          questID|questName|questDesc|rewardXP|rewardGold|minLevel|prereqQuestIDs|prereqItemIDs|questUnlockedIDs|rewardItemIDs */
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
        fputs(FIELD_SEP, fp);
        writeIdList(fp, q->inventoryItemsGiven, q->inventoryItemsGiven_count);
        fputc('\n', fp);
        /* questUnlocked is stored as the ninth field. */
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
    printf("\n");
    
    printf("Items Awarded on Completion: ");
    for (i = 0; i < q->inventoryItemsGiven_count; i++) {
        char name[MAX_NAME_LEN];

        getItemName(items, q->inventoryItemsGiven[i], name);
        printf("%s\t", name);
    }

    printf("\n\n");

}

void viewAllQuests(const QuestList *quests, const ItemList *items){

    if (quests == NULL)
        return;

    QUEST *temp = quests->head;

    while (temp != NULL) {
        
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

void freeQuests(QuestList *quests)
{
    if (quests == NULL)
        return;

    QUEST *current = quests->head;
    while (current != NULL) {
        QUEST *next = current->next;
        free(current);
        current = next;
    }

    quests->head = NULL;
    quests->tail = NULL;
}
