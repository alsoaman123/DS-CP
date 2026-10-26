#include <stdio.h>
#include "quest.h"
#include <stdlib.h>

int main(){

    ItemList items = {NULL, NULL};
    QuestList quests = {NULL, NULL};

    loadInventoryItems(&items);
    getQuestsInMemory(&quests);

    int choice = 0;
    int tempID = 0;
    QUEST *tempQuest = NULL;

    while(1){
        printf("\nQuest Functions - \n");
        printf("1. View all quests\n");
        printf("2. View quest by ID\n");
        printf("3. Add new quest\n");

        printf("\nItems Functions - \n");
        printf("4. View all items\n");
        printf("5. Add new item\n");

        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf ("%d", &choice);

        switch(choice){
            case 1:
                viewAllQuests(&quests, &items);
                break;
            case 2:
                printf("\nEnter Quest ID to search: ");
                scanf("%d", &tempID);
                tempQuest = findQuestByID(&quests, tempID);

                if(tempQuest == NULL){
                    printf("Quest not found!\n");
                    break;
                }
                else{
                    viewQuest(tempID, &quests, &items);
                    break;
                }
                break;
            case 3:
                addQuest(&quests, &items);
                break;
            case 4:
                viewInventoryItems(&items);
                break;
            case 5:
                createInventoryItems(&items);
                break;
            case 6:
                updateInventoryItems(&items);
                writeQuestsInFile(&quests);
                freeInventoryItems(&items);
                freeQuests(&quests);
                
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }

    }

    return 0;

}