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

#include "rssenclosure.h"

enum
{
	PROP_0,
	PROP_URL,
	PROP_PORT,
	PROP_LENGTH,
	PROP_TYPE
};

static void rss_enclosure_class_init (RssEnclosureClass *klass);
static void rss_enclosure_init (RssEnclosure *rss);

static void rss_enclosure_set_property (GObject *object,
                                        guint property_id,
                                        const GValue *value,
                                        GParamSpec *pspec);
static void rss_enclosure_get_property (GObject *object,
                                        guint property_id,
                                        GValue *value,
                                        GParamSpec *pspec);

#define RSSENCLOSURE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSENCLOSURE, RssEnclosurePrivate))

typedef struct _RssEnclosurePrivate RssEnclosurePrivate;
struct _RssEnclosurePrivate
	{
		gchar *url,
		      *type;
		gulong length;
	};

GType
rss_enclosure_get_type (void)
{
	static GType rss_enclosure_type = 0;

	if (!rss_enclosure_type)
		{
			static const GTypeInfo rss_enclosure_info =
			{
				sizeof (RssEnclosureClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_enclosure_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssEnclosure),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_enclosure_init,
				NULL
			};

			rss_enclosure_type = g_type_register_static (G_TYPE_OBJECT, "RssEnclosure",
			                                             &rss_enclosure_info, 0);
		}

	return rss_enclosure_type;
}

static void
rss_enclosure_class_init (RssEnclosureClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssEnclosurePrivate));

	object_class->set_property = rss_enclosure_set_property;
	object_class->get_property = rss_enclosure_get_property;

	g_object_class_install_property (object_class, PROP_URL,
	                                 g_param_spec_string ("url",
	                                                      "URL",
	                                                      "Where the enclosure is located.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LENGTH,
	                                 g_param_spec_ulong ("length",
	                                                     "Length",
	                                                     "How big it is in bytes.",
	                                                     0,
	                                                     ULONG_MAX,
	                                                     0,
	                                                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_TYPE,
	                                 g_param_spec_string ("type",
	                                                      "Type",
	                                                      "What its type is, a standard MIME type.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_enclosure_init (RssEnclosure *rss)
{
}

/**
 * rss_enclosure_new:
 * @url: Where the enclosure is located.
 * @length: How big it is in bytes.
 * @type: What its type is, a standard MIME type.
 *
 * Returns: the newly created #RssEnclosure object.
 */
RssEnclosure
*rss_enclosure_new (const gchar *url,
                    gulong length,
                    const gchar *type)
{
	g_return_val_if_fail (url != NULL, NULL);
	g_return_val_if_fail (type != NULL, NULL);

	gchar *_url = g_strstrip (g_strdup (url)),
	      *_type = g_strstrip (g_strdup (type));

	if (strcmp (_url, "") == 0)
		{
			g_critical ("rss_enclosure_new: url cannot be an empty string");
			return NULL;
		}
	if (strcmp (_type, "") == 0)
		{
			g_critical ("rss_enclosure_new: type cannot be an empty string");
			return NULL;
		}

	RssEnclosure *rsse = RSSENCLOSURE (g_object_new (rss_enclosure_get_type (), NULL));

	g_object_set (G_OBJECT (rsse),
	              "url", _url,
	              "length", length,
	              "type", _type,
	              NULL);

	return rsse;
}

/**
 * rss_enclosure_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssEnclosure
*rss_enclosure_new_from_xml (xmlNode *xnode)
{
	g_return_val_if_fail (xnode != NULL, NULL);

	RssEnclosure *rsse = rss_enclosure_new ("url", 1, "type");
	gboolean url = FALSE, length = FALSE, type = FALSE;

	xmlChar *prop;

	if ((prop = xmlGetProp (xnode, (const xmlChar *)"url")) != NULL)
		{
			g_object_set (G_OBJECT (rsse),
			              "url", (gchar *)prop,
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"length")) != NULL)
		{
			g_object_set (G_OBJECT (rsse),
			              "length", atol ((char *)prop),
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"type")) != NULL)
		{
			g_object_set (G_OBJECT (rsse),
			              "type", (gchar *)prop,
			              NULL);
		}

	return (!url || !length || !type ? NULL : rsse);
}

/**
 * rss_enclosure_get_xml:
 * @rss_enclosure: a #RssEnclosure object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_enclosure_get_xml (RssEnclosure *rss_enclosure)
{
	RssEnclosurePrivate *priv = RSSENCLOSURE_GET_PRIVATE (rss_enclosure);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"enclosure");

	xmlSetProp (xnode, (const xmlChar *)"url", (const xmlChar *)priv->url);
	xmlSetProp (xnode, (const xmlChar *)"length", (const xmlChar *)g_strdup_printf ("%d", priv->length));
	xmlSetProp (xnode, (const xmlChar *)"type", (const xmlChar *)priv->type);

	return xnode;
}

static void
rss_enclosure_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssEnclosure *rsse = RSSENCLOSURE (object);

	RssEnclosurePrivate *priv = RSSENCLOSURE_GET_PRIVATE (rsse);

	switch (property_id)
		{
			case PROP_URL:
				priv->url = g_strdup (g_value_get_string (value));
				break;

			case PROP_LENGTH:
				priv->length = g_value_get_ulong (value);
				break;

			case PROP_TYPE:
				priv->type = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_enclosure_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssEnclosure *rsse = RSSENCLOSURE (object);

	RssEnclosurePrivate *priv = RSSENCLOSURE_GET_PRIVATE (rsse);

	switch (property_id)
		{
			case PROP_URL:
				g_value_set_string (value, priv->url);
				break;

			case PROP_LENGTH:
				g_value_set_ulong (value, priv->length);
				break;

			case PROP_TYPE:
				g_value_set_string (value, priv->type);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
