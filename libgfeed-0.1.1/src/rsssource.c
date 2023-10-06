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

#include <time.h>

#include <libxml/tree.h>

#include "rsssource.h"

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_URL
};

static void rss_source_class_init (RssSourceClass *klass);
static void rss_source_init (RssSource *rss);

static void rss_source_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void rss_source_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

#define RSSSOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSSOURCE, RssSourcePrivate))

typedef struct _RssSourcePrivate RssSourcePrivate;
struct _RssSourcePrivate
	{
		gchar *title,
		      *url;
	};

GType
rss_source_get_type (void)
{
	static GType rss_source_type = 0;

	if (!rss_source_type)
		{
			static const GTypeInfo rss_source_info =
			{
				sizeof (RssSourceClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_source_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssSource),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_source_init,
				NULL
			};

			rss_source_type = g_type_register_static (G_TYPE_OBJECT, "RssSource",
			                                          &rss_source_info, 0);
		}

	return rss_source_type;
}

static void
rss_source_class_init (RssSourceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssSourcePrivate));

	object_class->set_property = rss_source_set_property;
	object_class->get_property = rss_source_get_property;

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "Title of the source which the item came from.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_URL,
	                                 g_param_spec_string ("url",
	                                                      "URL",
	                                                      "Links to the XMLization of the source.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_source_init (RssSource *rss)
{
}

/**
 * rss_source_new:
 * @title: Title of the source which the item came from.
 * @url: Links to the XMLization of the source.
 *
 * Returns: the newly created #RssSource object.
 */
RssSource
*rss_source_new (const gchar *title, const gchar *url)
{
	g_return_val_if_fail (title != NULL, NULL);
	g_return_val_if_fail (url != NULL, NULL);

	gchar *_title = g_strstrip (g_strdup (title)),
	      *_url = g_strstrip (g_strdup (url));

	if (strcmp (_title, "") == 0)
		{
			g_critical ("rss_source_new: title cannot be an empty string");
			return NULL;
		}
	if (strcmp (_url, "") == 0)
		{
			g_critical ("rss_source_new: url cannot be an empty string");
			return NULL;
		}

	RssSource *rsss = RSSSOURCE (g_object_new (rss_source_get_type (), NULL));

	g_object_set (G_OBJECT (rsss),
	              "title", _title,
	              "url", _url,
	              NULL);

	return rsss;
}

/**
 * rss_source_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssSource
*rss_source_new_from_xml (xmlNode *xnode)
{
	RssSource *rsss = rss_source_new ("title", "url");
	gboolean title = FALSE, url = FALSE;
	gchar *str_title, *str_url;

	str_url = (gchar *)xmlGetProp (xnode, (const xmlChar *)"url");
	if (str_url != NULL || strcmp (g_strstrip (str_url), "") != 0)
		{
			url = TRUE;
			g_object_set (G_OBJECT (rsss),
			              "url", str_url,
			              NULL);
		}

	str_title = (gchar *)xmlNodeGetContent ((xmlNodePtr)xnode);
	if (str_title != NULL && strcmp (g_strstrip (str_title), "") != 0)
		{
			title = TRUE;
			g_object_set (G_OBJECT (rsss),
			              "title", str_title,
			              NULL);
		}

	return (!title || !url ? NULL : rsss);
}

/**
 * rss_source_get_xml:
 * @rss_source: a #RssSource object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_source_get_xml (RssSource *rss_source)
{
	RssSourcePrivate *priv = RSSSOURCE_GET_PRIVATE (rss_source);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"source");

	xmlAddChild (xnode, xmlNewText ((const xmlChar *)priv->title));

	xmlSetProp (xnode, (const xmlChar *)"url", (const xmlChar *)priv->url);

	return xnode;
}

static void
rss_source_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssSource *rsss = RSSSOURCE (object);

	RssSourcePrivate *priv = RSSSOURCE_GET_PRIVATE (rsss);

	switch (property_id)
		{
			case PROP_TITLE:
				priv->title = g_strdup (g_value_get_string (value));
				break;

			case PROP_URL:
				priv->url = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_source_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssSource *rsss = RSSSOURCE (object);

	RssSourcePrivate *priv = RSSSOURCE_GET_PRIVATE (rsss);

	switch (property_id)
		{
			case PROP_TITLE:
				g_value_set_string (value, priv->title);
				break;

			case PROP_URL:
				g_value_set_string (value, priv->url);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
