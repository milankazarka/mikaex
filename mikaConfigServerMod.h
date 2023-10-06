/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_CONFIG_SERVER_MOD_H_
#define _MIKA_CONFIG_SERVER_MOD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define _ACTIVITIES_ROOT "/server/ftp/activities"
#define _ACTIVITY_PATH_LEN 512

void *_process_data_config_server( unsigned char *in, unsigned char *out );
int mikaConfigServerDirectoryListing( unsigned char *path, unsigned char *out );

#endif
