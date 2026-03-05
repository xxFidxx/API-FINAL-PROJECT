#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define HASHDIM 10
#define HASHDIMMAG 100

// Funzione di allocazione sicura con inizializzazione a zero
void* malloc_check(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Errore: impossibile allocare memoria di %zu byte\n", size);
        exit(EXIT_FAILURE);
    }
    memset(ptr, 0, size); // Inizializza la memoria a zero
    return ptr;
}

typedef struct corriere {
    int periodo;
    int capienza;
} corriere;

//  RICETTE
typedef struct node_type_ricette {
    char *ingredient;
    int quantity;
    struct node_type_ricette *next;
} node_type_ricette;

typedef struct list_type_ricette {
    node_type_ricette *testa;
    node_type_ricette *coda;
} list_type_ricette;

typedef struct bucket_type_ricette {
    char *name;
    int weight;
    struct bucket_type_ricette *next;
    list_type_ricette *lista_ingredienti;
} bucket_type_ricette;

typedef struct ht_type_ricette {
    bucket_type_ricette **buckets;
} ht_type_ricette;

int hash_ricette(char *name) {
    int value = 5381;
    int c;

    while ((c = *name++)) {
        value = ((value << 5) + value) + c;
    }

    if (value < 0) {
        value = -value;
    }

    return value % HASHDIM;
}

list_type_ricette *crea_lista_ricette() {
    list_type_ricette *list = malloc_check(sizeof(list_type_ricette));
    list->testa = NULL;
    list->coda = NULL;
    return list;
}

ht_type_ricette *crea_hash_ricette() {
    ht_type_ricette *hashtable = malloc_check(sizeof(ht_type_ricette));
    hashtable->buckets = malloc_check(sizeof(bucket_type_ricette*) * HASHDIM);
    for (int i = 0; i < HASHDIM; i++) {
        hashtable->buckets[i] = NULL;
    }
    return hashtable;
}

bucket_type_ricette* inserisci_ricette(ht_type_ricette *hashtable, char *name) {
    int flag, slot;
    bucket_type_ricette *ricetta_pointer = NULL;
    bucket_type_ricette *bucket;

    slot = hash_ricette(name);
    bucket = hashtable->buckets[slot];
    for (flag = 0; bucket != NULL && flag == 0; bucket = bucket->next) {
        if (strcmp(bucket->name, name) == 0) {
            flag = -1;
        }
    }
    if (flag != 0) {
        ricetta_pointer = NULL;
    } else {
        bucket = hashtable->buckets[slot];
        bucket_type_ricette *new_bucket = malloc_check(sizeof(bucket_type_ricette));
        new_bucket->name = strdup(name);
        new_bucket->lista_ingredienti = crea_lista_ricette();
        new_bucket->weight = 0;
        new_bucket->next = bucket;
        hashtable->buckets[slot] = new_bucket;
        ricetta_pointer = new_bucket;
    }

    return ricetta_pointer;
}

bucket_type_ricette* trova_ricetta(ht_type_ricette *hashtable, char *name) {
    int slot = hash_ricette(name);
    bucket_type_ricette *bucket = hashtable->buckets[slot];

    while (bucket != NULL) {
        if (strcmp(bucket->name, name) == 0) {
            return bucket;
        }
        bucket = bucket->next;
    }

    return NULL;
}

void stampa_hash_ricette(ht_type_ricette *hashtable) {
    for (int i = 0; i < HASHDIM; i++) {
        printf("buckets[%d]: ", i);
        bucket_type_ricette *bucket = hashtable->buckets[i];
        while (bucket != NULL) {
            printf("%s -> ", bucket->name);
            bucket = bucket->next;
        }
        printf("NULL\n");
    }
}

void dealloca_bucket_ricette(bucket_type_ricette *bucket) {
    if (bucket == NULL) {
        return;
    }
    free(bucket->name);
    free(bucket);
}

bool delete_ricette(ht_type_ricette *hashtable, char *name) {
    int slot = hash_ricette(name);
    bucket_type_ricette *bucket = hashtable->buckets[slot];
    bucket_type_ricette *prev = NULL;
    int presente=0;

    while (bucket != NULL && !presente) {
        if (strcmp(bucket->name, name) == 0) {
            if (prev == NULL) {
                hashtable->buckets[slot] = bucket->next;
            } else {
                prev->next = bucket->next;
            }
            dealloca_bucket_ricette(bucket);
            presente=1;
        }else{
            prev = bucket;
            bucket = bucket->next;
        }
    }

    if(!presente)
        return false;
    else
        return true;
}

list_type_ricette *aggiungi_coda_lista_ricette(list_type_ricette *list, char* name, int quant) {
    node_type_ricette *new_node = malloc_check(sizeof(node_type_ricette));
    new_node->ingredient = strdup(name);
    new_node->quantity = quant;
    new_node->next = NULL;

    if (list->testa != NULL) {
        list->coda->next = new_node;
        list->coda = new_node;
    } else {
        list->testa = new_node;
        list->coda = new_node;
    }
    return list;
}

void stampa_lista_ricette(list_type_ricette *lista, char* ricetta) {
    printf("\n ricetta:%s ingerdienti:", ricetta);
    node_type_ricette *tmp = lista->testa;
    while (tmp != NULL) {
        printf("%s quantita':%d -> ", tmp->ingredient, tmp->quantity);
        tmp = tmp->next;
    }
}

void stampa_hash_ricette_2(ht_type_ricette *hashtable) {
    for (int i = 0; i < HASHDIM; i++) {
        bucket_type_ricette *bucket = hashtable->buckets[i];
        while (bucket != NULL) {
            stampa_lista_ricette(bucket->lista_ingredienti, bucket->name);
            bucket = bucket->next;
        }
    }
}

// MAGAZZINO

int hash_mag(char *name) {
    int value = 5381;
    int c;

    while ((c = *name++)) {
        value = ((value << 5) + value) + c;
    }

    if (value < 0) {
        value = -value;
    }

    return value % HASHDIMMAG;
}

typedef struct node_type_mag {
    int quantity;
    int scadenza;
    struct node_type_mag *next;
} node_type_mag;

typedef struct list_type_mag {
    node_type_mag *testa;
    node_type_mag *coda;
} list_type_mag;

typedef struct bucket_type_mag {
    char *name;
    int qt_tot;
    struct bucket_type_mag *next;
    list_type_mag *lista_qt;
} bucket_type_mag;

typedef struct ht_type_mag {
    bucket_type_mag **buckets;
} ht_type_mag;

list_type_mag *crea_lista_mag() {
    list_type_mag *list = malloc_check(sizeof(list_type_mag));
    list->testa = NULL;
    list->coda = NULL;
    return list;
}

ht_type_mag *crea_hash() {
    ht_type_mag *hashtable = malloc_check(sizeof(ht_type_mag));
    hashtable->buckets = malloc_check(sizeof(bucket_type_mag*) * HASHDIMMAG);
    for (int i = 0; i < HASHDIMMAG; i++) {
        hashtable->buckets[i] = NULL;
    }
    return hashtable;
}

bucket_type_mag* trova_mag(ht_type_mag *hashtable, char *name) {
    int slot = hash_mag(name);
    bucket_type_mag *bucket = hashtable->buckets[slot];

    while (bucket != NULL) {
        if (strcmp(bucket->name, name) == 0) {
            return bucket;
        }
        bucket = bucket->next;
    }

    return NULL;
}

bucket_type_mag* inserisci_mag(ht_type_mag *hashtable, char *name) {
    int slot = hash_mag(name);
    bucket_type_mag *bucket = hashtable->buckets[slot];
    bucket_type_mag *ingrediente = NULL;

    while (bucket != NULL) {
        if (strcmp(bucket->name, name) == 0) {
            ingrediente = bucket;
            break;
        }
        bucket = bucket->next;
    }

    if (ingrediente == NULL) {
        bucket_type_mag *new_bucket = malloc_check(sizeof(bucket_type_mag));
        new_bucket->name = strdup(name);
        new_bucket->qt_tot = 0;
        new_bucket->lista_qt = crea_lista_mag();
        new_bucket->next = hashtable->buckets[slot];
        hashtable->buckets[slot] = new_bucket;
        ingrediente = new_bucket;
    }

    return ingrediente;
}

list_type_mag *aggiungi_lista_mag(list_type_mag *list, int quant, int scad) {
    node_type_mag *new_node = malloc_check(sizeof(node_type_mag));
    node_type_mag *prev = NULL;
    node_type_mag *current = NULL;
    new_node->quantity = quant;
    new_node->scadenza = scad;
    new_node->next = NULL;

    if (list->testa == NULL) {
        list->testa = new_node;
        list->coda = new_node;
    } else {
        current = list->testa;
        prev = NULL;

        while (current != NULL && current->scadenza < scad) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            new_node->next = list->testa;
            list->testa = new_node;
        } else if (current == NULL) {
            prev->next = new_node;
            list->coda = new_node;
        } else {
            prev->next = new_node;
            new_node->next = current;
        }
    }
    return list;
}

void stampa_lista_mag(list_type_mag *lista) {
    node_type_mag *tmp = lista->testa;
    while (tmp != NULL) {
        printf("qt: %d scad: %d -> ", tmp->quantity, tmp->scadenza);
        tmp = tmp->next;
    }
    printf("NULL\n");
}

void dealloca_lista_mag(list_type_mag *lista) {
    node_type_mag *current = lista->testa;
    while (current != NULL) {
        node_type_mag *next = current->next;
        free(current);
        current = next;
    }
    free(lista);
}

void dealloca_bucket_mag(bucket_type_mag *bucket) {
    if (bucket == NULL) {
        return;
    }
    dealloca_lista_mag(bucket->lista_qt);
    free(bucket->name);
    free(bucket);
}

void delete_mag(ht_type_mag *hashtable, char *name) {
    int slot = hash_mag(name);
    bucket_type_mag *bucket = hashtable->buckets[slot];
    bucket_type_mag *prev = NULL;

    while (bucket != NULL) {
        if (strcmp(bucket->name, name) == 0) {
            if (prev == NULL) {
                hashtable->buckets[slot] = bucket->next;
            } else {
                prev->next = bucket->next;
            }
            dealloca_bucket_mag(bucket);
            return;
        }
        prev = bucket;
        bucket = bucket->next;
    }
}

void stampa_hash_magazzino(ht_type_mag *hashtable) {
    for (int i = 0; i < HASHDIMMAG; i++) {
        printf("buckets[%d]: ", i);
        bucket_type_mag *bucket = hashtable->buckets[i];
        while (bucket != NULL) {
            printf("%s qt %d -> ", bucket->name, bucket->qt_tot);
            bucket = bucket->next;
        }
        printf("NULL\n");
    }
}

void stampa_hash_mag_2(ht_type_mag *hashtable) {
    for (int i = 0; i < HASHDIMMAG; i++) {
        bucket_type_mag *bucket = hashtable->buckets[i];
        while (bucket != NULL) {
            printf("\nprodotto: %s quantita' tot: %d ", bucket->name, bucket->qt_tot);
            stampa_lista_mag(bucket->lista_qt);
            bucket = bucket->next;
        }
    }
}

void stampa_lista_mag2(list_type_mag *list) {
    node_type_mag *curr = list->testa;
    while (curr != NULL) {
        printf("scadenza:%d, quantità: %d ->", curr->scadenza, curr->quantity);
        curr = curr->next;
    }
}

bucket_type_mag *controlla_scadenze(bucket_type_mag *bucket, int curr_time) {
    // Check for NULL pointers
    if (bucket == NULL) {

        fprintf(stderr, "Error: Bucket is NULL.\n");
        return NULL; // Or return bucket if you prefer to maintain the function's return type
    }

    if (bucket->lista_qt == NULL) {

        fprintf(stderr, "Error: List of quantities is NULL in bucket.\n");
        return bucket;
    }
    int esci=0;
    list_type_mag *list = bucket->lista_qt;
    node_type_mag *curr = list->testa;
    node_type_mag *tmp = NULL;  // Temporary node for deletion


    while (curr != NULL && !esci) {
        if (curr->scadenza < curr_time) {

            // Update bucket's total quantity
            bucket->qt_tot -= curr->quantity;

            // Set temporary node for deletion
            tmp = curr;

            // Move to the next node before deletion
            curr=curr->next;
            list->testa=curr;

            free(tmp);
        }else{
            esci=1;
        }
    }

    return bucket;
}

//SEZIONE ORDINI//

/*
 t *ypedef struct nodo_ingredienti_ordine {
 bucket_type_mag* ingrediente;
 struct lista_ingredienti_ordine *next;
 } nodo_ingredienti_ordine;

 typedef struct lista_ingredienti_ordine {
 nodo_ingredienti_ordine* ingrediente;
 struct lista_ingredienti_ordine *next;
 } lista_ingredienti_ordine;
 */

typedef struct node_type_ordini_preparare {
    bucket_type_ricette* ricetta;
    int qt_ordine;
    int time;
    struct node_type_ordini_preparare *next;
} node_type_ordini_preparare;


typedef struct list_type_ordini_preparare {
    node_type_ordini_preparare *testa;
    node_type_ordini_preparare *coda;
} list_type_ordini_preparare;

typedef struct node_type_ordini_pronti {
    char* ordine_name;
    int qt_ordine;
    int ordine_weight;
    int ordine_time;
    struct node_type_ordini_pronti *next;
} node_type_ordini_pronti;

typedef struct list_type_ordini_pronti {
    node_type_ordini_pronti *testa;
    node_type_ordini_pronti *coda;
} list_type_ordini_pronti;


list_type_ordini_preparare * crea_lista_ordini_preparare(){
    list_type_ordini_preparare *list;
    list = malloc_check(sizeof(list_type_ordini_preparare));
    list->testa=NULL;
    list->coda=NULL;

    return list;
}

list_type_ordini_pronti * crea_lista_ordini_pronti(){
    list_type_ordini_pronti *list;
    list = malloc_check(sizeof(list_type_ordini_pronti));
    list->testa=NULL;
    list->coda=NULL;
    return list;

}

bool is_preparabile(ht_type_mag *hashtable, char* nome_ingrediente, int qt_ingrediente, int qt_ordine, int curr_time) {
    bucket_type_mag* ingrediente = trova_mag(hashtable, nome_ingrediente);
    if (ingrediente != NULL) {
        ingrediente = controlla_scadenze(ingrediente, curr_time);
        if (ingrediente != NULL && ingrediente->qt_tot >= (qt_ingrediente * qt_ordine)) {
            return true;
        }
    }
    return false;
}



void prepara_ordine(ht_type_mag *hashtable, char* nome_ingrediente, int qt_ingrediente, int qt_ordine){
    bucket_type_mag* ingrediente = trova_mag(hashtable, nome_ingrediente);
    node_type_mag* tmp=NULL;
    node_type_mag* lotto=ingrediente->lista_qt->testa;
    int left_qt = qt_ingrediente * qt_ordine;
    if (ingrediente == NULL) {
        printf("ingrediente = NULL in prepara ordine\n");
        exit(EXIT_FAILURE);
    }else{
        // non ho bisogno di altre condizioni perché prima ho calcolato già che la qt_tot è sufficiente per l'ordine
        while(left_qt>0){
            if(lotto==NULL){
                printf("ingrediente = lotto = NULL in prepara ordine, non dovrebbe succedere visto che avevi calcolato prima che qt_tot è sufficiente quindi non dovresti arrivare alla fine dei lotti\n");
                exit(EXIT_FAILURE);
            }
            lotto=ingrediente->lista_qt->testa;
            left_qt-= lotto->quantity;
            ingrediente->qt_tot-= lotto->quantity;
            // se vado in negativo vuol dire che il lotto eccede di quantità quindi gliela restituisco
            if(left_qt <0){
                lotto->quantity=(-left_qt);
                ingrediente->qt_tot+= lotto->quantity;
            }else{
                tmp=lotto;
                lotto=lotto->next;
                free(tmp);
                ingrediente->lista_qt->testa=lotto;

            }
        }
    }
    return;
}

list_type_ordini_preparare *aggiungi_coda_lista_ordini_preparare(list_type_ordini_preparare *list, bucket_type_ricette* ricetta, int quant,int time) {
    node_type_ordini_preparare *new_node = malloc_check(sizeof(node_type_ordini_preparare));
    new_node->ricetta = ricetta;
    new_node->qt_ordine = quant;
    new_node->time = time;
    new_node->next = NULL;

    if (list->testa != NULL) {
        list->coda->next = new_node;
        list->coda = new_node;
    } else {
        list->testa = new_node;
        list->coda = new_node;
    }
    return list;
}

list_type_ordini_pronti *aggiungi_coda_lista_ordini_pronti(list_type_ordini_pronti *list, bucket_type_ricette* curr_ricetta, int weight_ricetta, int qt_ordin, int time) {
    node_type_ordini_pronti *new_node = malloc_check(sizeof(node_type_ordini_pronti));
    node_type_ordini_pronti *current = NULL;
    node_type_ordini_pronti *prec = NULL;
    new_node->ordine_name = curr_ricetta->name;
    new_node->qt_ordine = qt_ordin;
    new_node->ordine_weight = qt_ordin * weight_ricetta;
    new_node->ordine_time = time;
    new_node->next=NULL;
    int inserito=1;

    if (list->testa == NULL) {
        list->testa = new_node;
        list->coda = new_node;
    } else {
        current = list->testa;
        prec = NULL;
        inserito = 0;

        while (current != NULL && !inserito) {
            if (new_node->ordine_weight > current->ordine_weight) {
                new_node->next = current;
                if (prec == NULL) {
                    list->testa = new_node;
                } else {
                    prec->next = new_node;
                }
                inserito = 1;
            } else if (new_node->ordine_weight == current->ordine_weight) {
                while (current != NULL && new_node->ordine_weight == current->ordine_weight && new_node->ordine_time > current->ordine_time) {
                    prec = current;
                    current = current->next;
                }
                new_node->next = current;
                if (prec == NULL) {
                    list->testa = new_node;
                } else {
                    prec->next = new_node;
                }
                inserito = 1;
            } else {
                prec = current;
                current = current->next;
            }
        }

        if (!inserito) {
            prec->next = new_node;
            list->coda = new_node;
        }
    }
    return list;
}

list_type_ordini_preparare* cancella_coda_lista_ordini_preparare(list_type_ordini_preparare* list,node_type_ordini_preparare *nodo, node_type_ordini_preparare* prec){
    node_type_ordini_preparare *tmp=nodo;
    // siamo in testa alla lista
    if(prec==NULL){
        list->testa = nodo->next;
        if(list->testa==NULL)
            list->coda=NULL;
        // siamo in coda
    }else if(nodo->next==NULL){
        prec->next = NULL;
        list->coda = prec;
        // siamo in mezzo
    }else{
        prec->next = nodo->next;
    }
    free(tmp);

    return list;
}

bool check_lista_ordini_da_preparare(list_type_ordini_preparare* list, char* ricetta_name){
    node_type_ordini_preparare *curr = list->testa;
    int presente=0;

    while(curr!=NULL && !presente){
        if(strcmp(ricetta_name, curr->ricetta->name) == 0)
            presente=1;
        else
            curr=curr->next;
    }

    if(!presente)
        return false;
    else
        return true;

}

list_type_ordini_pronti* cancella_coda_lista_ordini_pronti(list_type_ordini_pronti* list,node_type_ordini_pronti *nodo, node_type_ordini_pronti* prec){
    node_type_ordini_pronti *tmp = nodo;

    // Siamo in testa alla lista
    if (prec == NULL) {
        list->testa = nodo->next;
        if (list->testa == NULL) {
            list->coda = NULL;
        }
        // Siamo in coda
    } else if (nodo->next == NULL) {
        prec->next = NULL;
        list->coda = prec;
        // Siamo in mezzo
    } else {
        prec->next = nodo->next;
    }
    free(tmp);

    return list;
}

bool check_lista_ordini_pronti(list_type_ordini_pronti* list, char* ricetta_name){
    node_type_ordini_pronti *curr = list->testa;
    int presente=0;

    while(curr!=NULL && !presente){
        if(strcmp(ricetta_name, curr->ordine_name) == 0)
            presente=1;
        else
            curr=curr->next;
    }

    if(!presente)
        return false;
    else
        return true;

}

// GESTIONE CORRIERE

list_type_ordini_pronti* gestione_corriere(list_type_ordini_pronti* list, int capienza){
    node_type_ordini_pronti *curr = list->testa;
    node_type_ordini_pronti *prec = NULL;
    node_type_ordini_pronti *to_delete = NULL;
    int curr_capienza = capienza;
    int vuoto = 1;

    while (curr != NULL && curr_capienza > 0) {
        if ((curr_capienza - curr->ordine_weight) >= 0) {
            curr_capienza -= curr->ordine_weight;
            to_delete = curr;
            printf("%d %s %d\n", curr->ordine_time, curr->ordine_name, curr->qt_ordine);
            curr = curr->next;
            list = cancella_coda_lista_ordini_pronti(list, to_delete, prec);
            vuoto = 0;
        } else {
            prec = curr;
            curr = curr->next;
        }
    }

    if (vuoto) {
        printf("camioncino vuoto\n");
    }

    return list;

}


int main() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();


    ht_type_ricette* hashtable_ricette = crea_hash_ricette();
    ht_type_mag* hashtable_mag = crea_hash();
    // crea_lista_ordini();

    bool end_of_file = false, preparabile;
    int quantity, scadenza, qt_ordine;
    char* buffer = NULL;
    size_t buffer_size = 0;
    char *buffer1, *buffer2, *buffer3, *command, *nome_ordine;
    corriere corr;
    bucket_type_ricette* ricetta_pointer;
    bucket_type_mag* ingrediente_pointer;
    node_type_ricette* node_ingrediente_pointer;
    list_type_ordini_preparare * lista_ordini_preparare = crea_lista_ordini_preparare();
    node_type_ordini_preparare *ordine_da_preparare = NULL;
    node_type_ordini_preparare *prec = NULL;
    node_type_ordini_preparare *ordine_da_eliminare =NULL;
    list_type_ordini_pronti * lista_ordini_pronti = crea_lista_ordini_pronti();
    int tim = 0;

    // inizializzo corriere
    if (getline(&buffer, &buffer_size, stdin) == -1) {
        end_of_file = true;
    } else {
        // rimuovo \n dalla fine del buffer
        buffer[strcspn(buffer, "\n")] = '\0';

        buffer1 = strtok(buffer, " ");
        if (buffer1 == NULL) {
            fprintf(stderr, "Error: Invalid input format.\n");
            free(buffer);
            return 1;
        }
        buffer2 = strtok(NULL, " ");
        if (buffer2 == NULL) {
            fprintf(stderr, "Error: Invalid input format.\n");
            free(buffer);
            return 1;
        }

        corr.periodo = atoi(buffer1);
        corr.capienza = atoi(buffer2);
    }

    // leggo la prima stringa
    if (getline(&buffer, &buffer_size, stdin) == -1) {
        end_of_file = true;
    } else {
        // rimuovo \n dalla fine del buffer
        buffer[strcspn(buffer, "\n")] = '\0';
        command = strtok(buffer, " ");
    }

    // ciclo per la lettura di tutto il file
    while (!end_of_file) {

        //gestione corriere
        if(tim!= 0 && tim%corr.periodo == 0){
            gestione_corriere(lista_ordini_pronti, corr.capienza);
        }

        if (strcmp(command, "ordine") == 0) {
            buffer1 = strtok(NULL, " ");
            buffer2 = strtok(NULL, " ");
            if (buffer1 == NULL || buffer2 == NULL) {
                fprintf(stderr, "Error: Invalid input format for ordine.\n");
                free(buffer);
                return 1;
            }

            nome_ordine = buffer1;
            qt_ordine = atoi(buffer2);
            ricetta_pointer = trova_ricetta(hashtable_ricette, nome_ordine);
            if (ricetta_pointer != NULL) {
                preparabile = true;
                if (ricetta_pointer->lista_ingredienti != NULL) {
                    node_ingrediente_pointer = ricetta_pointer->lista_ingredienti->testa;
                    while (node_ingrediente_pointer != NULL && preparabile) {
                        preparabile = is_preparabile(hashtable_mag, node_ingrediente_pointer->ingredient, node_ingrediente_pointer->quantity, qt_ordine, tim);
                        node_ingrediente_pointer = node_ingrediente_pointer->next;
                        // potresti fare una struttura che contiene tutti i puntatori

                    }
                    if(preparabile){
                        node_ingrediente_pointer = ricetta_pointer->lista_ingredienti->testa;
                        while (node_ingrediente_pointer != NULL) {
                            prepara_ordine(hashtable_mag, node_ingrediente_pointer->ingredient, node_ingrediente_pointer->quantity, qt_ordine);
                            node_ingrediente_pointer = node_ingrediente_pointer->next;
                            // potresti fare una struttura che contiene tutti i puntatori
                        }
                        aggiungi_coda_lista_ordini_pronti(lista_ordini_pronti,ricetta_pointer,ricetta_pointer->weight,qt_ordine,tim);
                    }
                }else{
                    aggiungi_coda_lista_ordini_preparare(lista_ordini_preparare, ricetta_pointer,qt_ordine,tim);
                }
                printf("accettato\n");
            }else {
                printf("rifiutato\n");
            }

            if (getline(&buffer, &buffer_size, stdin) == -1) {
                end_of_file = true;
            } else {
                buffer[strcspn(buffer, "\n")] = '\0';
                command = strtok(buffer, " ");
                tim++;
            }

        } else if (strcmp(command, "rimuovi_ricetta") == 0) {
            buffer1 = strtok(NULL, " ");
            if (buffer1 == NULL) {
                fprintf(stderr, "Error: Invalid input format for rimuovi_ricetta.\n");
                free(buffer);
                return 1;
            }
            if(check_lista_ordini_da_preparare(lista_ordini_preparare, buffer1) == false && check_lista_ordini_pronti(lista_ordini_pronti, buffer1) == false ){
                if(delete_ricette(hashtable_ricette, buffer1))
                    printf("rimossa\n");
                else printf("non presente\n");
            }else{
                printf("ordini in sospeso\n");
            }

            if (getline(&buffer, &buffer_size, stdin) == -1) {
                end_of_file = true;
            } else {
                buffer[strcspn(buffer, "\n")] = '\0';
                command = strtok(buffer, " ");
                tim++;
            }

        } else if (strcmp(command, "aggiungi_ricetta") == 0) {
            buffer1 = strtok(NULL, " ");
            if (buffer1 == NULL) {
                fprintf(stderr, "Error: Invalid input format for aggiungi_ricetta.\n");
                free(buffer);
                return 1;
            }
            ricetta_pointer = inserisci_ricette(hashtable_ricette, buffer1);
            if (ricetta_pointer == NULL) {

                while (buffer1 != NULL) {
                    buffer1 = strtok(NULL, " ");
                }
                if (getline(&buffer, &buffer_size, stdin) == -1) {
                    end_of_file = true;
                } else {
                    buffer[strcspn(buffer, "\n")] = '\0';
                    command = strtok(buffer, " ");
                    tim++;
                }
                printf("ignorato\n");
            } else {
                buffer1 = strtok(NULL, " ");
                while (buffer1 != NULL) {
                    buffer2 = strtok(NULL, " ");
                    if (buffer2 == NULL) {
                        fprintf(stderr, "Error: Invalid input format for ingredient quantity.\n");
                        free(buffer);
                        return 1;
                    }
                    quantity = atoi(buffer2);
                    ricetta_pointer->lista_ingredienti = aggiungi_coda_lista_ricette(ricetta_pointer->lista_ingredienti, buffer1, quantity);
                    ricetta_pointer->weight += quantity;
                    buffer1 = strtok(NULL, " ");
                }
                if (getline(&buffer, &buffer_size, stdin) == -1) {
                    end_of_file = true;
                } else {
                    buffer[strcspn(buffer, "\n")] = '\0';
                    command = strtok(buffer, " ");
                    tim++;
                }
                printf("aggiunta\n");
            }

        } else if (strcmp(command, "rifornimento") == 0) {
            buffer1 = strtok(NULL, " ");
            while (buffer1 != NULL) {
                buffer2 = strtok(NULL, " ");
                if (buffer2 == NULL) {
                    fprintf(stderr, "Error: Invalid input format for rifornimento.\n");
                    free(buffer);
                    return 1;
                }
                quantity = atoi(buffer2);
                buffer3 = strtok(NULL, " ");
                if (buffer3 == NULL) {
                    fprintf(stderr, "Error: Invalid input format for ingredient expiration date.\n");
                    free(buffer);
                    return 1;
                }
                scadenza = atoi(buffer3);
                ingrediente_pointer = inserisci_mag(hashtable_mag, buffer1);
                ingrediente_pointer->lista_qt = aggiungi_lista_mag(ingrediente_pointer->lista_qt, quantity, scadenza);
                ingrediente_pointer->qt_tot += quantity;
                buffer1 = strtok(NULL, " ");
            }
            printf("rifornito\n");
            // se a fine rifornimento sono preparabili altre ricette
            ordine_da_preparare = lista_ordini_preparare->testa;
            prec=NULL;

            while(ordine_da_preparare != NULL){
                ricetta_pointer = ordine_da_preparare->ricetta;
                if (ricetta_pointer != NULL) {
                    preparabile = true;
                    if (ricetta_pointer->lista_ingredienti != NULL) {
                        node_ingrediente_pointer = ricetta_pointer->lista_ingredienti->testa;
                        while (node_ingrediente_pointer != NULL && preparabile) {
                            preparabile = is_preparabile(hashtable_mag, node_ingrediente_pointer->ingredient, node_ingrediente_pointer->quantity, ordine_da_preparare->qt_ordine, ordine_da_preparare->time);
                            node_ingrediente_pointer = node_ingrediente_pointer->next;


                        }
                        if(preparabile){
                            node_ingrediente_pointer = ricetta_pointer->lista_ingredienti->testa;
                            while (node_ingrediente_pointer != NULL) {
                                prepara_ordine(hashtable_mag, node_ingrediente_pointer->ingredient, node_ingrediente_pointer->quantity, ordine_da_preparare->qt_ordine);
                                node_ingrediente_pointer = node_ingrediente_pointer->next;

                            }
                            aggiungi_coda_lista_ordini_pronti(lista_ordini_pronti,ricetta_pointer,ricetta_pointer->weight,ordine_da_preparare->qt_ordine,ordine_da_preparare->time);
                            ordine_da_eliminare =ordine_da_preparare;
                            ordine_da_preparare = ordine_da_preparare->next;
                            cancella_coda_lista_ordini_preparare(lista_ordini_preparare,ordine_da_eliminare,prec);
                        }else{
                            prec=ordine_da_preparare;
                            ordine_da_preparare = ordine_da_preparare->next;
                        }
                    } else{
                        printf("ordine_da_preparare->ricetta==NULL");
                        exit(EXIT_FAILURE);
                    }
                }else{
                    printf("ricetta_pointer==NULL");
                    exit(EXIT_FAILURE);
                }
            }
            if (getline(&buffer, &buffer_size, stdin) == -1) {
                end_of_file = true;
            } else {
                buffer[strcspn(buffer, "\n")] = '\0';
                command = strtok(buffer, " ");
                tim++;
            }
        }




    }
    tim++;
    if(tim%corr.periodo == 0){
        gestione_corriere(lista_ordini_pronti, corr.capienza);
    }

    //printf("periodo: %d, capienza: %d\n", corr.periodo, corr.capienza);
    printf("%d\n", tim);
    //stampa_hash_ricette_2(hashtable_ricette);
    //stampa_hash_mag_2(hashtable_mag);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    //printf("Tempo totale di esecuzione: %f secondi\n", cpu_time_used);
    free(buffer);
    return 0;
}
