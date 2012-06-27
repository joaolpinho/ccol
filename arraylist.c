/**
 *  @file   arraylist.c
 *  @link   https://github.com/joaolpinho
 *
 *  @brief  Resizable & multi type support array
 *
 *  @author João Pinho
 *  @link   https://github.com/joaolpinho
 *
 *  @date   06/06/2012
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
 *  @AUTHOR joaolpinho
 *  @URL    https://github.com/joaolpinho
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "arraylist.h"

#ifndef MOUSTASHED_ERROR_STRINGS
    #define MOUSTASHED_ERROR_STRINGS
    #define S_NOMEM "Allocating memory"
    #define S_EFAULT "Invalid handler"
#endif

#define A_INITCAPACITY 30

/**
 * Struct and Type definitions
 *
 */
struct _Controller {
    int total_buckets;
    int used_buckets;
    struct _Array *buckets;
};

struct _Array {
    int total_buckets;
    int used_buckets;
    void **buckets;
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
static void check(struct _Array *);
static int getfreearray();
static void *ALitnext(iterator *);
static void *ALitprev(iterator *);
static void ALupdateit(iterator *);
static void ALresetit(iterator *);

/**
 * Functions definition
 *
 */
int ALnew(int init_size) {
    int handler = -1;
    struct _Array *tmp = NULL;
    struct _Array *array = NULL;
    
    if (controller.buckets == NULL) {
        controller.buckets = malloc((A_INITCAPACITY)*(sizeof(void **)));
        if (controller.buckets == NULL) {
            perror(S_NOMEM);
            exit(EXIT_FAILURE);
        }
        controller.total_buckets = A_INITCAPACITY;
    }
    if (controller.used_buckets > controller.total_buckets*A_LOADFACT) {
        tmp = realloc(controller.buckets, sizeof(struct _Array) * controller.used_buckets*A_EXPRATE);
        if (tmp == NULL) {
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        controller.buckets = tmp;
        controller.total_buckets *= A_EXPRATE;
    }
    handler = getfreearray();
    array = &controller.buckets[handler];
    array->buckets = malloc(sizeof(void **)*init_size);
    if (array->buckets == NULL) {
        perror(S_NOMEM);
        exit(EXIT_FAILURE);
    }
    controller.used_buckets++;
    array->total_buckets = init_size;
    array->used_buckets = 0;
    return handler;
}

void ALpurge(int handler) {
    if (handler <= controller.used_buckets) {
        free(controller.buckets[handler].buckets);
        controller.buckets[handler].buckets = malloc(sizeof(void **)*A_INITCAPACITY);
        if (controller.buckets[handler].buckets == NULL) {
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        controller.buckets[handler].total_buckets = A_INITCAPACITY;
        controller.buckets[handler].used_buckets = 0;
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
}

void ALdispose(int handler) {
    if (handler <= controller.used_buckets) {
        controller.buckets[handler].total_buckets = 0;
        controller.buckets[handler].used_buckets = 0;
        if (controller.buckets[handler].buckets != NULL) {
            free(controller.buckets[handler].buckets);
        }
        controller.used_buckets--;
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return;
}

void *ALadd(int handler, void *elem) {
    struct _Array *a;
    
    if (handler <= controller.used_buckets) {
        a = &controller.buckets[handler];
        a->buckets[a->used_buckets] = elem;
        a->used_buckets++;
        check(a);
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    } 
    return elem;
}

void *ALget(int handler, int i) {
    void *elem = NULL;
    if (handler <= controller.used_buckets) {
        if (controller.buckets[handler].total_buckets >= i)
            elem = controller.buckets[handler].buckets[i];
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return elem;
}

void *ALset(int handler, int i, void *elem) {
    struct _Array *a;
    void *holder = NULL, *next = NULL;
    int j = 0;
    if ((handler <= controller.used_buckets) && (i <= controller.buckets[handler].used_buckets)) {
        a = &controller.buckets[handler];
        next = a->buckets[i];
        a->buckets[i] = elem;
        j = i+1;
        while (j <= a->total_buckets) {
            holder = a->buckets[j];
            a->buckets[j] = next;
            next = holder;
            j++;
        }
        a->used_buckets++;
        check(a);
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return elem;
}

void *ALremove(int handler, int i) {
    struct _Array *a;
    void *prev = NULL, *elem = NULL;
    int j = 0;
    if ((handler <= controller.used_buckets) && (i <= controller.buckets[handler].total_buckets)) {
        a = &controller.buckets[handler];
        elem = prev = a->buckets[i];
        j = i+1;
        while (j <= a->total_buckets) {
            a->buckets[j-1] = a->buckets[j];
            j++;
        }
        a->used_buckets--;
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return elem;
}

int ALsize(int handler) {
    int size = -1;
    if (handler <= controller.used_buckets) {
        size = controller.buckets[handler].used_buckets;
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return size;
}

void** ALtoarray(int handler) {
    void **array = NULL;
    if (handler <= controller.used_buckets) {
        array =  controller.buckets[handler].buckets;
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return array;
}

iterator ALiterator(int handler) {
    iterator it;
    if (handler <= controller.used_buckets) {
        it.handler = handler;
        it.carriage = 0;
        it.next = ALitnext;
        it.prev = ALitprev;
        it.update = ALupdateit;
        it.reset = ALresetit;
        it.update(&it);
    } else {
        errno = EFAULT;
        perror(S_EFAULT);
    }
    return it;
}


/**
 * Static-scope functions definition
 *
 */
static void check(struct _Array *a) {
    void **tmp = NULL;
    
    if (a->used_buckets > a->total_buckets*A_LOADFACT) {
        tmp = realloc(a->buckets, sizeof(void**) * a->used_buckets*A_EXPRATE);
        if (tmp == NULL) {
            errno = ENOMEM;
            perror(S_NOMEM); 
            exit(EXIT_FAILURE);
        }
        a->buckets = tmp;
        a->total_buckets *= A_EXPRATE;
    }
    
}

static int getfreearray() {
    int handler = 0;
    while ((handler < controller.used_buckets) 
           && (controller.buckets[handler].buckets != NULL))
            handler++;
    
    return handler;
}

static void *ALitnext(iterator *it) {
    void *elem = NULL;
    if (it->hasnext) {
        elem = controller.buckets[it->handler].buckets[it->carriage];
        it->carriage++;
        it->update(it);
    }
    return elem;
}

static void *ALitprev(iterator *it) {
    void *elem = NULL;
    if (it->hasprev) {
        it->carriage--;
        elem = controller.buckets[it->handler].buckets[it->carriage];
        it->update(it);
    }
    return elem;
}

static void ALupdateit(iterator *it) {
    it->total_elems = controller.buckets[it->handler].used_buckets;
    it->hasnext = (it->carriage < it->total_elems)?1:0;
    it->hasprev = (it->carriage > 0)?1:0;
    return;
}

static void ALresetit(iterator *it) {
    it->carriage = 0;
    it->update(it);
}