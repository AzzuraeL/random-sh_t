#include <stdio.h>
#include <stdlib.h>

typedef struct TNode {
    int data;
    struct TNode *next;
} TNode;

TNode *head;

void init() {
    head = NULL;
}

int isEmpty() {
    if (head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void insertDepan(int databaru) {
    TNode *baru;
    baru = (TNode*)malloc(sizeof(TNode));
    baru->data =databaru ;
    baru->next = NULL;
    if (isEmpty()) {
        head = baru;
        head->next = NULL;
    } else {
        baru->next = head;
        head = baru;
    }
    printf("Data baru telah dimasukkan\n");
}

void insertBelakang(int databaru) {
    TNode *baru, *bantu;
    baru = (TNode*)malloc(sizeof(TNode));
    baru->data = databaru;
    baru->next = NULL;
    if (isEmpty()) {
        head = baru;
        head->next = NULL;
    } else {
        bantu = head;
        while (bantu->next != NULL) {
            bantu = bantu->next;
        }
        bantu->next = baru;
    }
    printf("Data baru telah dimasukkan\n");
}

void tampil() {
    TNode *bantu = head;
    if (!isEmpty(head)) {
        while (bantu != NULL) {
            printf("%d ", bantu->data);
            bantu = bantu->next;
        }
        printf("\n");
    } else {
        printf("List kosong\n");
    }
}

int main() {
    
    init(head);
    insertDepan(10);
    insertDepan(20);
    insertBelakang(30);
    insertBelakang(40);
    printf("Isi linked list:\n");
    tampil(head);

    while (head != NULL) {
        TNode *temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
