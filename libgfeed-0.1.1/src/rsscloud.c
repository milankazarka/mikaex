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

#include "rsscloud.h"

enum
{
	PROP_0,
	PROP_DOMAIN,
	PROP_PORT,
	PROP_PATH,
	PROP_REGISTER_PROCEDURE,
	PROP_PROTOCOL
};

static void rss_cloud_class_init (RssCloudClass *klass);
static void rss_cloud_init (RssCloud *rss);

static void rss_cloud_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void rss_cloud_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define RSSCLOUD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSCLOUD, RssCloudPrivate))

typedef struct _RssCloudPrivate RssCloudPrivate;
struct _RssCloudPrivate
	{
		gchar *domain,
		      *path,
		      *registerProcedure,
		      *protocol;
		guint port;
	};

GType
rss_cloud_get_type (void)
{
	static GType rss_cloud_type = 0;

	if (!rss_cloud_type)
		{
			static const GTypeInfo rss_cloud_info =
			{
				sizeof (RssCloudClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_cloud_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssCloud),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_cloud_init,
				NULL
			};

			rss_cloud_type = g_type_register_static (G_TYPE_OBJECT, "RssCloud",
			                                         &rss_cloud_info, 0);
		}

	return rss_cloud_type;
}

static void
rss_cloud_class_init (RssCloudClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssCloudPrivate));

	object_class->set_property = rss_cloud_set_property;
	object_class->get_property = rss_cloud_get_property;

	g_object_class_install_property (object_class, PROP_DOMAIN,
	                                 g_param_spec_string ("domain",
	                                                      "Domain",
	                                                      "Domain",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_PORT,
	                                 g_param_spec_uint ("port",
	                                                    "Port",
	                                                    "Port",
	                                                    1,
	                                                    65565,
	                                                    80,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_PATH,
	                                 g_param_spec_string ("path",
	                                                      "Path",
	                                                      "Path",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_REGISTER_PROCEDURE,
	                                 g_param_spec_string ("register-procedure",
	                                                      "Register Procedure",
	                                                      "Register Procedure",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_PROTOCOL,
	                                 g_param_spec_string ("protocol",
	                                                      "Protocol",
	                                                      "Protocol",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_cloud_init (RssCloud *rss)
{
}

/**
 * rss_cloud_new:
 * @domain:
 * @port:
 * @path:
 * @registerProcedure:
 * @protocol:
 *
 * Returns: the newly created #RssCloud object.
 */
RssCloud
*rss_cloud_new (const gchar *domain,
                guint port,
                const gchar *path,
                const gchar *registerProcedure,
                const gchar *protocol)
{
	g_return_val_if_fail (domain != NULL, NULL);
	g_return_val_if_fail (port > 0 && port < 65565, NULL);
	g_return_val_if_fail (path != NULL, NULL);
	g_return_val_if_fail (registerProcedure != NULL, NULL);
	g_return_val_if_fail (protocol != NULL, NULL);

	gchar *_domain = g_strstrip (g_strdup (domain)),
	      *_path = g_strstrip (g_strdup (path)),
	      *_registerProcedure = g_strstrip (g_strdup (registerProcedure)),
	      *_protocol = g_strstrip (g_strdup (protocol));

	if (strcmp (_domain, "") == 0)
		{
			g_critical ("rss_cloud_new: domain cannot be an empty string");
			return NULL;
		}
	if (strcmp (_path, "") == 0)
		{
			g_critical ("rss_cloud_new: path cannot be an empty string");
			return NULL;
		}
	if (strcmp (_registerProcedure, "") == 0)
		{
			g_critical ("rss_cloud_new: registerProcedure cannot be an empty string");
			return NULL;
		}
	if (strcmp (_protocol, "") == 0)
		{
			g_critical ("rss_cloud_new: protocol cannot be an empty string");
			return NULL;
		}

	RssCloud *rssc = RSSCLOUD (g_object_new (rss_cloud_get_type (), NULL));

	g_object_set (G_OBJECT (rssc),
	              "domain", _domain,
	              "port", port,
	              "path", _path,
	              "register-procedure", _registerProcedure,
	              "protocol", _protocol,
	              NULL);

	return rssc;
}

/**
 * rss_cloud_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssCloud
*rss_cloud_new_from_xml (xmlNode *xnode)
{
	g_return_val_if_fail (xnode != NULL, NULL);

	RssCloud *rssc = rss_cloud_new ("domain", 80, "path", "register-procedure", "protocol");
	gboolean domain = FALSE, port = FALSE, path = FALSE,
	         registerProcedure = FALSE, protocol = FALSE;

	xmlChar *prop;

	if ((prop = xmlGetProp (xnode, (const xmlChar *)"domain")) != NULL)
		{
			domain = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "domain", (gchar *)prop,
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"port")) != NULL)
		{
			port = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "port", atol ((char *)prop),
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"path")) != NULL)
		{
			path = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "path", (gchar *)prop,
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"registerProcedure")) != NULL)
		{
			registerProcedure = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "register-procedure", (gchar *)prop,
			              NULL);
		}
	if ((prop = xmlGetProp (xnode, (const xmlChar *)"protocol")) != NULL)
		{
			protocol = TRUE;
			g_object_set (G_OBJECT (rssc),
			              "protocol", (gchar *)prop,
			              NULL);
		}

	return (!domain || !port || !path || !registerProcedure || !protocol ? NULL : rssc);
}

/**
 * rss_cloud_get_xml:
 * @rss_cloud: a #RssCloud object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_cloud_get_xml (RssCloud *rss_cloud)
{
	RssCloudPrivate *priv = RSSCLOUD_GET_PRIVATE (rss_cloud);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"cloud");

	xmlSetProp (xnode, (const xmlChar *)"domain", (const xmlChar *)priv->domain);
	xmlSetProp (xnode, (const xmlChar *)"port", (const xmlChar *)g_strdup_printf ("%d", priv->port));
	xmlSetProp (xnode, (const xmlChar *)"path", (const xmlChar *)priv->path);
	xmlSetProp (xnode, (const xmlChar *)"registerProcedure", (const xmlChar *)priv->registerProcedure);
	xmlSetProp (xnode, (const xmlChar *)"protocol", (const xmlChar *)priv->protocol);

	return xnode;
}

static void
rss_cloud_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssCloud *rssc = RSSCLOUD (object);

	RssCloudPrivate *priv = RSSCLOUD_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_DOMAIN:
				priv->domain = g_strdup (g_value_get_string (value));
				break;

			case PROP_PORT:
				priv->port = g_value_get_uint (value);
				break;

			case PROP_PATH:
				priv->path = g_strdup (g_value_get_string (value));
				break;

			case PROP_REGISTER_PROCEDURE:
				priv->registerProcedure = g_strdup (g_value_get_string (value));
				break;

			case PROP_PROTOCOL:
				priv->protocol = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_cloud_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssCloud *rssc = RSSCLOUD (object);

	RssCloudPrivate *priv = RSSCLOUD_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_DOMAIN:
				g_value_set_string (value, priv->domain);
				break;

			case PROP_PORT:
				g_value_set_uint (value, priv->port);
				break;

			case PROP_PATH:
				g_value_set_string (value, priv->path);
				break;

			case PROP_REGISTER_PROCEDURE:
				g_value_set_string (value, priv->registerProcedure);
				break;

			case PROP_PROTOCOL:
				g_value_set_string (value, priv->protocol);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
