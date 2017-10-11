/* $Id: hash.c,v 1.14 2002/10/24 23:57:21 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "glheader.h"
#include "imports.h"
#include "glthread.h"
#include "hash.h"
#include "context.h"


/**
 * \file hash.c
 * \brief Generic hash table.  Used for display lists and texture objects.
 * The hash functions are thread-safe.
 * \author Brian Paul
 * \note key=0 is illegal
 */


#define TABLE_SIZE 1023  /**< Size of lookup table/array */

/**
 * An entry in the hash table.  This struct is private to this file.
 */
struct HashEntry {
   GLuint Key;             /**< the entry's key */
   void *Data;             /**< the entry's data */
   struct HashEntry *Next; /**< pointer to next entry */
};

/**
 * The hashtable data structure.  This is an opaque types (it's not
 * defined in the .h file).
 */
struct _mesa_HashTable {
   struct HashEntry *Table[TABLE_SIZE];  /**< the lookup table */
   GLuint MaxKey;                        /**< highest key inserted so far */
   _glthread_Mutex Mutex;                /**< mutual exclusion lock */
};



/**
 * Create a new hash table.
 * \return pointer to a new, empty hash table.
 */
struct _mesa_HashTable *_mesa_NewHashTable(void)
{
   struct _mesa_HashTable *table = CALLOC_STRUCT(_mesa_HashTable);
   if (table) {
      _glthread_INIT_MUTEX(table->Mutex);
   }
   return table;
}



/**
 * Delete a hash table.
 * \param table - the hash table to delete
 */
void _mesa_DeleteHashTable(struct _mesa_HashTable *table)
{
   GLuint i;
   assert(table);
   for (i=0;i<TABLE_SIZE;i++) {
      struct HashEntry *entry = table->Table[i];
      while (entry) {
	 struct HashEntry *next = entry->Next;
	 FREE(entry);
	 entry = next;
      }
   }
   FREE(table);
}



/**
 * Lookup an entry in the hash table.
 * \param table - the hash table
 * \param key - the key
 * \return pointer to user's data or NULL if key not in table
 */
void *_mesa_HashLookup(const struct _mesa_HashTable *table, GLuint key)
{
   GLuint pos;
   const struct HashEntry *entry;

   assert(table);
   assert(key);

   pos = key & (TABLE_SIZE-1);
   entry = table->Table[pos];
   while (entry) {
      if (entry->Key == key) {
	 return entry->Data;
      }
      entry = entry->Next;
   }
   return NULL;
}



/**
 * Insert into the hash table.  If an entry with this key already exists
 * we'll replace the existing entry.
 * \param table - the hash table
 * \param key - the key (not zero)
 * \param data - pointer to user data
 */
void _mesa_HashInsert(struct _mesa_HashTable *table, GLuint key, void *data)
{
   /* search for existing entry with this key */
   GLuint pos;
   struct HashEntry *entry;

   assert(table);
   assert(key);

   _glthread_LOCK_MUTEX(table->Mutex);

   if (key > table->MaxKey)
      table->MaxKey = key;

   pos = key & (TABLE_SIZE-1);
   entry = table->Table[pos];
   while (entry) {
      if (entry->Key == key) {
         /* replace entry's data */
	 entry->Data = data;
         _glthread_UNLOCK_MUTEX(table->Mutex);
	 return;
      }
      entry = entry->Next;
   }

   /* alloc and insert new table entry */
   entry = MALLOC_STRUCT(HashEntry);
   entry->Key = key;
   entry->Data = data;
   entry->Next = table->Table[pos];
   table->Table[pos] = entry;

   _glthread_UNLOCK_MUTEX(table->Mutex);
}



/**
 * Remove an entry from the hash table.
 * \param table - the hash table
 * \param key - key of entry to remove
 */
void _mesa_HashRemove(struct _mesa_HashTable *table, GLuint key)
{
   GLuint pos;
   struct HashEntry *entry, *prev;

   assert(table);
   assert(key);

   _glthread_LOCK_MUTEX(table->Mutex);

   pos = key & (TABLE_SIZE-1);
   prev = NULL;
   entry = table->Table[pos];
   while (entry) {
      if (entry->Key == key) {
         /* found it! */
         if (prev) {
            prev->Next = entry->Next;
         }
         else {
            table->Table[pos] = entry->Next;
         }
         FREE(entry);
         _glthread_UNLOCK_MUTEX(table->Mutex);
	 return;
      }
      prev = entry;
      entry = entry->Next;
   }

   _glthread_UNLOCK_MUTEX(table->Mutex);
}



/**
 * Get the key of the "first" entry in the hash table.
 * This is used in the course of deleting all display lists when
 * a context is destroyed.
 * \param table - the hash table
 * \return key for the "first" entry in the hash table.
 */
GLuint _mesa_HashFirstEntry(struct _mesa_HashTable *table)
{
   GLuint pos;
   assert(table);
   _glthread_LOCK_MUTEX(table->Mutex);
   for (pos=0; pos < TABLE_SIZE; pos++) {
      if (table->Table[pos]) {
         _glthread_UNLOCK_MUTEX(table->Mutex);
         return table->Table[pos]->Key;
      }
   }
   _glthread_UNLOCK_MUTEX(table->Mutex);
   return 0;
}



/**
 * Dump contents of hash table for debugging.
 * \param table - the hash table
 */
void _mesa_HashPrint(const struct _mesa_HashTable *table)
{
   GLuint i;
   assert(table);
   for (i=0;i<TABLE_SIZE;i++) {
      const struct HashEntry *entry = table->Table[i];
      while (entry) {
	 _mesa_debug(NULL, "%u %p\n", entry->Key, entry->Data);
	 entry = entry->Next;
      }
   }
}



/**
 * Find a block of 'numKeys' adjacent unused hash keys.
 * \param table - the hash table
 * \param numKeys - number of keys needed
 * \return Starting key of free block or 0 if failure
 */
GLuint _mesa_HashFindFreeKeyBlock(struct _mesa_HashTable *table, GLuint numKeys)
{
   GLuint maxKey = ~((GLuint) 0);
   _glthread_LOCK_MUTEX(table->Mutex);
   if (maxKey - numKeys > table->MaxKey) {
      /* the quick solution */
      _glthread_UNLOCK_MUTEX(table->Mutex);
      return table->MaxKey + 1;
   }
   else {
      /* the slow solution */
      GLuint freeCount = 0;
      GLuint freeStart = 1;
      GLuint key;
      for (key=1; key!=maxKey; key++) {
	 if (_mesa_HashLookup(table, key)) {
	    /* darn, this key is already in use */
	    freeCount = 0;
	    freeStart = key+1;
	 }
	 else {
	    /* this key not in use, check if we've found enough */
	    freeCount++;
	    if (freeCount == numKeys) {
               _glthread_UNLOCK_MUTEX(table->Mutex);
	       return freeStart;
	    }
	 }
      }
      /* cannot allocate a block of numKeys consecutive keys */
      _glthread_UNLOCK_MUTEX(table->Mutex);
      return 0;
   }
}



#ifdef HASH_TEST_HARNESS
int main(int argc, char *argv[])
{
   int a, b, c;
   struct HashTable *t;

   _mesa_printf("&a = %p\n", &a);
   _mesa_printf("&b = %p\n", &b);

   t = _mesa_NewHashTable();
   _mesa_HashInsert(t, 501, &a);
   _mesa_HashInsert(t, 10, &c);
   _mesa_HashInsert(t, 0xfffffff8, &b);
   _mesa_HashPrint(t);

   _mesa_printf("Find 501: %p\n", _mesa_HashLookup(t,501));
   _mesa_printf("Find 1313: %p\n", _mesa_HashLookup(t,1313));
   _mesa_printf("Find block of 100: %d\n", _mesa_HashFindFreeKeyBlock(t, 100));

   _mesa_DeleteHashTable(t);

   return 0;
}
#endif
