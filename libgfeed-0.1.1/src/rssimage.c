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

#include "rssimage.h"

enum
{
	PROP_0,
	PROP_URL,
	PROP_TITLE,
	PROP_LINK,
	PROP_WIDTH,
	PROP_HEIGHT,
	PROP_DESCRIPTION
};

static void rss_image_class_init (RssImageClass *klass);
static void rss_image_init (RssImage *rss);

static void rss_image_set_property (GObject *object,
                                           guint property_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void rss_image_get_property (GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec);

#define RSSIMAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSIMAGE, RssImagePrivate))

typedef struct _RssImagePrivate RssImagePrivate;
struct _RssImagePrivate
	{
		gchar *url,
		      *title,
		      *link,
		      *description;
		guint width, height;
	};

GType
rss_image_get_type (void)
{
	static GType rss_image_type = 0;

	if (!rss_image_type)
		{
			static const GTypeInfo rss_image_info =
			{
				sizeof (RssImageClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_image_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssImage),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_image_init,
				NULL
			};

			rss_image_type = g_type_register_static (G_TYPE_OBJECT, "RssImage",
			                                         &rss_image_info, 0);
		}

	return rss_image_type;
}

static void
rss_image_class_init (RssImageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssImagePrivate));

	object_class->set_property = rss_image_set_property;
	object_class->get_property = rss_image_get_property;

	g_object_class_install_property (object_class, PROP_URL,
	                                 g_param_spec_string ("url",
	                                                      "URL",
	                                                      "The URL of a GIF, JPEG or PNG image",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "Describes the image, it is used in the ALT attribute of the HTML img tag when the channel is rendered in HTML",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_string ("link",
	                                                      "Link",
	                                                      "Is the URL of the site, when the channel is rendered, the image is a link to the site",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_WIDTH,
	                                 g_param_spec_uint ("width",
	                                                    "Width",
	                                                    "The width of the image in pixels",
	                                                    0,
	                                                    144,
	                                                    88,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_HEIGHT,
	                                 g_param_spec_uint ("height",
	                                                    "Height",
	                                                    "The height of the image in pixels",
	                                                    0,
	                                                    400,
	                                                    31,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DESCRIPTION,
	                                 g_param_spec_string ("description",
	                                                      "Description",
	                                                      "Contains text that is included in the TITLE attribute of the link formed around the image in the HTML rendering",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_image_init (RssImage *rss)
{
}

/**
 * rss_image_new:
 * @url: the URL of a GIF, JPEG or PNG image.
 * @title: describes the image, it is used in the ALT attribute of the HTML img tag when the channel is rendered in HTML.
 * @link: iIs the URL of the site, when the channel is rendered, the image is a link to the site.
 *
 * Returns: the newly created #RssImage object.
 */
RssImage
*rss_image_new (const gchar *url, const gchar *title, const gchar *link)
{
	g_return_val_if_fail (url != NULL, NULL);
	g_return_val_if_fail (title != NULL, NULL);
	g_return_val_if_fail (link != NULL, NULL);

	gchar *_url = g_strstrip (g_strdup (url)),
	      *_title = g_strstrip (g_strdup (title)),
	      *_link = g_strstrip (g_strdup (link));

	if (strcmp (_url, "") == 0)
		{
			g_critical ("rss_image_new: url cannot be an empty string");
			return NULL;
		}
	if (strcmp (_title, "") == 0)
		{
			g_critical ("rss_image_new: title cannot be an empty string");
			return NULL;
		}
	if (strcmp (_link, "") == 0)
		{
			g_critical ("rss_image_new: link cannot be an empty string");
			return NULL;
		}

	RssImage *rssi = RSSIMAGE (g_object_new (rss_image_get_type (), NULL));

	g_object_set (G_OBJECT (rssi),
	              "url", _url,
	              "title", _title,
	              "link", _link,
	              NULL);

	return rssi;
}

/**
 * rss_image_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssImage
*rss_image_new_from_xml (xmlNode *xnode)
{
	RssImage *rssi = rss_image_new ("url", "title", "link");
	gboolean url = FALSE, title = FALSE, link = FALSE;

	xmlNode *cur = xnode->xmlChildrenNode;
	while (cur != NULL)
		{
			if (xmlStrcmp (cur->name, (const xmlChar *)"url") == 0)
				{
					url = TRUE;
					g_object_set (G_OBJECT (rssi),
					              "url", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"title") == 0)
				{
					title = TRUE;
					g_object_set (G_OBJECT (rssi),
					              "title", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"link") == 0)
				{
					link = TRUE;
					g_object_set (G_OBJECT (rssi),
					              "link", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"width") == 0)
				{
					g_object_set (G_OBJECT (rssi),
					              "width", atol ((char *)xmlNodeGetContent (cur)),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"height") == 0)
				{
					g_object_set (G_OBJECT (rssi),
					              "height", atol ((char *)xmlNodeGetContent (cur)),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"description") == 0)
				{
					g_object_set (G_OBJECT (rssi),
					              "description", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}

			cur = cur->next;
		}

	return (!url || !title || !link ? NULL : rssi);
}

/**
 * rss_image_get_xml:
 * @rss_image: a #RssImage object.
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_image_get_xml (RssImage *rss_image)
{
	RssImagePrivate *priv = RSSIMAGE_GET_PRIVATE (rss_image);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"image");

	xmlNewTextChild (xnode, NULL, (const xmlChar *)"url", (const xmlChar *)priv->url);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"title", (const xmlChar *)priv->title);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"link", (const xmlChar *)priv->link);

	if (priv->width != 88)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"width", (const xmlChar *)g_strdup_printf ("%d", priv->width));
		}
	if (priv->height != 31)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"height", (const xmlChar *)g_strdup_printf ("%d", priv->height));
		}
	if (strcmp (priv->description, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"description", (const xmlChar *)priv->description);
		}

	return xnode;
}

static void
rss_image_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssImage *rssi = RSSIMAGE (object);

	RssImagePrivate *priv = RSSIMAGE_GET_PRIVATE (rssi);

	switch (property_id)
		{
			case PROP_URL:
				priv->url = g_strdup (g_value_get_string (value));
				break;

			case PROP_TITLE:
				priv->title = g_strdup (g_value_get_string (value));
				break;

			case PROP_LINK:
				priv->link = g_strdup (g_value_get_string (value));
				break;

			case PROP_WIDTH:
				priv->width = g_value_get_uint (value);
				break;

			case PROP_HEIGHT:
				priv->height = g_value_get_uint (value);
				break;

			case PROP_DESCRIPTION:
				priv->description = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_image_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssImage *rssi = RSSIMAGE (object);

	RssImagePrivate *priv = RSSIMAGE_GET_PRIVATE (rssi);

	switch (property_id)
		{
			case PROP_URL:
				g_value_set_string (value, priv->url);
				break;

			case PROP_TITLE:
				g_value_set_string (value, priv->title);
				break;

			case PROP_LINK:
				g_value_set_string (value, priv->link);
				break;

			case PROP_WIDTH:
				g_value_set_uint (value, priv->width);
				break;

			case PROP_HEIGHT:
				g_value_set_uint (value, priv->height);
				break;

			case PROP_DESCRIPTION:
				g_value_set_string (value, priv->description);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
