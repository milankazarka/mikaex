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

#ifndef __RSSENCLOSURE_H__
#define __RSSENCLOSURE_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSENCLOSURE                 (rss_enclosure_get_type ())
#define RSSENCLOSURE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSENCLOSURE, RssEnclosure))
#define RSSENCLOSURE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSENCLOSURE, RssEnclosureClass))
#define IS_RSSENCLOSURE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSENCLOSURE))
#define IS_RSSENCLOSURE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSENCLOSURE))
#define RSSENCLOSURE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSENCLOSURE, RssEnclosureClass))


typedef struct _RssEnclosure RssEnclosure;
typedef struct _RssEnclosureClass RssEnclosureClass;

struct _RssEnclosure
	{
		GObject parent;
	};

struct _RssEnclosureClass
	{
		GObjectClass parent_class;
	};

GType rss_enclosure_get_type (void) G_GNUC_CONST;

RssEnclosure *rss_enclosure_new (const gchar *url,
                                 gulong length,
                                 const gchar *type);
RssEnclosure *rss_enclosure_new_from_xml (xmlNode *xnode);

xmlNode *rss_enclosure_get_xml (RssEnclosure *rss_enclosure);


G_END_DECLS

#endif /* __RSSENCLOSURE_H__ */
