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

#include "rsschannel.h"
#include "rssimage.h"
#include "rsstextinput.h"
#include "rsscloud.h"

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_LINK,
	PROP_DESCRIPTION,
	PROP_LANGUAGE,
	PROP_COPYRIGHT,
	PROP_MANAGING_EDITOR,
	PROP_WEBMASTER,
	PROP_PUB_DATE,
	PROP_LAST_BUILD_DATE,
	PROP_GENERATOR,
	PROP_DOCS,
	PROP_CLOUD,
	PROP_TTL,
	PROP_IMAGE,
	PROP_TEXT_INPUT,
	PROP_SKIP_HOURS,
	PROP_SKIP_DAYS
};

static void rss_channel_class_init (RssChannelClass *klass);
static void rss_channel_init (RssChannel *rss);

static void rss_channel_set_property (GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec);
static void rss_channel_get_property (GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

#define RSSCHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSCHANNEL, RssChannelPrivate))

typedef struct _RssChannelPrivate RssChannelPrivate;
struct _RssChannelPrivate
	{
		gchar *title,
		      *link,
		      *description,
		      *language,
		      *copyright,
		      *managingEditor,
		      *webMaster,
		      *generator,
		      *docs;
		struct tm *pubDate,
		          *lastBuildDate;
		guint ttl,
		      skipHours,
		      skipDays;
		RssCloud *cloud;
		RssImage *image;
		RssTextInput *textInput;

		GList *categories, *items;
	};

GType
rss_channel_get_type (void)
{
	static GType rss_channel_type = 0;

	if (!rss_channel_type)
		{
			static const GTypeInfo rss_channel_info =
			{
				sizeof (RssChannelClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_channel_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssChannel),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_channel_init,
				NULL
			};

			rss_channel_type = g_type_register_static (G_TYPE_OBJECT, "RssChannel",
			                                           &rss_channel_info, 0);
		}

	return rss_channel_type;
}

static void
rss_channel_class_init (RssChannelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssChannelPrivate));

	object_class->set_property = rss_channel_set_property;
	object_class->get_property = rss_channel_get_property;

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "The name of the channel",
	                                                      "default",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_string ("link",
	                                                      "Link",
	                                                      "The URL to the HTML website corresponding to the channel",
	                                                      "default",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DESCRIPTION,
	                                 g_param_spec_string ("description",
	                                                      "Description",
	                                                      "Phrase or sentence describing the channel",
	                                                      "default",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LANGUAGE,
	                                 g_param_spec_string ("language",
	                                                      "Language",
	                                                      "The language the channel is written in",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_COPYRIGHT,
	                                 g_param_spec_string ("copyright",
	                                                      "Copyright",
	                                                      "Copyright notice for content in the channel",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_MANAGING_EDITOR,
	                                 g_param_spec_string ("managing-editor",
	                                                      "Managing editor",
	                                                      "Email address for person responsible for editorial content",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_WEBMASTER,
	                                 g_param_spec_string ("webmaster",
	                                                      "Webmaster",
	                                                      "Email address for person responsible for technical issues relating to channel",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_PUB_DATE,
	                                 g_param_spec_pointer ("pub-date",
	                                                       "Publication date",
	                                                       "The publication date for the content in the channel",
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LAST_BUILD_DATE,
	                                 g_param_spec_pointer ("last-build-date",
	                                                       "Last build date",
	                                                       "The last time the content of the channel changed",
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_GENERATOR,
	                                 g_param_spec_string ("generator",
	                                                      "Generator",
	                                                      "A string indicating the program used to generate the channel",
	                                                      "<a href=\"http://libgfeed.sourceforge.net\">libgfeed v0.0.1</a>",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DOCS,
	                                 g_param_spec_string ("docs",
	                                                      "Docs",
	                                                      "A URL that points to the documentation for the format used in the RSS file",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_CLOUD,
	                                 g_param_spec_object ("cloud",
	                                                      "Cloud",
	                                                      "Allows processes to register with a cloud to be notified of updates to the channel, implementing a lightweight publish-subscribe protocol for RSS feeds.",
	                                                      TYPE_RSSCLOUD,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_TTL,
	                                 g_param_spec_uint ("ttl",
	                                                    "TTL",
	                                                    "Time To Live",
	                                                    0,
	                                                    UINT_MAX,
	                                                    0,
	                                                    G_PARAM_READWRITE));
	g_object_class_install_property (object_class, PROP_IMAGE,
	                                 g_param_spec_object ("image",
	                                                      "Image",
	                                                      "Specifies a GIF, JPEG or PNG image that can be displayed with the channel.",
	                                                      TYPE_RSSIMAGE,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_TEXT_INPUT,
	                                 g_param_spec_object ("text-input",
	                                                      "Text Input",
	                                                      "Specifies a text input box that can be displayed with the channel.",
	                                                      TYPE_RSSTEXTINPUT,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_SKIP_HOURS,
	                                 g_param_spec_uint ("skip-hours",
	                                                    "Skip hours",
	                                                    "A hint for aggregators telling them which hours they can skip",
	                                                    0,
	                                                    16777215,
	                                                    0,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_SKIP_DAYS,
	                                 g_param_spec_uint ("skip-days",
	                                                    "Skip days",
	                                                    "A hint for aggregators telling them which days they can skip",
	                                                    0,
	                                                    127,
	                                                    0,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_channel_init (RssChannel *rss_channel)
{
	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	priv->ttl = 0;
}

/**
 * rss_channel_new:
 * @title: The name of the channel. It's how people refer to your service.
 * @link: The URL to the HTML website corresponding to the channel.
 * @description: Phrase or sentence describing the channel.
 *
 * Returns: the newly created #RssChannel object.
 */
RssChannel
*rss_channel_new (const gchar *title, const gchar *link, const gchar *description)
{
	g_return_val_if_fail (title != NULL, NULL);
	g_return_val_if_fail (link != NULL, NULL);
	g_return_val_if_fail (description != NULL, NULL);

	gchar *_title = g_strstrip (g_strdup (title)),
	      *_link = g_strstrip (g_strdup (link)),
	      *_description = g_strstrip (g_strdup (description));

	if (strcmp (_title, "") == 0)
		{
			g_critical ("rss_channel_new: title cannot be an empty string");
			return NULL;
		}
	if (strcmp (_link, "") == 0)
		{
			g_critical ("rss_channel_new: link cannot be an empty string");
			return NULL;
		}
	if (strcmp (_description, "") == 0)
		{
			g_critical ("rss_channel_new: description cannot be an empty string");
			return NULL;
		}

	RssChannel *rssc = RSSCHANNEL (g_object_new (rss_channel_get_type (), NULL));

	g_object_set (G_OBJECT (rssc),
	              "title", _title,
	              "link", _link,
	              "description", _description,
	              NULL);

	return rssc;
}

/**
 * rss_channel_new_from_xml:
 * @xnode: a #xmlNode to parse.
 *
 * Returns: the newly created #RssChannel object.
 */
RssChannel
*rss_channel_new_from_xml (xmlNode *xnode)
{
	/* creating a temporary rss_channel */
	RssChannel *rssc = rss_channel_new ("title", "link", "description");
	gboolean title = FALSE, link = FALSE, description = FALSE;

	xmlNode *cur = xnode->xmlChildrenNode;
	while (cur != NULL)
		{
			if (xmlStrcmp (cur->name, (const xmlChar *)"title") == 0)
				{
					title = TRUE;
					g_object_set (G_OBJECT (rssc),
					              "title", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"link") == 0)
				{
					link = TRUE;
					g_object_set (G_OBJECT (rssc),
					              "link", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"description") == 0)
				{
					description = TRUE;
					g_object_set (G_OBJECT (rssc),
					              "description", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"language") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "language", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"copyright") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "copyright", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"managingEditor") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "managing-editor", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"webmaster") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "webmaster", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"pubDate") == 0)
				{
					struct tm tm;
					gchar *ret, *text = (gchar *)xmlNodeGetContent (cur);
					ret = (gchar *)strptime (text, "%a, %d %b %Y %T GMT", &tm);
					if (ret != NULL)
						{
							g_object_set (G_OBJECT (rssc),
							              "pub-date", &tm,
							              NULL);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"lastBuildDate") == 0)
				{
					struct tm tm;
					gchar *ret, *text = (gchar *)xmlNodeGetContent (cur);
					ret = (gchar *)strptime (text, "%a, %d %b %Y %T GMT", &tm);
					if (ret != NULL)
						{
							g_object_set (G_OBJECT (rssc),
							              "last-build-date", &tm,
							              NULL);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"category") == 0)
				{
					rss_channel_add_category (rssc, rss_category_new_from_xml (cur));
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"generator") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "generator", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"docs") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "docs", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"cloud") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "cloud", rss_cloud_new_from_xml (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"ttl") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "ttl", atol ((char *)xmlNodeGetContent (cur)),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"image") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "image", rss_image_new_from_xml (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"textInput") == 0)
				{
					g_object_set (G_OBJECT (rssc),
					              "text-input", rss_text_input_new_from_xml (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"skipHours") == 0)
				{
					xmlNode *xhours = cur->xmlChildrenNode;
					guint hours = 0;
					while (xhours != NULL)
						{
							if (xmlStrcmp (xhours->name, (const xmlChar *)"hour") == 0)
								{
									if (atol ((char *)xmlNodeGetContent (xhours)) == 0)
										{
											hours |= RSSCHANNEL_HOUR_0;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 1)
										{
											hours |= RSSCHANNEL_HOUR_1;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 2)
										{
											hours |= RSSCHANNEL_HOUR_2;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 3)
										{
											hours |= RSSCHANNEL_HOUR_3;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 4)
										{
											hours |= RSSCHANNEL_HOUR_4;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 5)
										{
											hours |= RSSCHANNEL_HOUR_5;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 6)
										{
											hours |= RSSCHANNEL_HOUR_6;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 7)
										{
											hours |= RSSCHANNEL_HOUR_7;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 8)
										{
											hours |= RSSCHANNEL_HOUR_8;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 9)
										{
											hours |= RSSCHANNEL_HOUR_9;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 10)
										{
											hours |= RSSCHANNEL_HOUR_10;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 11)
										{
											hours |= RSSCHANNEL_HOUR_11;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 12)
										{
											hours |= RSSCHANNEL_HOUR_12;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 13)
										{
											hours |= RSSCHANNEL_HOUR_13;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 14)
										{
											hours |= RSSCHANNEL_HOUR_14;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 15)
										{
											hours |= RSSCHANNEL_HOUR_15;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 16)
										{
											hours |= RSSCHANNEL_HOUR_16;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 17)
										{
											hours |= RSSCHANNEL_HOUR_17;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 18)
										{
											hours |= RSSCHANNEL_HOUR_18;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 19)
										{
											hours |= RSSCHANNEL_HOUR_19;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 20)
										{
											hours |= RSSCHANNEL_HOUR_20;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 21)
										{
											hours |= RSSCHANNEL_HOUR_21;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 22)
										{
											hours |= RSSCHANNEL_HOUR_22;
										}
									else if (atol ((char *)xmlNodeGetContent (xhours)) == 23)
										{
											hours |= RSSCHANNEL_HOUR_23;
										}
								}
						
							xhours = xhours->next;
						}

					if (hours > 0)
						{
							g_object_set (G_OBJECT (rssc),
							              "skip-hours", hours,
							              NULL);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"skipDays") == 0)
				{
					xmlNode *xdays = cur->xmlChildrenNode;
					guint days = 0;
					while (xdays != NULL)
						{
							if (xmlStrcmp (xdays->name, (const xmlChar *)"day") == 0)
								{
									if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Monday") != NULL)
										{
											days |= RSSCHANNEL_DAY_MONDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Tuesday") != NULL)
										{
											days |= RSSCHANNEL_DAY_TUESDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Wednesday") != NULL)
										{
											days |= RSSCHANNEL_DAY_WEDNESDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Thursday") != NULL)
										{
											days |= RSSCHANNEL_DAY_THURSDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Friday") != NULL)
										{
											days |= RSSCHANNEL_DAY_FRIDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Saturday") != NULL)
										{
											days |= RSSCHANNEL_DAY_SATURDAY;
										}
									else if (xmlStrcasestr (xmlNodeGetContent (xdays), (xmlChar *)"Sunday") != NULL)
										{
											days |= RSSCHANNEL_DAY_SUNDAY;
										}
								}

							xdays = xdays->next;
						}

					if (days > 0)
						{
							g_object_set (G_OBJECT (rssc),
							              "skip-days", days,
							              NULL);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"item") == 0)
				{
					rss_channel_add_item (rssc, rss_channel_item_new_from_xml (cur));
				}

			cur = cur->next;
		}

	return (!title || !link || !description ? NULL : rssc);
}

/**
 * rss_channel_add_category:
 * @rss_channel: a #RssChannel object.
 * @rss_category: a #RssCategory object.
 *
 */
gboolean
rss_channel_add_category (RssChannel *rss_channel, RssCategory *rss_category)
{
	g_return_val_if_fail (rss_category != NULL, FALSE);

	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	priv->categories = g_list_append (priv->categories, rss_category);

	return TRUE;
}

/**
 * rss_channel_get_categories:
 * @rss_channel: a #RssChannel object.
 *
 */
GList
*rss_channel_get_categories (RssChannel *rss_channel)
{
	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	return priv->categories;
}

/**
 * rss_channel_add_item:
 * @rss_channel: a #RssChannel object.
 * @rss_channel_item: a #RssChannelItem object.
 *
 */
gboolean
rss_channel_add_item (RssChannel *rss_channel, RssChannelItem *rss_channel_item)
{
	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	priv->items = g_list_append (priv->items, rss_channel_item);

	return TRUE;
}

/**
 * rss_channel_remove_item:
 * @rss_channel: a #RssChannel object.
 * @rss_channel_item: a #RssChannelItem object.
 *
 */
gboolean
rss_channel_remove_item (RssChannel *rss_channel, RssChannelItem *rss_channel_item)
{
	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	priv->items = g_list_remove (priv->items, (gconstpointer)rss_channel_item);

	return TRUE;
}

/**
 * rss_channel_get_xml:
 * @rss_channel: a #RssChannel object.
 *
 * Returns: the #xmlNode correspondent to the channel and its items.
 */
xmlNode
*rss_channel_get_xml (RssChannel *rss_channel)
{
	char pd[256];
	GList *categories, *items;
	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rss_channel);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"channel");

	xmlNewTextChild (xnode, NULL, (const xmlChar *)"title", (const xmlChar *)priv->title);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"link", (const xmlChar *)priv->link);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"description", (const xmlChar *)priv->description);

	if (strcmp (priv->language, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"language", (const xmlChar *)priv->language);
		}
	if (strcmp (priv->copyright, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"copyright", (const xmlChar *)priv->copyright);
		}
	if (strcmp (priv->managingEditor, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"managingEditor", (const xmlChar *)priv->managingEditor);
		}
	if (strcmp (priv->webMaster, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"webMaster", (const xmlChar *)priv->webMaster);
		}

	if (priv->pubDate != NULL)
		{
			strftime (pd, 256, "%a, %d %b %Y %T GMT", (const struct tm *)priv->pubDate);
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"pubDate", (const xmlChar *)pd);
		}
	if (priv->lastBuildDate != NULL)
		{
			strftime (pd, 256, "%a, %d %b %Y %T GMT", (const struct tm *)priv->lastBuildDate);
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"lastBuildDate", (const xmlChar *)pd);
		}

	categories = priv->categories;
	while (categories != NULL)
		{
			xmlAddChild (xnode, rss_category_get_xml ((RssCategory *)categories->data));

			categories = g_list_next (categories);
		}

	if (strcmp (priv->generator, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"generator", (const xmlChar *)priv->generator);
		}
	if (strcmp (priv->docs, "") != 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"docs", (const xmlChar *)priv->docs);
		}

	if (priv->cloud != NULL)
		{
			xmlAddChild (xnode, rss_cloud_get_xml (priv->cloud));
		}

	if (priv->ttl > 0)
		{
			xmlNewTextChild (xnode, NULL, (const xmlChar *)"ttl", (const xmlChar *)g_strdup_printf ("%d", priv->ttl));
		}

	if (priv->image != NULL)
		{
			xmlAddChild (xnode, rss_image_get_xml (priv->image));
		}

	if (priv->textInput!= NULL)
		{
			xmlAddChild (xnode, rss_text_input_get_xml (priv->textInput));
		}

	if (priv->skipHours > 0)
		{
			xmlNode *xnodeh = xmlNewNode (NULL, (const xmlChar *)"skipHours");
			if (priv->skipHours & RSSCHANNEL_HOUR_0)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"0");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_1)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"1");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_2)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"2");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_3)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"3");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_4)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"4");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_5)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"5");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_6)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"6");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_7)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"7");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_8)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"8");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_9)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"9");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_10)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"10");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_11)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"11");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_12)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"12");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_13)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"13");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_14)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"14");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_15)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"15");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_16)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"16");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_17)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"17");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_18)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"18");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_19)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"19");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_20)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"20");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_21)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"21");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_22)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"22");
				}
			if (priv->skipHours & RSSCHANNEL_HOUR_23)
				{
					xmlNewTextChild (xnodeh, NULL, (const xmlChar *)"hour", (const xmlChar *)"23");
				}

			xmlAddChild (xnode, xnodeh);
		}

	if (priv->skipDays > 0)
		{
			xmlNode *xnoded = xmlNewNode (NULL, (const xmlChar *)"skipDays");
			if (priv->skipDays & RSSCHANNEL_DAY_MONDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Monday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_TUESDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Tuesday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_WEDNESDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Wednesday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_THURSDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Thursday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_FRIDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Friday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_SATURDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Saturday");
				}
			if (priv->skipDays & RSSCHANNEL_DAY_SUNDAY)
				{
					xmlNewTextChild (xnoded, NULL, (const xmlChar *)"day", (const xmlChar *)"Sunday");
				}

			xmlAddChild (xnode, xnoded);
		}

	items = priv->items;
	while (items != NULL)
		{
			xmlAddChild (xnode, rss_channel_item_get_xml ((RssChannelItem *)items->data));

			items = g_list_next (items);
		}

	return xnode;
}

static void
rss_channel_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssChannel *rssc = RSSCHANNEL (object);

	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rssc);

	switch (property_id)
		{
			case PROP_TITLE:
				{
					gchar *prop = g_strdup (g_value_get_string (value));
					if (strcmp (g_strstrip (prop), "") == 0)
						{
							g_critical ("rss_channel_set_property: title cannot be an empty string");
						}
					else
						{
							priv->title = prop;
						}
				}
				break;

			case PROP_LINK:
				{
					gchar *prop = g_strdup (g_value_get_string (value));
					if (strcmp (g_strstrip (prop), "") == 0)
						{
							g_critical ("rss_channel_set_property: link cannot be an empty string");
						}
					else
						{
							priv->link = prop;
						}
				}
				break;

			case PROP_DESCRIPTION:
				{
					gchar *prop = g_strdup (g_value_get_string (value));
					if (strcmp (g_strstrip (prop), "") == 0)
						{
							g_critical ("rss_channel_set_property: description cannot be an empty string");
						}
					else
						{
							priv->description = prop;
						}
				}
				break;

			case PROP_LANGUAGE:
				priv->language = g_strdup (g_value_get_string (value));
				break;

			case PROP_COPYRIGHT:
				priv->copyright = g_strdup (g_value_get_string (value));
				break;

			case PROP_MANAGING_EDITOR:
				priv->managingEditor = g_strdup (g_value_get_string (value));
				break;

			case PROP_WEBMASTER:
				priv->webMaster = g_strdup (g_value_get_string (value));
				break;

			case PROP_PUB_DATE:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->pubDate = g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (struct tm));
					}
				break;

			case PROP_LAST_BUILD_DATE:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->lastBuildDate = g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (struct tm));
					}
				break;

			case PROP_GENERATOR:
				priv->generator = g_strdup (g_value_get_string (value));
				break;

			case PROP_DOCS:
				priv->docs = g_strdup (g_value_get_string (value));
				break;

			case PROP_CLOUD:
				if (g_value_get_object (value) != NULL)
					{
						priv->cloud = (RssCloud *)g_value_dup_object (value);
					}
				break;

			case PROP_TTL:
				priv->ttl = g_value_get_uint (value);
				break;

			case PROP_IMAGE:
				if (g_value_get_object (value) != NULL)
					{
						priv->image = (RssImage *)g_value_dup_object (value);
					}
				break;

			case PROP_TEXT_INPUT:
				if (g_value_get_object (value) != NULL)
					{
						priv->textInput = (RssTextInput *)g_value_dup_object (value);
					}
				break;

			case PROP_SKIP_HOURS:
				priv->skipHours = g_value_get_uint (value);
				break;

			case PROP_SKIP_DAYS:
				priv->skipDays = g_value_get_uint (value);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_channel_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssChannel *rssc = RSSCHANNEL (object);

	RssChannelPrivate *priv = RSSCHANNEL_GET_PRIVATE (rssc);

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

			case PROP_LANGUAGE:
				g_value_set_string (value, priv->language);
				break;

			case PROP_COPYRIGHT:
				g_value_set_string (value, priv->copyright);
				break;

			case PROP_MANAGING_EDITOR:
				g_value_set_string (value, priv->managingEditor);
				break;

			case PROP_WEBMASTER:
				g_value_set_string (value, priv->webMaster);
				break;

			case PROP_PUB_DATE:
				g_value_set_pointer (value, priv->pubDate);
				break;

			case PROP_LAST_BUILD_DATE:
				g_value_set_pointer (value, priv->lastBuildDate);
				break;

			case PROP_GENERATOR:
				g_value_set_string (value, priv->generator);
				break;

			case PROP_DOCS:
				g_value_set_string (value, priv->docs);
				break;

			case PROP_CLOUD:
				g_value_set_object (value, priv->cloud);
				break;

			case PROP_TTL:
				g_value_set_uint (value, priv->ttl);
				break;

			case PROP_IMAGE:
				g_value_set_object (value, priv->image);
				break;

			case PROP_TEXT_INPUT:
				g_value_set_object (value, priv->textInput);
				break;

			case PROP_SKIP_HOURS:
				g_value_set_uint (value, priv->skipHours);
				break;

			case PROP_SKIP_DAYS:
				g_value_set_uint (value, priv->skipDays);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
