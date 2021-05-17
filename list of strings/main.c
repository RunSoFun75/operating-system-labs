#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct node {
    char *value;
    struct node *next;
};

struct node *nodeInit(){
    struct node *head;
    if ((head = (struct node *) malloc(sizeof(struct node))) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    head->next = NULL;
    head->value = NULL;
    return head;
}

void freeNode(struct node *currentNode){
    free(currentNode->value);
    free(currentNode);
    return;
}

struct node *addString(char *newLine){
    struct node *newNode;
    if ((newNode = (struct node *)malloc(sizeof(struct node))) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    if ((newNode->value = (char *)malloc(strlen(newLine) + 1)) == NULL) {
        perror("malloc returns NULL");
        exit(1);
    }
    strcpy(newNode->value, newLine);
    newNode->next = NULL;
    return(newNode);
}


int main(int argc, char *argv[]){
    char line[BUFSIZ];
    struct node *head = NULL,
            *currentNode = NULL,
            *i = NULL;

    head = nodeInit();
    currentNode = head;

    printf("Enter lines of text. To end entering, "
           "put '.' in the start of line.\n");

    while (gets(line) != NULL) {
        if (line[0] == '.') {
            break;
        }
        currentNode->next = addString(line);
        currentNode = currentNode->next;
    }

    for (i = head->next; i != NULL; i = i->next) {
        puts(i->value);
    }
    i = head->next;

    struct node *j = NULL;
    while (i != NULL) {
        j = i->next;
        freeNode(i);
        i = NULL;
        i = j;
    }
    free(head);
    return 0;
}
