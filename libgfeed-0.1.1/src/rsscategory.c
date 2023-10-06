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

#include "rsscategory.h"

enum
{
	PROP_0,
	PROP_LOCATION,
	PROP_DOMAIN
};

static void rss_category_class_init (RssCategoryClass *klass);
static void rss_category_init (RssCategory *rss);

static void rss_category_set_property (GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec);
static void rss_category_get_property (GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec);

#define RSSCATEGORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSCATEGORY, RssCategoryPrivate))

typedef struct _RssCategoryPrivate RssCategoryPrivate;
struct _RssCategoryPrivate
	{
		gchar *location,
		      *domain;
	};

GType
rss_category_get_type (void)
{
	static GType rss_category_type = 0;

	if (!rss_category_type)
		{
			static const GTypeInfo rss_category_info =
			{
				sizeof (RssCategoryClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_category_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssCategory),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_category_init,
				NULL
			};

			rss_category_type = g_type_register_static (G_TYPE_OBJECT, "RssCategory",
			                                            &rss_category_info, 0);
		}

	return rss_category_type;
}

static void
rss_category_class_init (RssCategoryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssCategoryPrivate));

	object_class->set_property = rss_category_set_property;
	object_class->get_property = rss_category_get_property;

	g_object_class_install_property (object_class, PROP_LOCATION,
	                                 g_param_spec_string ("location",
	                                                      "Location",
	                                                      "A forward-slash-separated string that identifies a hierarchic location in the indicated taxonomy.",
	                                                      "default",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DOMAIN,
	                                 g_param_spec_string ("domain",
	                                                      "Domain",
	                                                      "A string that identifies a categorization taxonomy.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_category_init (RssCategory *rss)
{
}

/**
 * rss_category_new:
 * @location: A forward-slash-separated string that identifies a hierarchic location in the indicated taxonomy.
 *
 * Returns: the newly created #RssCategory object.
 */
RssCategory
*rss_category_new (const gchar *location)
{
	g_return_val_if_fail (location != NULL, NULL);

	gchar *_location = g_strstrip (g_strdup (location));

	if (strcmp (_location, "") == 0)
		{
			g_critical ("rss_category_new: location cannot be an empty string");
			return NULL;
		}

	RssCategory *rssc = RSSCATEGORY (g_object_new (rss_category_get_type (), NULL));

	g_object_set (G_OBJECT (rssc),
	              "location", _location,
	              NULL);

	return rssc;
}

/**
 * rss_category_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssCategory
*rss_category_new_from_xml (xmlNode *xnode)
{
	g_return_val_if_fail (xnode != NULL, NULL);

	RssCategory *rssc = rss_category_new ("location");
	gboolean location = FALSE;

	xmlChar *prop;

	if ((prop = xmlGetProp (xnode, (const xmlChar *)"domain")) != NULL)
		{
			g_object_set (G_OBJECT (rssc),
			              "domain", (gchar *)prop,
			              NULL);
		}

	gchar *text = (gchar *)xmlNodeGetContent (xnode);
	if (strcmp (g_strstrip (text), "") != 0)
		{
			location = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "location", text,
			              NULL);
		}

	return (!location ? NULL : rssc);
}

/**
 * rss_category_get_xml:
 * @rss_category: a #RssCategory object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_category_get_xml (RssCategory *rss_category)
{
	RssCategoryPrivate *priv = RSSCATEGORY_GET_PRIVATE (rss_category);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"category");

	xmlAddChild (xnode, xmlNewText ((const xmlChar *)priv->location));

	if (strcmp (priv->domain, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"domain", (const xmlChar *)priv->domain);
		}

	return xnode;
}

static void
rss_category_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssCategory *rssc = RSSCATEGORY (object);

	RssCategoryPrivate *priv = RSSCATEGORY_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_LOCATION:
				priv->location = g_strdup (g_value_get_string (value));
				break;

			case PROP_DOMAIN:
				priv->domain = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_category_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssCategory *rssc = RSSCATEGORY (object);

	RssCategoryPrivate *priv = RSSCATEGORY_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_LOCATION:
				g_value_set_string (value, priv->location);
				break;

			case PROP_DOMAIN:
				g_value_set_string (value, priv->domain);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
