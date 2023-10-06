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

#ifndef __RSSTEXTINPUT_H__
#define __RSSTEXTINPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS


#define TYPE_RSSTEXTINPUT                 (rss_text_input_get_type ())
#define RSSTEXTINPUT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RSSTEXTINPUT, RssTextInput))
#define RSSTEXTINPUT_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RSSTEXTINPUT, RssTextInputClass))
#define IS_RSSTEXTINPUT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RSSTEXTINPUT))
#define IS_RSSTEXTINPUT_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RSSTEXTINPUT))
#define RSSTEXTINPUT_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RSSTEXTINPUT, RssTextInputClass))


typedef struct _RssTextInput RssTextInput;
typedef struct _RssTextInputClass RssTextInputClass;

struct _RssTextInput
	{
		GObject parent;
	};

struct _RssTextInputClass
	{
		GObjectClass parent_class;
	};

GType rss_text_input_get_type (void) G_GNUC_CONST;

RssTextInput *rss_text_input_new (const gchar *title,
                                  const gchar *description,
                                  const gchar *name,
                                  const gchar *link);
RssTextInput *rss_text_input_new_from_xml (xmlNode *xnode);

xmlNode *rss_text_input_get_xml (RssTextInput *rss_text_input);


G_END_DECLS

#endif /* __RSSTEXTINPUT_H__ */
