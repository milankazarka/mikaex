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

#ifndef __RSSIMAGE_H__
#define __RSSIMAGE_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSIMAGE                 (rss_image_get_type ())
#define RSSIMAGE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSIMAGE, RssImage))
#define RSSIMAGE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSIMAGE, RssImageClass))
#define IS_RSSIMAGE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSIMAGE))
#define IS_RSSIMAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSIMAGE))
#define RSSIMAGE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSIMAGE, RssImageClass))


typedef struct _RssImage RssImage;
typedef struct _RssImageClass RssImageClass;

struct _RssImage
	{
		GObject parent;
	};

struct _RssImageClass
	{
		GObjectClass parent_class;
	};

GType rss_image_get_type (void) G_GNUC_CONST;

RssImage *rss_image_new (const gchar *url, const gchar *title, const gchar *link);
RssImage *rss_image_new_from_xml (xmlNode *xnode);

xmlNode *rss_image_get_xml (RssImage *rss_image);


G_END_DECLS

#endif /* __RSSIMAGE_H__ */
