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

#include "rssguid.h"

enum
{
	PROP_0,
	PROP_GUID,
	PROP_IS_PERMALINK
};

static void rss_guid_class_init (RssGuidClass *klass);
static void rss_guid_init (RssGuid *rss);

static void rss_guid_set_property (GObject *object,
                                   guint property_id,
                                   const GValue *value,
                                   GParamSpec *pspec);
static void rss_guid_get_property (GObject *object,
                                   guint property_id,
                                   GValue *value,
                                   GParamSpec *pspec);

#define RSSGUID_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSGUID, RssGuidPrivate))

typedef struct _RssGuidPrivate RssGuidPrivate;
struct _RssGuidPrivate
	{
		gchar *guid;
		gboolean isPermaLink;
	};

GType
rss_guid_get_type (void)
{
	static GType rss_guid_type = 0;

	if (!rss_guid_type)
		{
			static const GTypeInfo rss_guid_info =
			{
				sizeof (RssGuidClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_guid_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssGuid),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_guid_init,
				NULL
			};

			rss_guid_type = g_type_register_static (G_TYPE_OBJECT, "RssGuid",
			                                        &rss_guid_info, 0);
		}

	return rss_guid_type;
}

static void
rss_guid_class_init (RssGuidClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssGuidPrivate));

	object_class->set_property = rss_guid_set_property;
	object_class->get_property = rss_guid_get_property;

	g_object_class_install_property (object_class, PROP_GUID,
	                                 g_param_spec_string ("guid",
	                                                      "GUID",
	                                                      "A string that uniquely identifies the item (Global Unique Identifier).",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_IS_PERMALINK,
	                                 g_param_spec_boolean ("is-permalink",
	                                                       "Is PermaLink",
	                                                       "True if GUID is a url that can be opened in a Web browser.",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_guid_init (RssGuid *rss)
{
}

/**
 * rss_guid_new:
 * @guid: A string that uniquely identifies the item (Global Unique Identifier).
 *
 * Returns: the newly created #RssGuid object.
 */
RssGuid
*rss_guid_new (const gchar *guid)
{
	g_return_val_if_fail (guid != NULL, NULL);

	gchar *_guid = g_strstrip (g_strdup (guid));

	if (strcmp (_guid, "") == 0)
		{
			g_critical ("rss_guid_new: guid cannot be an empty string");
			return NULL;
		}

	RssGuid *rssg = RSSGUID (g_object_new (rss_guid_get_type (), NULL));

	g_object_set (G_OBJECT (rssg),
	              "guid", _guid,
	              NULL);

	return rssg;
}

/**
 * rss_guid_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #RssGuid object.
 */
RssGuid
*rss_guid_new_from_xml (xmlNode *xnode)
{
	g_return_val_if_fail (xnode != NULL, NULL);

	RssGuid *rssg = rss_guid_new ("guid");
	gboolean guid = FALSE;

	xmlChar *prop;

	if ((prop = xmlGetProp (xnode, (const xmlChar *)"isPermaLink")) != NULL)
		{
			g_object_set (G_OBJECT (rssg),
			              "is-permalink", (gchar *)prop,
			              NULL);
		}

	gchar *text = (gchar *)xmlNodeGetContent (xnode);
	if (strcmp (g_strstrip (text), "") != 0)
		{
			guid = TRUE;
			g_object_set (G_OBJECT (rssg),
			              "guid", text,
			              NULL);
		}

	return (!guid ? NULL : rssg);
}

/**
 * rss_guid_get_xml:
 * @rss_guid: a #RssGuid object.
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_guid_get_xml (RssGuid *rss_guid)
{
	RssGuidPrivate *priv = RSSGUID_GET_PRIVATE (rss_guid);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"guid");

	xmlAddChild (xnode, xmlNewText ((const xmlChar *)priv->guid));

	if (!priv->isPermaLink)
		{
			xmlSetProp (xnode, (const xmlChar *)"isPermaLink", (const xmlChar *)"false");
		}

	return xnode;
}

static void
rss_guid_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssGuid *rssc = RSSGUID (object);

	RssGuidPrivate *priv = RSSGUID_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_GUID:
				priv->guid = g_strdup (g_value_get_string (value));
				break;

			case PROP_IS_PERMALINK:
				priv->isPermaLink = g_value_get_boolean (value);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_guid_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssGuid *rssc = RSSGUID (object);

	RssGuidPrivate *priv = RSSGUID_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_GUID:
				g_value_set_string (value, priv->guid);
				break;

			case PROP_IS_PERMALINK:
				g_value_set_boolean (value, priv->isPermaLink);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
