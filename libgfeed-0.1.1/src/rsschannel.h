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

#ifndef __RSSCHANNEL_H__
#define __RSSCHANNEL_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include "rsscategory.h"
#include "rsschannelitem.h"

G_BEGIN_DECLS


#define TYPE_RSSCHANNEL                 (rss_channel_get_type ())
#define RSSCHANNEL(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSCHANNEL, RssChannel))
#define RSSCHANNEL_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSCHANNEL, RssChannelClass))
#define IS_RSSCHANNEL(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSCHANNEL))
#define IS_RSSCHANNEL_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSCHANNEL))
#define RSSCHANNEL_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSCHANNEL, RssChannelClass))


#define RSSCHANNEL_DAY_MONDAY     1 << 0
#define RSSCHANNEL_DAY_TUESDAY    1 << 1
#define RSSCHANNEL_DAY_WEDNESDAY  1 << 2
#define RSSCHANNEL_DAY_THURSDAY   1 << 3
#define RSSCHANNEL_DAY_FRIDAY     1 << 4
#define RSSCHANNEL_DAY_SATURDAY   1 << 5
#define RSSCHANNEL_DAY_SUNDAY     1 << 6

#define RSSCHANNEL_HOUR_0   1 << 0
#define RSSCHANNEL_HOUR_1   1 << 1
#define RSSCHANNEL_HOUR_2   1 << 2
#define RSSCHANNEL_HOUR_3   1 << 3
#define RSSCHANNEL_HOUR_4   1 << 4
#define RSSCHANNEL_HOUR_5   1 << 5
#define RSSCHANNEL_HOUR_6   1 << 6
#define RSSCHANNEL_HOUR_7   1 << 7
#define RSSCHANNEL_HOUR_8   1 << 8
#define RSSCHANNEL_HOUR_9   1 << 9
#define RSSCHANNEL_HOUR_10  1 << 10
#define RSSCHANNEL_HOUR_11  1 << 11
#define RSSCHANNEL_HOUR_12  1 << 12
#define RSSCHANNEL_HOUR_13  1 << 13
#define RSSCHANNEL_HOUR_14  1 << 14
#define RSSCHANNEL_HOUR_15  1 << 15
#define RSSCHANNEL_HOUR_16  1 << 16
#define RSSCHANNEL_HOUR_17  1 << 17
#define RSSCHANNEL_HOUR_18  1 << 18
#define RSSCHANNEL_HOUR_19  1 << 19
#define RSSCHANNEL_HOUR_20  1 << 20
#define RSSCHANNEL_HOUR_21  1 << 21
#define RSSCHANNEL_HOUR_22  1 << 22
#define RSSCHANNEL_HOUR_23  1 << 23

typedef struct _RssChannel RssChannel;
typedef struct _RssChannelClass RssChannelClass;

struct _RssChannel
	{
		GObject parent;
	};

struct _RssChannelClass
	{
		GObjectClass parent_class;
	};

GType rss_channel_get_type (void) G_GNUC_CONST;

RssChannel *rss_channel_new (const gchar *title, const gchar *link, const gchar *description);
RssChannel *rss_channel_new_from_xml (xmlNode *xnode);

gboolean rss_channel_add_category (RssChannel *rss_channel, RssCategory *rss_category);
GList *rss_channel_get_categories (RssChannel *rss_channel);

gboolean rss_channel_add_item (RssChannel *rss_channel, RssChannelItem *rss_channel_item);
gboolean rss_channel_remove_item (RssChannel *rss_channel, RssChannelItem *rss_channel_item);

xmlNode *rss_channel_get_xml (RssChannel *rss_channel);


G_END_DECLS

#endif /* __RSSCHANNEL_H__ */
