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

#ifndef __ATOM_ENTRY_H__
#define __ATOM_ENTRY_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include "atomcommon.h"
#include "atomid.h"
#include "atomtext.h"
#include "atomdate.h"

G_BEGIN_DECLS


#define TYPE_ATOM_ENTRY                 (atom_entry_get_type ())
#define ATOM_ENTRY(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM_ENTRY, AtomEntry))
#define ATOM_ENTRY_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM_ENTRY, AtomEntryClass))
#define IS_ATOM_ENTRY(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM_ENTRY))
#define IS_ATOM_ENTRY_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM_ENTRY))
#define ATOM_ENTRY_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM_ENTRY, AtomEntryClass))


typedef struct _AtomEntry AtomEntry;
typedef struct _AtomEntryClass AtomEntryClass;

struct _AtomEntry
	{
		AtomCommon parent;
	};

struct _AtomEntryClass
	{
		AtomCommonClass parent_class;
	};

GType atom_entry_get_type (void) G_GNUC_CONST;

AtomEntry *atom_entry_new (AtomID *id, AtomText *title, AtomDate *updated);
AtomEntry *atom_entry_new_from_xml (xmlNode *xnode);

void atom_entry_get_xml (AtomEntry *atom_entry, xmlNode *xnode);


G_END_DECLS

#endif /* __ATOM_ENTRY_H__ */
