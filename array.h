/**
 *  @file   array.h
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
 */

#ifndef moustached_array_h
#define moustached_array_h

#if !defined(MOUSTASHED_ITERATOR)
#define MOUSTASHED_ITERATOR
struct _Iterator {
    int handler;
    int carriage;
    int total_elems;
    
    char hasnext;
    char hasprev;
    
    void (*update)(struct _Iterator*);
    void (*reset)(struct _Iterator*);
    void *(*next)(struct _Iterator*);
    void *(*prev)(struct _Iterator*);
};
typedef struct _Iterator iterator;
#endif
typedef int Array;


Array Anew(int);
void Apurge(Array);
void Adispose(Array);

void *Aadd(Array, void*);
void *Aget(Array, int);
void *Aset(Array, int, void*);
void *Aremove(Array, int);

int Asize(Array);
void** Atoarray(Array);
iterator Aiterator(Array);


#endif
