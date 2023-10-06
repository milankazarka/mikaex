/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaDb.h"
#include "config.h"

#define _MIKA_DB_OBJECT_SIZE 512 // default object size

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

int mikaDbClear( struct mikaDb *db )
{
  if (!db)
    return 1;
  
  db->db 	= NULL;
  db->txn 	= NULL;
  db->envp 	= NULL;
  
  return 0;
}

struct mikaDb *mikaDbCreate( struct mikaDb *db, unsigned char *filename )
{
  if (!filename)
  {
    #ifdef _DEBUG
      printf("mikaDbCreate failed - no filename provided\n");
    #endif
    return NULL;
  }
  
  if (!db)
    db = mikaDbAlloc(NULL);
  
  mikaDbClear(db);
  db_create(&db->db,NULL,0);
  int ret = db->db->open(db->db,NULL,(const char*)filename,NULL,DB_BTREE,
    DB_CREATE,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (ret!=0)
  {
    #ifdef _DEBUG
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
    #ifdef _DEBUG
      printf("mikaDbInit failed - no filename provided\n");
    #endif
    return NULL;
  }
  
  if (!db)
    db = mikaDbAlloc(NULL);
  
  mikaDbClear(db);
  db_create(&db->db,NULL,0);
  int ret = db->db->open(db->db,NULL,(const char*)filename,NULL,DB_BTREE,
    0,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (ret!=0)
  {
    #ifdef _DEBUG
      printf("mikaDbInit failed - could not init database(%s)\n", filename);
    #endif
    mikaDbRelease(db);
    return NULL;  
  }
  
  return db;
}

int mikaDbClose( struct mikaDb *db )
{
  if (!db)
    return 1;
  return 0;
}

#define _MIKA_DB_REUSE_ENVP
#define _MIKA_DB_TRANSACTIONS
#define _MIKA_DB_FORCE_COMMIT
int mikaDbPut( struct mikaDb *db, unsigned char *key, unsigned char *value )
{
  if (!db || !key || !value)
  {
    #ifdef _DEBUG
      printf("mikaDbPut error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaDbPut key(%s) value(%s)\n",
      (char*)key,(char*)value);
  #endif
  
  int ret;
  DBT dbkey;
  DBT dbvalue;
  u_int32_t env_flags;
  
  memset(&dbkey,0,sizeof(dbkey));
  memset(&dbvalue,0,sizeof(dbvalue));
  
  dbkey.data = (void*)key;
  dbkey.size = strlen((char*)key);
  
  dbvalue.data = (void*)value;
  dbvalue.size = strlen((char*)value);
  
  #ifdef _MIKA_DB_TRANSACTIONS
    env_flags = DB_CREATE |    /* Create the environment if it does
                              * not already exist. */
              DB_INIT_TXN |  /* Initialize transactions */
              DB_INIT_LOCK | /* Initialize locking. */
              DB_INIT_LOG |  /* Initialize logging */
              DB_INIT_MPOOL; /* Initialize the in-memory cache. */
  
    #ifdef _MIKA_DB_REUSE_ENVP
      if (!db->envp)
    #endif
    {
      ret = db_env_create(&db->envp, 0);
      if (ret != 0)
        db->envp->err(db->envp, ret, "Environment create failed.");
  
      const char *db_home_dir = "tmp";
      ret = db->envp->open(db->envp, db_home_dir, env_flags, 0);
    }
  
    db->txn = NULL;
    ret = db->envp->txn_begin(db->envp, NULL, &db->txn, 0);
    if (ret != 0) 
    {
        db->envp->err(db->envp, ret, "Transaction begin failed.");
    }
  #endif        
  
  ret = db->db->put(db->db,NULL,&dbkey,&dbvalue,DB_AUTO_COMMIT); // DB_AUTO_COMMIT
  if (ret!=0)
  {
    #ifdef _DEBUG
      printf("mikaDbPut failed\n");
    #endif
    return 1;
  }
  #ifdef _MIKA_DB_FORCE_COMMIT
    db->db->sync(db->db,0);
  #endif
  
  #ifdef _MIKA_DB_TRANSACTIONS
    ret = db->txn->commit(db->txn, 0);
  #endif
  //ret = envp->close(envp, 0);
  
  dbvalue.size = 0;
  ret = db->db->get(db->db,NULL,&dbkey,&dbvalue,0);
  if (ret!=0)
  {
    #ifdef _DEBUG
      printf("mikaDbPut failed return test\n");
    #endif
    return 1;
  }
  
  if (dbvalue.size>0)
  {
    unsigned char *backtest = (unsigned char*)malloc(dbvalue.size+1);
    strncpy((char*)backtest,(char*)dbvalue.data,dbvalue.size);
    backtest[dbvalue.size] = 0x00;
    #ifdef _DEBUG
      printf("mikaDbPut retrieved ndata(%d) data(%s)\n",
        dbvalue.size,(char*)backtest);
    #endif
    free(backtest);
  }
  else
  {
  }
  
  return 0;
}

int mikaDbGet( struct mikaDb *db, unsigned char *key, unsigned char *value )
{
  if (!db || !key || !value)
  {
    #ifdef _DEBUG
      printf("mikaDbGet error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaDbGet key(%s) value_size(%d)\n",
      (char*)key,strlen((char*)value));
  #endif
  
  DBT dbkey;
  DBT dbvalue;
  
  memset(&dbkey,0,sizeof(dbkey));
  memset(&dbvalue,0,sizeof(dbvalue));
  
  dbkey.data = (void*)key;
  dbkey.size = strlen((char*)key);
  dbvalue.data = (void*)value;
  value[0] = 0x00;
  dbvalue.size = 0;
  
  int ret = db->db->get(db->db,NULL,&dbkey,&dbvalue,0);
  #ifdef _DEBUG
    printf("mikaDbGet ret(%d)\n",ret);
  #endif
  if (ret==0)
  {
    strncpy((char*)value,(char*)dbvalue.data,(int)dbvalue.size);
    value[dbvalue.size] = 0x00;
    #ifdef _DEBUG
      printf("mikaDbGet key(%s) value(%s)\n",
        key,value);
    #endif
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaDbGet error\n");
    #endif
  }
  
  return 0;
}

/** remove an entry from the database
 */
int mikaDbRm( struct mikaDb *db, unsigned char *key )
{
  if (!db || !key)
    return 1;
  return 0;
}

/** list all items of the database
 */
int mikaDbList( struct mikaDb *db )
{
  if (!db)
    return 1;
  
  DBT dbkey;
  DBT dbvalue;
  int ret;
  
  static unsigned char key[_MIKA_DB_OBJECT_SIZE];
  static unsigned char value[_MIKA_DB_OBJECT_SIZE];
  
  memset(&dbkey,0,sizeof(dbkey));
  memset(&dbvalue,0,sizeof(dbvalue));   
  
  DBC *cursorp;
  db->db->cursor(db->db, NULL, &cursorp, 0);
  
  int num = 0;
  while ((ret = cursorp->c_get(cursorp, &dbkey, &dbvalue, DB_NEXT)) == 0) 
  {
    strncpy((char*)key,(char*)dbkey.data,(int)dbkey.size);
    key[dbkey.size] = 0x00;
    strncpy((char*)value,(char*)dbvalue.data,(int)dbvalue.size);
    value[dbvalue.size] = 0x00;
    #ifdef _DEBUG
      printf("mikaDbList item num(%d) key(%s) value(%s) keysize(%d) valuesize(%d)\n",
        num,(char*)key,(char*)value,dbkey.size,dbvalue.size);
    #endif
    num++;
  }
          
  
  if (cursorp != NULL)
      cursorp->c_close(cursorp);
      
  return 0;
}
