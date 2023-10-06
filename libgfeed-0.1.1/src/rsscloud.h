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

#ifndef __RSSCLOUD_H__
#define __RSSCLOUD_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSCLOUD                 (rss_cloud_get_type ())
#define RSSCLOUD(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSCLOUD, RssCloud))
#define RSSCLOUD_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSCLOUD, RssCloudClass))
#define IS_RSSCLOUD(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSCLOUD))
#define IS_RSSCLOUD_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSCLOUD))
#define RSSCLOUD_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSCLOUD, RssCloudClass))


typedef struct _RssCloud RssCloud;
typedef struct _RssCloudClass RssCloudClass;

struct _RssCloud
	{
		GObject parent;
	};

struct _RssCloudClass
	{
		GObjectClass parent_class;
	};

GType rss_cloud_get_type (void) G_GNUC_CONST;

RssCloud *rss_cloud_new (const gchar *domain,
                         guint port,
                         const gchar *path,
                         const gchar *registerProcedure,
                         const gchar *protocol);
RssCloud *rss_cloud_new_from_xml (xmlNode *xnode);

xmlNode *rss_cloud_get_xml (RssCloud *rss_cloud);


G_END_DECLS

#endif /* __RSSCLOUD_H__ */
