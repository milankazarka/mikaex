/* libgfeed
 *
 * Copyright (C) 2005-2006 Andrea Zagli <azagli@inwind.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Andrea Zagli <azagli@inwind.it>
 */

#ifndef __ATOM_DATE_H__
#define __ATOM_DATE_H__

#include <time.h>

#include <glib.h>
#include <libxml/tree.h>

#include "atomcommon.h"

G_BEGIN_DECLS


#define TYPE_ATOM_DATE                 (atom_date_get_type ())
#define ATOM_DATE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM_DATE, AtomDate))
#define ATOM_DATE_COMMON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM_DATE, AtomDateClass))
#define IS_ATOM_DATE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM_DATE))
#define IS_ATOM_DATE_COMMON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM_DATE))
#define ATOM_DATE_COMMON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM_DATE, AtomDateClass))


typedef struct _AtomDate AtomDate;
typedef struct _AtomDateClass AtomDateClass;

struct _AtomDate
	{
		AtomCommon parent;
	};

struct _AtomDateClass
	{
		AtomCommonClass parent_class;
	};

GType atom_date_get_type (void) G_GNUC_CONST;

AtomDate *atom_date_new (struct tm dateTime);
AtomDate *atom_date_new_from_xml (xmlNode *xnode);

void atom_date_get_xml (AtomDate *atom_date, xmlNode *xnode);


G_END_DECLS

#endif /* __ATOM_DATE_H__ */
