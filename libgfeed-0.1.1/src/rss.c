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

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include "rss.h"

enum
{
	PROP_0,
	PROP_CHANNEL,
};

static void rss_class_init (RssClass *klass);
static void rss_init (Rss *rss);

static void rss_set_property (GObject *object,
                              guint property_id,
                              const GValue *value,
                              GParamSpec *pspec);
static void rss_get_property (GObject *object,
                              guint property_id,
                              GValue *value,
                              GParamSpec *pspec);

static Rss *parse_xml (xmlDoc *xdoc);

#define RSS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSS, RssPrivate))

typedef struct _RssPrivate RssPrivate;
struct _RssPrivate
	{
		RssChannel *channel;
	};

GType
rss_get_type (void)
{
	static GType rss_type = 0;

	if (!rss_type)
		{
			static const GTypeInfo rss_info =
			{
				sizeof (RssClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (Rss),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_init,
				NULL
			};

			rss_type = g_type_register_static (G_TYPE_OBJECT, "Rss",
			                                   &rss_info, 0);
		}

	return rss_type;
}

static void
rss_class_init (RssClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssPrivate));

	object_class->set_property = rss_set_property;
	object_class->get_property = rss_get_property;

	g_object_class_install_property (object_class, PROP_CHANNEL,
	                                 g_param_spec_object ("channel",
	                                                      "Channel",
	                                                      "The channel node",
	                                                      TYPE_RSSCHANNEL,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_init (Rss *rss)
{
}

/**
 * rss_new:
 *
 * Returns: the newly created #Rss object.
 */
Rss
*rss_new ()
{
	return RSS (g_object_new (rss_get_type (), NULL));
}

/**
 * rss_new_from_file:
 * @filename:
 *
 * Returns: the newly created #Rss object.
 */
Rss
*rss_new_from_file (const gchar *filename)
{
	Rss *rss;

	xmlDoc *xdoc = xmlParseFile (filename);

	rss = parse_xml (xdoc);

	return rss;
}

/**
 * rss_new_from_buffer:
 * @buffer: a pointer to an array of #gchar.
 *
 * Returns: the newly created #Rss object.
 */
Rss
*rss_new_from_buffer (const gchar *buffer)
{
	Rss *rss;

	xmlDoc *xdoc = xmlParseDoc (buffer);

	rss = parse_xml (xdoc);

	return rss;
}

/**
 * rss_get_xml_doc:
 * @rss: a #Rss object.
 *
 * Returns: the #xmlDoc correspondent to the entire rss file.
 */
xmlDoc
*rss_get_xml_doc (Rss *rss)
{
	RssPrivate *priv = RSS_GET_PRIVATE (rss);

	xmlDoc *xdoc = xmlNewDoc ((const xmlChar *)"1.0");
	xmlNode *xroot = xmlNewNode (NULL, (const xmlChar *)"rss");

  /* adding root element */
  if (xroot == NULL) return NULL;

  xmlSetProp (xroot, (const xmlChar *)"version", (const xmlChar *)"2.0");
  xmlDocSetRootElement (xdoc, xroot);

	if (priv->channel != NULL)
		{
			xmlAddChild (xroot, rss_channel_get_xml (priv->channel));
		}
	else
		{
			g_warning ("Node «channel» not present.");
		}

	return xdoc;
}

/**
 * rss_save_file:
 * @rss: a #Rss object.
 * @filename:
 *
 */
gboolean
rss_save_file (Rss *rss, const gchar *filename)
{
	return (xmlSaveFileEnc (filename, rss_get_xml_doc (rss), "iso-8859-1") > -1);
}

static void
rss_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Rss *rss = RSS (object);

	RssPrivate *priv = RSS_GET_PRIVATE (rss);

	switch (property_id)
		{
			case PROP_CHANNEL:
				if (g_value_get_object (value) != NULL)
					{
						priv->channel = (RssChannel *)g_value_dup_object (value);
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Rss *rss = RSS (object);

	RssPrivate *priv = RSS_GET_PRIVATE (rss);

	switch (property_id)
		{
			case PROP_CHANNEL:
				g_value_set_object (value, priv->channel);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static Rss
*parse_xml (xmlDoc *xdoc)
{
	Rss *rss = rss_new ();

	if (xdoc != NULL)
		{
			xmlNode *cur = xmlDocGetRootElement (xdoc);
			if (cur != NULL)
				{
					if (xmlStrcmp (cur->name, (const xmlChar *)"rss") == 0)
						{
							if (xmlStrcmp (xmlGetProp (cur, (const xmlChar *)"version"), (const xmlChar *)"2.0") == 0)
								{
									cur = cur->xmlChildrenNode;
									if (cur != NULL)
										{
											if (xmlStrcmp (cur->name, (const xmlChar *)"channel") == 0)
												{
													RssChannel *rssc = rss_channel_new_from_xml (cur);
													if (rssc != NULL)
														{
															g_object_set (G_OBJECT (rss),
															              "channel", rssc,
															              NULL);
														}
												}
											else
												{
													g_critical ("The unique first element must be «channel»");
												}
										}
									else
										{
											g_critical ("Empty rss document.");
										}
								}
							else
								{
									g_critical ("Wrong rss version.");
								}
						}
					else
						{
							g_critical ("XML document is not a rss feed.");
						}
				}
			else
				{
					g_critical ("Empty document.");
				}
		}

	return rss;
}
