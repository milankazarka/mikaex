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

#ifndef __RSSCHANNELITEMITEM_H__
#define __RSSCHANNELITEMITEM_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include "rsscategory.h"

G_BEGIN_DECLS


#define TYPE_RSSCHANNELITEM                 (rss_channel_item_get_type ())
#define RSSCHANNELITEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSCHANNELITEM, RssChannelItem))
#define RSSCHANNELITEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSCHANNELITEM, RssChannelItemClass))
#define IS_RSSCHANNELITEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSCHANNELITEM))
#define IS_RSSCHANNELITEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSCHANNELITEM))
#define RSSCHANNELITEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSCHANNELITEM, RssChannelItemClass))


typedef struct _RssChannelItem RssChannelItem;
typedef struct _RssChannelItemClass RssChannelItemClass;

struct _RssChannelItem
	{
		GObject parent;
	};

struct _RssChannelItemClass
	{
		GObjectClass parent_class;
	};

GType rss_channel_item_get_type (void) G_GNUC_CONST;

RssChannelItem *rss_channel_item_new (const gchar *title, const gchar *description);
RssChannelItem *rss_channel_item_new_from_xml (xmlNode *xnode);

gboolean rss_channel_item_add_category (RssChannelItem *rss_channel_item, RssCategory *rss_category);
GList *rss_channel_item_get_categories (RssChannelItem *rss_channel_item);

xmlNode *rss_channel_item_get_xml (RssChannelItem *rss_channel_item);


G_END_DECLS

#endif /* __RSSCHANNELITEM_H__ */
