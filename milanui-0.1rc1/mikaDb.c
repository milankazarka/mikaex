/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaServer.h"

struct mikaDb *mikaDbAlloc( struct mikaDb *db )
{
  if (!db)
    db = (struct mikaDb*)malloc(sizeof(struct mikaDb));
  return db;
}

int mikaDbRelease( struct mikaDb *db )
{
  if (!db)
    return 1;
  
  free(db->db);
  free(db);
  
  return 0;
}

struct mikaDb *mikaDbCreate( struct mikaDb *db, unsigned char *filename )
{
  if (!filename)
  {
    #ifdef DEBUG
      printf("mikaDbCreate failed - no filename provided\n");
    #endif
    return NULL;
  }
  
  if (!db)
    db = mikaDbAlloc(NULL);
  
  db->db = NULL;
  db_create(&db->db,NULL,0);
  int ret = db->db->open(db->db,NULL,(const char*)filename,NULL,DB_BTREE,DB_CREATE,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (ret!=0)
  {
    #ifdef DEBUG
      printf("mikaDbCreate failed - could not create database(%s)\n", filename);
    #endif
    mikaDbRelease(db);
    return NULL;  
  }
  
  return db;
}

struct mikaDb *mikaDbInit( struct mikaDb *db, unsigned char *filename )
{
  if (!filename)
  {
    #ifdef DEBUG
      printf("mikaDbInit failed - no filename provided\n");
    #endif
    return NULL;
  }
  
  if (!db)
    db = mikaDbAlloc(NULL);
  
  db->db = NULL;
  db_create(&db->db,NULL,0);
  int ret = db->db->open(db->db,NULL,(const char*)filename,NULL,DB_BTREE,0,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (ret!=0)
  {
    #ifdef DEBUG
      printf("mikaDbInit failed - could not init database(%s)\n", filename);
    #endif
    mikaDbRelease(db);
    return NULL;  
  }
  
  return db;
}

int mikaDbPut( struct mikaDb *db, unsigned char *key, unsigned char *value )
{
  if (!db || !key || !value)
    return 1;
  
  DBT dbkey;
  DBT dbvalue;
  
  memset(&dbkey,0,sizeof(dbkey));
  memset(&dbvalue,0,sizeof(dbvalue));
  
  dbkey.data = (void*)key;
  dbkey.size = strlen((char*)key);
  
  dbvalue.data = (void*)value;
  dbvalue.size = strlen((char*)value);
  
  int ret = db->db->put(db->db,NULL,&dbkey,&dbvalue,DB_AUTO_COMMIT);
  if (ret!=0)
  {
    #ifdef DEBUG
      printf("mikaDbPut failed\n");
    #endif
    return 1;
  }
  
  dbvalue.size = 0;
  ret = db->db->get(db->db,NULL,&dbkey,&dbvalue,0);
  if (ret!=0)
  {
    #ifdef DEBUG
      printf("mikaDbPut failed return test\n");
    #endif
    return 1;
  }
  #ifdef DEBUG
    printf("mikaDbPut retrieved ndata(%d) data(%s)\n",
      dbvalue.size,dbvalue.data);
  #endif
  
  return 0;
}
