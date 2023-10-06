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

#ifndef __ATOM_H__
#define __ATOM_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include "atomcommon.h"
#include "atomid.h"
#include "atomtext.h"
#include "atomdate.h"

G_BEGIN_DECLS


#define TYPE_ATOM                 (atom_get_type ())
#define ATOM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM, Atom))
#define ATOM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM, AtomClass))
#define IS_ATOM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM))
#define IS_ATOM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM))
#define ATOM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM, AtomClass))


typedef struct _Atom Atom;
typedef struct _AtomClass AtomClass;

struct _Atom
	{
		AtomCommon parent;
	};

struct _AtomClass
	{
		AtomCommonClass parent_class;
	};

GType atom_get_type (void) G_GNUC_CONST;

Atom *atom_new (AtomID *id, AtomText *title, AtomDate *updated);
Atom *atom_new_from_file (const gchar *filename);
Atom *atom_new_from_buffer (const gchar *buffer);

xmlDoc *atom_get_xml_doc (Atom *atom);
gboolean atom_save_file (Atom *atom, const gchar *filename);


G_END_DECLS

#endif /* __ATOM_H__ */
