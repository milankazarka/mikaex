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

#ifndef __RSSSOURCE_H__
#define __RSSSOURCE_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSSOURCE                 (rss_cloud_get_type ())
#define RSSSOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSSOURCE, RssSource))
#define RSSSOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSSOURCE, RssSourceClass))
#define IS_RSSSOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSSOURCE))
#define IS_RSSSOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSSOURCE))
#define RSSSOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSSOURCE, RssSourceClass))


typedef struct _RssSource RssSource;
typedef struct _RssSourceClass RssSourceClass;

struct _RssSource
	{
		GObject parent;
	};

struct _RssSourceClass
	{
		GObjectClass parent_class;
	};

GType rss_source_get_type (void) G_GNUC_CONST;

RssSource *rss_source_new (const gchar *title, const gchar *url);
RssSource *rss_source_new_from_xml (xmlNode *xnode);

xmlNode *rss_source_get_xml (RssSource *rss_source);


G_END_DECLS

#endif /* __RSSSOURCE_H__ */
