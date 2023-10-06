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

#ifndef __RSS_H__
#define __RSS_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include "rsschannel.h"

G_BEGIN_DECLS


#define TYPE_RSS                 (rss_get_type ())
#define RSS(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSS, Rss))
#define RSS_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSS, RssClass))
#define IS_RSS(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSS))
#define IS_RSS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSS))
#define RSS_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSS, RssClass))


typedef struct _Rss Rss;
typedef struct _RssClass RssClass;

struct _Rss
	{
		GObject parent;
	};

struct _RssClass
	{
		GObjectClass parent_class;
	};

GType rss_get_type (void) G_GNUC_CONST;

Rss *rss_new (void);
Rss *rss_new_from_file (const gchar *filename);
Rss *rss_new_from_buffer (const gchar *buffer);

xmlDoc *rss_get_xml_doc (Rss *rss);
gboolean rss_save_file (Rss *rss, const gchar *filename);


G_END_DECLS

#endif /* __RSS_H__ */
