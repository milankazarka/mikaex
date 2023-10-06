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

#ifndef __RSSGUID_H__
#define __RSSGUID_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSGUID                 (rss_cloud_get_type ())
#define RSSGUID(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSGUID, RssGuid))
#define RSSGUID_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSGUID, RssGuidClass))
#define IS_RSSGUID(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSGUID))
#define IS_RSSGUID_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSGUID))
#define RSSGUID_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSGUID, RssGuidClass))


typedef struct _RssGuid RssGuid;
typedef struct _RssGuidClass RssGuidClass;

struct _RssGuid
	{
		GObject parent;
	};

struct _RssGuidClass
	{
		GObjectClass parent_class;
	};

GType rss_guid_get_type (void) G_GNUC_CONST;

RssGuid *rss_guid_new (const gchar *guid);
RssGuid *rss_guid_new_from_xml (xmlNode *xnode);

xmlNode *rss_guid_get_xml (RssGuid *rss_guid);


G_END_DECLS

#endif /* __RSSGUID_H__ */
