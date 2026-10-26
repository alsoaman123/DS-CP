#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "quest.h"

/* Run in a temporary directory: the persistence API uses fixed file names. */
int main(void)
{
    QUEST first = {0}, second = {0};
    first.questID = 1;
    strcpy(first.questName, "Rescue NPC");
    strcpy(first.questDesc, "Free the captive");
    first.rewardXP = 200;
    first.rewardGold = 60;
    first.minLevel = 3;
    first.prerequisiteQuestID[0] = 7;
    first.prerequisiteQuestID_count = 1;
    first.prerequisiteItemsID[0] = 8;
    first.prerequisiteItemsID_count = 1;
    first.questUnlocked_Count = MAX_QUEST_UNLOCKED;
    for (int i = 0; i < MAX_QUEST_UNLOCKED; i++)
        first.questUnlocked[i] = 10 + i;
    first.inventoryItemsGiven_count = MAX_ITEMS_ADDED;
    for (int i = 0; i < MAX_ITEMS_ADDED; i++)
        first.inventoryItemsGiven[i] = 50 + i;
    second.questID = 2; /* Empty strings and lists must survive reloading. */
    first.next = &second;
    second.prev = &first;
    QuestList original = {&first, &second};
    QuestList loaded = {0};

    assert(MAX_QUEST_UNLOCKED == 20);
    assert(writeQuestsInFile(&original) == 1);
    assert(getQuestsInMemory(&loaded) == 2);
    const QUEST *q = loaded.head;
    assert(q != NULL && q->questID == 1);
    assert(strcmp(q->questName, first.questName) == 0);
    assert(strcmp(q->questDesc, first.questDesc) == 0);
    assert(q->rewardXP == 200 && q->rewardGold == 60 && q->minLevel == 3);
    assert(q->prerequisiteQuestID_count == 1 && q->prerequisiteQuestID[0] == 7);
    assert(q->prerequisiteItemsID_count == 1 && q->prerequisiteItemsID[0] == 8);
    assert(q->questUnlocked_Count == MAX_QUEST_UNLOCKED);
    assert(memcmp(q->questUnlocked, first.questUnlocked, sizeof(first.questUnlocked)) == 0);
    assert(q->inventoryItemsGiven_count == MAX_ITEMS_ADDED);
    assert(memcmp(q->inventoryItemsGiven, first.inventoryItemsGiven, sizeof(first.inventoryItemsGiven)) == 0);
    q = q->next;
    assert(q != NULL && q == loaded.tail && q->prev == loaded.head);
    assert(q->questID == 2 && q->questName[0] == '\0' && q->questDesc[0] == '\0');
    assert(q->prerequisiteQuestID_count == 0 && q->prerequisiteItemsID_count == 0);
    assert(q->questUnlocked_Count == 0 && q->inventoryItemsGiven_count == 0);
    assert(q->next == NULL);
    assert(writeQuestsInFile(&loaded) == 1); /* Replace an existing save. */
    freeQuests(&loaded);
    assert(getQuestsInMemory(&loaded) == 2);
    assert(loaded.head->questUnlocked_Count == MAX_QUEST_UNLOCKED);
    assert(loaded.head->inventoryItemsGiven_count == MAX_ITEMS_ADDED);
    freeQuests(&loaded);
    assert(loaded.head == NULL && loaded.tail == NULL);
    assert(remove(QUESTS_FILE) == 0);
    puts("Quest persistence round-trip passed.");
    return 0;
}
