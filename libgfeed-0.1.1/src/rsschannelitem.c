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

#include "rsschannelitem.h"
#include "rssenclosure.h"
#include "rssguid.h"
#include "rsssource.h"

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_LINK,
	PROP_DESCRIPTION,
	PROP_AUTHOR,
	PROP_COMMENTS,
	PROP_ENCLOSURE,
	PROP_GUID,
	PROP_PUB_DATE,
	PROP_SOURCE
};

static void rss_channel_item_class_init (RssChannelItemClass *klass);
static void rss_channel_item_init (RssChannelItem *rss);

static void rss_channel_item_set_property (GObject *object,
                                           guint property_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void rss_channel_item_get_property (GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec);

#define RSSCHANNELITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSCHANNELITEM, RssChannelItemPrivate))

typedef struct _RssChannelItemPrivate RssChannelItemPrivate;
struct _RssChannelItemPrivate
	{
		gchar *title,
		      *link,
		      *description,
		      *author,
		      *category,
		      *comments;
		RssEnclosure *enclosure;
		RssGuid *guid;
		struct tm *pubDate;
		GList *categories;
		RssSource *source;
	};

GType
rss_channel_item_get_type (void)
{
	static GType rss_channel_item_type = 0;

	if (!rss_channel_item_type)
		{
			static const GTypeInfo rss_channel_item_info =
			{
				sizeof (RssChannelItemClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_channel_item_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssChannelItem),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_channel_item_init,
				NULL
			};

			rss_channel_item_type = g_type_register_static (G_TYPE_OBJECT, "RssChannelItem",
			                                                &rss_channel_item_info, 0);
		}

	return rss_channel_item_type;
}

static void
rss_channel_item_class_init (RssChannelItemClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssChannelItemPrivate));

	object_class->set_property = rss_channel_item_set_property;
	object_class->get_property = rss_channel_item_get_property;

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "The title of the item.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_string ("link",
	                                                      "Link",
	                                                      "The URL of the item.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DESCRIPTION,
	                                 g_param_spec_string ("description",
	                                                      "Description",
	                                                      "The item synopsis.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_AUTHOR,
	                                 g_param_spec_string ("author",
	                                                      "Author",
	                                                      "Email address of the author of the item.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_COMMENTS,
	                                 g_param_spec_string ("comments",
	                                                      "Comments",
	                                                      "URL of a page for comments relating to the item.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_ENCLOSURE,
	                                 g_param_spec_object ("enclosure",
	                                                      "Enclosure",
	                                                      "Describes a media object that is attached to the item.",
	                                                      TYPE_RSSENCLOSURE,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_GUID,
	                                 g_param_spec_object ("guid",
	                                                      "GUID",
	                                                      "A string that uniquely identifies the item.",
	                                                      TYPE_RSSGUID,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_PUB_DATE,
	                                 g_param_spec_pointer ("pub-date",
	                                                       "Publication date",
	                                                       "Indicates when the item was published.",
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_SOURCE,
	                                 g_param_spec_object ("source",
	                                                      "Source",
	                                                      "The RSS channel that the item came from.",
	                                                      TYPE_RSSSOURCE,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_channel_item_init (RssChannelItem *rss)
{
}

/**
 * rss_channel_item_new:
 * @title: the title of the item.
 * @description: the item synopsis.
 *
 * Returns: the newly created #RssChannelItem object.
 */
RssChannelItem
*rss_channel_item_new (const gchar *title, const gchar *description)
{
	g_return_val_if_fail (title != NULL && description != NULL, NULL);

	gchar *_title = g_strstrip (g_strdup (title)),
	      *_description = g_strstrip (g_strdup (description));

	if (strcmp (_title, "") == 0 && strcmp (_description, "") == 0)
		{
			g_critical ("rss_channel_item_new: one of title or description must be present");
			return NULL;
		}

	RssChannelItem *rssci = RSSCHANNELITEM (g_object_new (rss_channel_item_get_type (), NULL));

	g_object_set (G_OBJECT (rssci),
	              "title", _title,
	              "description", _description,
	              NULL);

	return rssci;
}

/**
 * rss_channel_item_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssChannelItem
*rss_channel_item_new_from_xml (xmlNode *xnode)
{
	g_return_val_if_fail (xnode != NULL, NULL);

	RssChannelItem *rssci = rss_channel_item_new ("title", "description");
	gboolean title = FALSE, description = FALSE;

	xmlNode *cur = xnode->xmlChildrenNode;
	while (cur != NULL)
		{
			if (xmlStrcmp (cur->name, (const xmlChar *)"title") == 0)
				{
					title = TRUE;
					g_object_set (G_OBJECT (rssci),
					              "title", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"link") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "link", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"description") == 0)
				{
					description = TRUE;
					g_object_set (G_OBJECT (rssci),
					              "description", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"author") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "author", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"category") == 0)
				{
					rss_channel_item_add_category (rssci, rss_category_new_from_xml (cur));
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"comments") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "comments", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"enclosure") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "enclosure", rss_enclosure_new_from_xml (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"guid") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "guid", rss_guid_new_from_xml (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"pubDate") == 0)
				{
					struct tm tm;
					gchar *ret, *text = (gchar *)xmlNodeGetContent (cur);
					ret = (gchar *)strptime (text, "%a, %d %b %Y %T GMT", &tm);
					if (ret != NULL)
						{
							g_object_set (G_OBJECT (rssci),
							              "pub-date", &tm,
							              NULL);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"image") == 0)
				{
					g_object_set (G_OBJECT (rssci),
					              "image", rss_image_new_from_xml (cur),
					              NULL);
				}

			cur = cur->next;
		}

	return (!title || !description ? NULL : rssci);
}

/**
 * rss_channel_item_add_category:
 * @rss_channel_item:
 * @rss_category:
 *
 */
gboolean
rss_channel_item_add_category (RssChannelItem *rss_channel_item, RssCategory *rss_category)
{
	RssChannelItemPrivate *priv = RSSCHANNELITEM_GET_PRIVATE (rss_channel_item);

	priv->categories = g_list_append (priv->categories, rss_category);

	return TRUE;
}

/**
 * rss_channel_item_get_categories:
 * @rss_channel_item:
 *
 */
GList
*rss_channel_item_get_categories (RssChannelItem *rss_channel_item)
{
	RssChannelItemPrivate *priv = RSSCHANNELITEM_GET_PRIVATE (rss_channel_item);

	return priv->categories;
}

/**
 * rss_channel_item_get_xml:
 * @rss_channel_item: a #RssChannelItem object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_channel_item_get_xml (RssChannelItem *rss_channel_item)
{
	char pd[256];
	GList *categories;
	RssChannelItemPrivate *priv = RSSCHANNELITEM_GET_PRIVATE (rss_channel_item);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"item");

	if (strcmp (priv->title, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"title", (const xmlChar *)priv->title);
		}
	if (strcmp (priv->link, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"link", (const xmlChar *)priv->link);
		}
	if (strcmp (priv->description, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"description", (const xmlChar *)priv->description);
		}
	if (strcmp (priv->author, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"author", (const xmlChar *)priv->author);
		}

	categories = priv->categories;
	while (categories != NULL)
		{
			xmlAddChild (xnode, rss_category_get_xml ((RssCategory *)categories->data));

			categories = g_list_next (categories);
		}

	if (strcmp (priv->comments, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"comments", (const xmlChar *)priv->comments);
		}

	if (priv->enclosure != NULL)
		{
			xmlAddChild (xnode, rss_enclosure_get_xml (priv->enclosure));
		}
	if (priv->guid != NULL)
		{
			xmlAddChild (xnode, rss_guid_get_xml (priv->guid));
		}

	if (priv->pubDate != NULL)
		{
			strftime (pd, 256, "%a, %d %b %Y %T GMT", (const struct tm *)priv->pubDate);
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"pubDate", (const xmlChar *)pd);
		}

	if (priv->source != NULL)
		{
			xmlAddChild (xnode, rss_source_get_xml (priv->source));
		}

	return xnode;
}

static void
rss_channel_item_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssChannelItem *rssci = RSSCHANNELITEM (object);

	RssChannelItemPrivate *priv = RSSCHANNELITEM_GET_PRIVATE (rssci);

	switch (property_id)
		{
			case PROP_TITLE:
				priv->title = g_strdup (g_value_get_string (value));
				break;

			case PROP_LINK:
				priv->link = g_strdup (g_value_get_string (value));
				break;

			case PROP_DESCRIPTION:
				priv->description = g_strdup (g_value_get_string (value));
				break;

			case PROP_AUTHOR:
				priv->author = g_strdup (g_value_get_string (value));
				break;

			case PROP_COMMENTS:
				priv->comments = g_strdup (g_value_get_string (value));
				break;

			case PROP_ENCLOSURE:
				if (g_value_get_object (value) != NULL)
					{
						priv->enclosure = (RssEnclosure *)g_value_dup_object (value);
					}
				break;

			case PROP_GUID:
				if (g_value_get_object (value) != NULL)
					{
						priv->guid = (RssGuid *)g_value_dup_object (value);
					}
				break;

			case PROP_PUB_DATE:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->pubDate = g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (struct tm));
					}
				break;

			case PROP_SOURCE:
				if (g_value_get_object (value) != NULL)
					{
						priv->source = (RssSource *)g_value_dup_object (value);
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_channel_item_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssChannelItem *rssci = RSSCHANNELITEM (object);

	RssChannelItemPrivate *priv = RSSCHANNELITEM_GET_PRIVATE (rssci);

	switch (property_id)
		{
			case PROP_TITLE:
				g_value_set_string (value, priv->title);
				break;

			case PROP_LINK:
				g_value_set_string (value, priv->link);
				break;

			case PROP_DESCRIPTION:
				g_value_set_string (value, priv->description);
				break;

			case PROP_AUTHOR:
				g_value_set_string (value, priv->author);
				break;

			case PROP_COMMENTS:
				g_value_set_string (value, priv->comments);
				break;

			case PROP_ENCLOSURE:
				g_value_set_object (value, priv->enclosure);
				break;

			case PROP_GUID:
				g_value_set_object (value, priv->guid);
				break;

			case PROP_PUB_DATE:
				g_value_set_pointer (value, priv->pubDate);
				break;

			case PROP_SOURCE:
				g_value_set_object (value, priv->source);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
