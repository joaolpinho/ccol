/**
 *  @file   linkedlist.c
 *  @link   https://github.com/joaolpinho
 *
 *  @brief  Doubly-LinkedList
 *
 *  @author João Pinho
 *  @link   https://github.com/joaolpinho
 *
 *  @date   07/06/2012
 *
 *  This file is part of moustashed-library.
 *
 *  moustashed-library is a C library of many utils and data structures.
 *  Copyright (C) 2012  João Pinho
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "linkedlist.h"


#define LL_EXPRATE 2
#define LL_LOADFACT 0.75
#define LL_INITCAPACITY 30

#ifndef MOUSTASHED_ERROR_STRINGS
#define MOUSTASHED_ERROR_STRINGS
#define S_NOMEM "Allocating memory"
#define S_EFAULT "Invalid handler"
#endif

/**
 * Struct and Type definitions
 *
 */
struct _Controller {
    int total_buckets;
    int used_buckets;
    struct _LinkedList *buckets;
};

struct _Node {
    char enabled;
    struct _Node *next;
    struct _Node *prev;
    
    void *data;
};

struct _LinkedList {
    int used_buckets;
    struct _Node *head;
    struct _Node *tail;
};


/**
 * Static-scope variables declaration
 *
 */
static struct _Controller controller;

/**
 * Static-scope functions declaration
 *
 */
static int getfreelist();
static char validhandler(int);
static void *LLitnext(iterator *);
static void *LLitprev(iterator *);
static void LLupdateit(iterator *);
static void LLresetit(iterator *);


LinkedList LLnew(void) {
    int handler = -1;
    struct _LinkedList *tmp = NULL;
    
    if (controller.buckets == NULL) {
        controller.buckets = malloc((LL_INITCAPACITY)*(sizeof(void **)));
        if (controller.buckets == NULL) {
            perror(S_NOMEM);
            exit(EXIT_FAILURE);
        }
        controller.total_buckets = LL_INITCAPACITY;
    }
    if (controller.used_buckets > controller.total_buckets*LL_LOADFACT) {
        tmp = realloc(controller.buckets, sizeof(struct _LinkedList) * controller.used_buckets*LL_EXPRATE);
        if (tmp == NULL) {
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        controller.buckets = tmp;
        controller.total_buckets *= LL_EXPRATE;
    }
    handler = getfreelist();
    controller.used_buckets++;
    controller.buckets[handler].used_buckets = 0;
    return handler;
}

void LLpurge(int handler) {
    if (validhandler(handler)) {
        while (controller.buckets[handler].used_buckets > 0) {
            LLremove(handler, 0);
        }
        controller.buckets[handler].used_buckets = 0;
        controller.buckets[handler].head = NULL;
        controller.buckets[handler].tail = NULL;
    }
    return;
}

void LLdispose(int handler) {
    if (validhandler(handler)) {
        LLpurge(handler);
        controller.buckets[handler].head = NULL;
        controller.buckets[handler].tail = NULL;
        controller.buckets[handler].used_buckets = 0;
    }
    return;
}



void *LLadd(int handler, void *elem) {
    struct _Node *n = NULL;
    struct _Node *prev = NULL;
    
    if (validhandler(handler)) {
        n = controller.buckets[handler].head;
        if (n == NULL) {
            n = malloc(sizeof(struct _Node));
            n->prev = NULL;
            n->next = NULL;
            controller.buckets[handler].head = n;
        } else {
            
            while (n != NULL) {
                prev = n;
                n = n->next;
            }
            n = malloc(sizeof(struct _Node));
            if (n == NULL) {
                perror(S_NOMEM);
                exit(EXIT_FAILURE);
            }
            n->prev = prev;
            n->next = NULL;
            prev->next = n;
        }
        n->data = elem;
        controller.buckets[handler].used_buckets++;
    }
    return elem;
}

void *LLget(int handler, int i) {
    void *elem = NULL;
    struct _Node *n = NULL;
    int c = 0;
    if (validhandler(handler)) {
        if (controller.buckets[handler].used_buckets > i) {
            n = controller.buckets[handler].head;
            for (c = 0; c < i; c++) {
                n = n->next;
            }
            elem = n->data;
        }
    }
    return elem;
}

void *LLset(LinkedList handler, int i, void *elem) {
    struct _LinkedList *a = NULL;
    struct _Node *n = NULL;
    struct _Node *newnode = NULL;
    int c = 0;
    if (validhandler(handler) && (i >= 0) && (i < controller.buckets[handler].used_buckets)) {
        a = &controller.buckets[handler];
        n = a->head;
        
        for (c = 0; c<i; c++) {
            n = n->next;
        }
        newnode = malloc(sizeof(struct _Node));
        if (newnode == NULL) {
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        
        if (n->prev != NULL) {
            n->prev->next = newnode;
        }
        newnode->prev = n->prev;
        
        if (n == controller.buckets[handler].head) {
            controller.buckets[handler].head = newnode;
        }
        
        n->prev = newnode;
        newnode->next = n;
        
        newnode->data = elem;
        a->used_buckets++;
    }
    return elem;
}

void *LLremove(int handler, int i) {
    struct _LinkedList *a = NULL;
    struct _Node *n = NULL;
    void *elem = NULL;
    int c = 0;
    if (validhandler(handler)) {
        a = &controller.buckets[handler];
        n = a->head;
        if (n) {
            if (i > 0) {
                while (n && (c < i)) {
                    n = n->next;
                }
                if (n->next) {
                    n->next->prev = n->prev;
                }
                if (n->prev) {
                    n->prev->next = n->next;
                }
                
            } else if (i == 0){
                if (a->head->next != NULL) {
                    a->head->next->prev = NULL;
                }
                a->head = a->head->next;
            }
            elem = n->data;
            free(n);
            n = NULL;
            a->used_buckets--;
        } else {
            errno = EINVAL;
        }
    }
    return elem;
}


int LLsize(int handler) {
    int size = -1;
    if (validhandler(handler)) {
        size = controller.buckets[handler].used_buckets;
    }
    return size;
}

void** LLtoarray(int handler) {
    void **array = NULL;
    if (validhandler(handler)) {
        array = malloc(sizeof(void *)*controller.buckets[handler].used_buckets);
        if (array == NULL) {
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        iterator it = LLiterator(handler);
        while (it.hasnext) {
            array[it.carriage-1] = it.next(&it);
        }
    }
    return array;
}

iterator LLiterator(int handler) {
    iterator it;
    if (validhandler(handler)) {
        it.handler = handler;
        it.carriage = 0;
        it.next = LLitnext;
        it.prev = LLitprev;
        it.update = LLupdateit;
        it.reset = LLresetit;
        it.update(&it);
    }
    return it;
}


/**
 * Static-scope functions definition
 *
 */
static int getfreelist() {
    int handler = 0;
    while ((handler < controller.used_buckets) 
           && (controller.buckets[handler].head != NULL))
        handler++;
    
    return handler;
}

static char validhandler(int handler) {
    if (handler < controller.used_buckets) {
        return 1;
    } else {
        errno = EFAULT;
    }
    return 0;
}

static void *LLitnext(iterator *it) {
    void *elem = NULL;
    struct _Node *n = NULL;
    int i = 0;
    if (it->hasnext) {
        n = controller.buckets[it->handler].head;
        for (i = 0; i < it->carriage; i++) {
            n = n->next;
        }
        elem = n->data;
        it->carriage++;
        it->update(it);
    }
    return elem;
}

static void *LLitprev(iterator *it) {
    void *elem = NULL;
    struct _Node *n = NULL;
    int i = 0;
    if (it->hasprev) {
        it->carriage--;
        n = controller.buckets[it->handler].head;
        for (i = 1; i < it->carriage; i++) {
            n = n->next;
        }
        elem = n->data;
        it->update(it);
    }
    return elem;
}

static void LLupdateit(iterator *it) {
    it->total_elems = controller.buckets[it->handler].used_buckets;
    it->hasnext = (it->carriage < it->total_elems)?1:0;
    it->hasprev = (it->carriage > 0)?1:0;
    return;
}

static void LLresetit(iterator *it) {
    it->carriage = 0;
    it->update(it);
}
