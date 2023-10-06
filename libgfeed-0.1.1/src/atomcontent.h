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

#ifndef __ATOM_CONTENT_H__
#define __ATOM_CONTENT_H__

#include <glib.h>
#include <libxml/tree.h>

#include "atomcommon.h"

G_BEGIN_DECLS


#define TYPE_ATOM_CONTENT                 (atom_content_get_type ())
#define ATOM_CONTENT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM_CONTENT, AtomContent))
#define ATOM_CONTENT_COMMON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM_CONTENT, AtomContentClass))
#define IS_ATOM_CONTENT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM_CONTENT))
#define IS_ATOM_CONTENT_COMMON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM_CONTENT))
#define ATOM_CONTENT_COMMON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM_CONTENT, AtomContentClass))


typedef struct _AtomContent AtomContent;
typedef struct _AtomContentClass AtomContentClass;

struct _AtomContent
	{
		AtomCommon parent;
	};

struct _AtomContentClass
	{
		AtomCommonClass parent_class;
	};

typedef enum
{
	ATOM_CONTENT_TEXT,
	ATOM_CONTENT_HTML,
	ATOM_CONTENT_XHTML,
	ATOM_CONTENT_MEDIA
} AtomContentType;

GType atom_content_get_type (void) G_GNUC_CONST;

AtomContent *atom_content_new (AtomContentType type);
AtomContent *atom_content_new_from_xml (xmlNode *xnode);

void atom_content_get_xml (AtomContent *atom_content, xmlNode *xnode);


G_END_DECLS

#endif /* __ATOM_CONTENT_H__ */
