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

#ifndef __ATOM_COMMON_H__
#define __ATOM_COMMON_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_ATOM_COMMON                 (atom_common_get_type ())
#define ATOM_COMMON(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ATOM_COMMON, AtomCommon))
#define ATOM_COMMON_COMMON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ATOM_COMMON, AtomCommonClass))
#define IS_ATOM_COMMON(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ATOM_COMMON))
#define IS_ATOM_COMMON_COMMON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ATOM_COMMON))
#define ATOM_COMMON_COMMON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ATOM_COMMON, AtomCommonClass))


typedef struct _AtomCommon AtomCommon;
typedef struct _AtomCommonClass AtomCommonClass;

struct _AtomCommon
	{
		GObject parent;
	};

struct _AtomCommonClass
	{
		GObjectClass parent_class;
	};

GType atom_common_get_type (void) G_GNUC_CONST;

AtomCommon *atom_common_new (void);

void atom_common_set_properties_from_xml (AtomCommon *atom_common, xmlNode *xnode);

void atom_common_get_xml (AtomCommon *atom_common, xmlNode *xnode);


G_END_DECLS

#endif /* __ATOM_COMMON_H__ */
