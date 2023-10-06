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

#ifndef __ATOM_TEXT_H__
#define __ATOM_TEXT_H__

#include <glib.h>
#include <libxml/tree.h>

#include "atomcommon.h"

G_BEGIN_DECLS


#define TYPE_ATOM_TEXT                 (atom_text_get_type ())
#define ATOM_TEXT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM_TEXT, AtomText))
#define ATOM_TEXT_COMMON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM_TEXT, AtomTextClass))
#define IS_ATOM_TEXT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM_TEXT))
#define IS_ATOM_TEXT_COMMON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM_TEXT))
#define ATOM_TEXT_COMMON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM_TEXT, AtomTextClass))


typedef struct _AtomText AtomText;
typedef struct _AtomTextClass AtomTextClass;

struct _AtomText
	{
		AtomCommon parent;
	};

struct _AtomTextClass
	{
		AtomCommonClass parent_class;
	};

typedef enum
{
	ATOM_TEXT_TEXT,
	ATOM_TEXT_HTML,
	ATOM_TEXT_XHTML
} AtomTextType;

GType atom_text_get_type (void) G_GNUC_CONST;

AtomText *atom_text_new (AtomTextType type, const gchar *text);
AtomText *atom_text_new_from_xml (xmlNode *xnode);

void atom_text_get_xml (AtomText *atom_text, xmlNode *xnode);


G_END_DECLS

#endif /* __ATOM_TEXT_H__ */
