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

#include "rsstextinput.h"

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_DESCRIPTION,
	PROP_NAME,
	PROP_LINK
};

static void rss_text_input_class_init (RssTextInputClass *klass);
static void rss_text_input_init (RssTextInput *rss);

static void rss_text_input_set_property (GObject *object,
                                         guint property_id,
                                         const GValue *value,
                                         GParamSpec *pspec);
static void rss_text_input_get_property (GObject *object,
                                         guint property_id,
                                         GValue *value,
                                         GParamSpec *pspec);

#define RSSTEXTINPUT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RSSTEXTINPUT, RssTextInputPrivate))

typedef struct _RssTextInputPrivate RssTextInputPrivate;
struct _RssTextInputPrivate
	{
		gchar *title,
		      *description,
		      *name,
		      *link;
	};

GType
rss_text_input_get_type (void)
{
	static GType rss_text_input_type = 0;

	if (!rss_text_input_type)
		{
			static const GTypeInfo rss_text_input_info =
			{
				sizeof (RssTextInputClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) rss_text_input_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (RssTextInput),
				0,	/* n_preallocs */
				(GInstanceInitFunc) rss_text_input_init,
				NULL
			};

			rss_text_input_type = g_type_register_static (G_TYPE_OBJECT, "RssTextInput",
			                                              &rss_text_input_info, 0);
		}

	return rss_text_input_type;
}

static void
rss_text_input_class_init (RssTextInputClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RssTextInputPrivate));

	object_class->set_property = rss_text_input_set_property;
	object_class->get_property = rss_text_input_get_property;

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "The label of the Submit button in the text input area",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_DESCRIPTION,
	                                 g_param_spec_string ("description",
	                                                      "Description",
	                                                      "Explains the text input area",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_NAME,
	                                 g_param_spec_string ("name",
	                                                      "Name",
	                                                      "The name of the text object in the text input area",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_string ("link",
	                                                      "Link",
	                                                      "The URL of the CGI script that processes text input requests",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
rss_text_input_init (RssTextInput *rss)
{
}

/**
 * rss_text_input_new:
 * @title:
 * @description:
 * @name:
 * @link:
 *
 * Returns: the newly created #RssTextInput object.
 */
RssTextInput
*rss_text_input_new (const gchar *title,
                     const gchar *description,
                     const gchar *name,
                     const gchar *link)
{
	g_return_val_if_fail (title != NULL, NULL);
	g_return_val_if_fail (description!= NULL, NULL);
	g_return_val_if_fail (name!= NULL, NULL);
	g_return_val_if_fail (link != NULL, NULL);

	gchar *_title = g_strstrip (g_strdup (title)),
	      *_description = g_strstrip (g_strdup (description)),
	      *_name = g_strstrip (g_strdup (name)),
	      *_link = g_strstrip (g_strdup (link));

	if (strcmp (_title, "") == 0)
		{
			g_critical ("rss_text_input_new: title cannot be an empty string");
			return NULL;
		}
	if (strcmp (_description, "") == 0)
		{
			g_critical ("rss_text_input_new: description cannot be an empty string");
			return NULL;
		}
	if (strcmp (_name, "") == 0)
		{
			g_critical ("rss_text_input_new: name cannot be an empty string");
			return NULL;
		}
	if (strcmp (_link, "") == 0)
		{
			g_critical ("rss_text_input_new: link cannot be an empty string");
			return NULL;
		}

	RssTextInput *rssti = RSSTEXTINPUT (g_object_new (rss_text_input_get_type (), NULL));

	g_object_set (G_OBJECT (rssti),
	              "title", _title,
	              "description", _description,
	              "name", _name,
	              "link", _link,
	              NULL);

	return rssti;
}

/**
 * rss_text_input_new_from_xml:
 * @xnode: an #xmlNode.
 *
 */
RssTextInput
*rss_text_input_new_from_xml (xmlNode *xnode)
{
	RssTextInput *rssti = rss_text_input_new ("title", "description", "name", "link");
	gboolean title = FALSE, description = FALSE, name = FALSE, link = FALSE;

	xmlNode *cur = xnode->xmlChildrenNode;
	while (cur != NULL)
		{
			if (xmlStrcmp (cur->name, (const xmlChar *)"title") == 0)
				{
					title = TRUE;
					g_object_set (G_OBJECT (rssti),
					              "title", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"description") == 0)
				{
					description = TRUE;
					g_object_set (G_OBJECT (rssti),
					              "description", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"name") == 0)
				{
					name = TRUE;
					g_object_set (G_OBJECT (rssti),
					              "name", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"link") == 0)
				{
					link = TRUE;
					g_object_set (G_OBJECT (rssti),
					              "link", (gchar *)xmlNodeGetContent (cur),
					              NULL);
				}

			cur = cur->next;
		}

	return (!title || !description || !name || !link ? NULL : rssti);
}

/**
 * rss_text_input_get_xml:
 * @rss_text_input: a #RssTextInput object
 *
 * Returns: the #xmlNode correspondent to the item.
 */
xmlNode
*rss_text_input_get_xml (RssTextInput *rss_text_input)
{
	RssTextInputPrivate *priv = RSSTEXTINPUT_GET_PRIVATE (rss_text_input);

	xmlNode *xnode = xmlNewNode (NULL, (const xmlChar *)"textInput");

	xmlNewTextChild (xnode, NULL, (const xmlChar *)"title", (const xmlChar *)priv->title);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"description", (const xmlChar *)priv->description);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"name", (const xmlChar *)priv->name);
	xmlNewTextChild (xnode, NULL, (const xmlChar *)"link", (const xmlChar *)priv->link);

	return xnode;
}

static void
rss_text_input_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	RssTextInput *rssti = RSSTEXTINPUT (object);

	RssTextInputPrivate *priv = RSSTEXTINPUT_GET_PRIVATE (rssti);

	switch (property_id)
		{
			case PROP_TITLE:
				priv->title = g_strdup (g_value_get_string (value));
				break;

			case PROP_DESCRIPTION:
				priv->description = g_strdup (g_value_get_string (value));
				break;

			case PROP_NAME:
				priv->name = g_strdup (g_value_get_string (value));
				break;

			case PROP_LINK:
				priv->link = g_strdup (g_value_get_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
rss_text_input_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	RssTextInput *rssti = RSSTEXTINPUT (object);

	RssTextInputPrivate *priv = RSSTEXTINPUT_GET_PRIVATE (rssti);

	switch (property_id)
		{
			case PROP_TITLE:
				g_value_set_string (value, priv->title);
				break;

			case PROP_DESCRIPTION:
				g_value_set_string (value, priv->description);
				break;

			case PROP_NAME:
				g_value_set_string (value, priv->name);
				break;

			case PROP_LINK:
				g_value_set_string (value, priv->link);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
