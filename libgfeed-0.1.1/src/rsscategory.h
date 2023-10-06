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

#ifndef __RSSCATEGORY_H__
#define __RSSCATEGORY_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSCATEGORY                 (rss_category_get_type ())
#define RSSCATEGORY(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSCATEGORY, RssCategory))
#define RSSCATEGORY_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSCATEGORY, RssCategoryClass))
#define IS_RSSCATEGORY(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSCATEGORY))
#define IS_RSSCATEGORY_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSCATEGORY))
#define RSSCATEGORY_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSCATEGORY, RssCategoryClass))


typedef struct _RssCategory RssCategory;
typedef struct _RssCategoryClass RssCategoryClass;

struct _RssCategory
	{
		GObject parent;
	};

struct _RssCategoryClass
	{
		GObjectClass parent_class;
	};

GType rss_category_get_type (void) G_GNUC_CONST;

RssCategory *rss_category_new (const gchar *location);
RssCategory *rss_category_new_from_xml (xmlNode *xnode);

xmlNode *rss_category_get_xml (RssCategory *rss_category);


G_END_DECLS

#endif /* __RSSCATEGORY_H__ */
