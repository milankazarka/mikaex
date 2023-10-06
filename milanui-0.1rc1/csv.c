/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "csv.h"

#define _STATIC_NO

int _trop_csv_clear_member( struct trop_Csv *csv )
{
  if (!csv) return -1;
 
  csv->next	= NULL; 
  csv->data	= NULL;
  csv->len	= 0;
  csv->members	= NULL;
  csv->next	= NULL;
  
  csv->specId	= -1;
  
  return 0;
}

int _trop_csv_trim_empty( unsigned char *str )
{
  int n;
  int len;
  
  if (!str) return -1;
  
  len = strlen(str);
  
  unsigned char *tmp = (unsigned char*)malloc(len+1);
  for (n = 0; n < len; n++)
  {
    if (str[n]!=' ' && str[n] !='\t')
      break;
  }
  memcpy(tmp,str+n,len-n);
  tmp[len-n] = 0x00;
  strcpy(str,tmp);
  free(tmp);
  return 0;
}

int trop_CsvAddMember(
  struct trop_Csv *csv,
  unsigned char *data
)
{
  struct trop_Csv *current;
  struct trop_Csv *currenti;
  
  if (!csv||!data) return -1;
  
  current = csv;
  while(current)
  {
    if (current->next==NULL)
    {
      currenti = current->members;
      while(currenti)
      {
        if (currenti->next==NULL)
        {
          currenti->next = (struct trop_Csv*)malloc(sizeof(struct trop_Csv));
          currenti = currenti->next;
          
          // TODO
          //_trop_csv_clear_member
          
          currenti->next = NULL;
          currenti->data = (unsigned char*)malloc(strlen(data)+1);
          memcpy(currenti->data,data,strlen(data));
          currenti->data[strlen(data)] = 0x00;
          currenti->len = strlen(data);
          break;
        }
        currenti = currenti->next;
      }
      if (!currenti)
      {
        current->members = (struct trop_Csv*)malloc(sizeof(struct trop_Csv));
        currenti = current->members;
        currenti->next = NULL;
        currenti->data = (unsigned char*)malloc(strlen(data)+1);
        memcpy(currenti->data,data,strlen(data));
        currenti->data[strlen(data)] = 0x00;
        currenti->len = strlen(data);
      }
      break;
    }
    current = current->next;
  }
  
  return 0;
}

/**
 *   in-line members get separated by <;>
 *
 */
struct trop_Csv *trop_CsvInit(
  struct trop_Csv *csv,
  unsigned char *filename
)
{
  FILE *fd = NULL;
  unsigned char line[_CSV_LINELEN];
  unsigned char member[_CSV_LINELEN];
  int n;
  int mode;
  int start;
  struct trop_Csv *current;

  if (!filename)
    return NULL;
  
  if ((fd = fopen(filename,"r"))==NULL)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) 
        printf(":trop_CsvInit Could not open (%s)\n",filename);
    #endif
    return NULL;
  }
  
  if (!csv)
    csv = (struct trop_Csv *)malloc(sizeof(struct trop_Csv));
  
  csv->data	= NULL;
  csv->len	= 0;
  csv->members	= NULL;
  csv->next	= NULL;
  
  while(1)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("CSV line\n");
    #endif
    if (feof(fd))
    {
      break;
    }
    line[0] = 0x00;
    fgets((char*)line,_CSV_LINELEN-1,fd);
    if (strlen((char*)line)==0)
    {
      break;
    }
    if (line[strlen((char*)line)-1]=='\n')
    {
      line[strlen((char*)line)-1] = 0x00;
    }
    mode = 0;
    start = 0;
    for(n = 0; n < strlen((char*)line); n++)
    {
      switch(mode)
      {
        case 0:
          if (line[n] == ';')
          {
            memcpy((char*)member,(char*)line+start,n-start);
            member[(n-start)] = 0x00;
            _trop_csv_trim_empty((unsigned char*)member);
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("\tCSV member (%s)\n",&member);
            #endif
            trop_CsvAddMember(csv,(unsigned char*)member);
            start = n+1;
          }
        break;
      }
    }
    
    current = csv;
    while(current)
    {
      if (!current->next)
      {
        current->next = (struct trop_Csv*)malloc(sizeof(struct trop_Csv));
        current = current->next;
        current->members	= NULL;
        current->data		= NULL;
        current->len		= 0;
        current->next		= NULL;
      }
      current = current->next;
    }
  }
    
  return csv;
}

/**
 *  get member data from the list
 *
 *  list(0)
 *  list(1)
 *    +--->member(0)
 *    +--->member(1)
 */
int trop_CsvGetDataAtPosition(
  struct trop_Csv *csv,
  int posList,
  int posMember,
  int maxLen,
  unsigned char *data,
  int *lenData
)
{
  int lCount = 0;
  int mCount = 0;
  struct trop_Csv *current;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE)
      printf("\t:trop_CsvGetDataAtPosition 00 (%d) (%d)\n",posList,posMember);
  #endif
  
  if (!csv||!data||!lenData||posList<0||posMember<0)
    return -1;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE)
      printf("\t:trop_CsvGetDataAtPosition 01 (%d) (%d)\n",posList,posMember);
  #endif
  
  current = csv;
  while(current)
  {
    if (lCount==posList)
    {
      current = current->members;
      while(current)
      {
        if (mCount==posMember)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE)
              printf(":trop_CsvGetDataAtPosition found listMember\n");
          #endif
          if (maxLen<current->len)
            *lenData = maxLen;
          else
            *lenData = current->len;
          memcpy(data,current->data,*lenData);
          return 0;
        }
        mCount++;
        current = current->next;
      }
      return 0;
    }
    lCount++;
    current = current->next;
  }
  
  return -1;
}

int trop_CsvGetListSize(
  struct trop_Csv *csv
)
{
  
  struct trop_Csv *current;
  int size = 0;
  
  current = csv;
  while(current)
  {
    size++;
    current = current->next;
  }
  
  return size;
}

int trop_CsvGetListMemberSize(
  struct trop_Csv *csv,
  int posList
)
{
  struct trop_Csv *current;
  int size = 0;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE)
      printf("\t:trop_CsvGetListMemberSize at (%d)\n",posList);
  #endif
  
  current = csv;
  while(current)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE)
        printf("\t:trop_CsvGetListMemberSize nc\n");
    #endif
    if (size==posList)
    {
      size = 0;
      current = current->members;
      while(current)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE)
            printf("\t\t:trop_CsvGetListMemberSize member(%s)\n",
              current->data);
        #endif
        size++;
        current = current->next;
      }
      break;
    }
    size++;
    current = current->next;
  }
  
  return size;
}

#ifdef _STATIC
  int main()
  {
    trop_CsvInit(NULL,"test.csv");
    return 0;
  }
#endif
