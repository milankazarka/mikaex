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

#include <libxml/xpath.h>

#include "atomcontent.h"

enum
{
	PROP_0,
	PROP_TYPE,
	PROP_MEDIA_TYPE,
	PROP_SRC,
	PROP_TEXT
};

static void atom_content_class_init (AtomContentClass *klass);
static void atom_content_init (AtomContent *atom_content);

static void atom_content_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_content_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_CONTENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_CONTENT, AtomContentPrivate))

typedef struct _AtomContentPrivate AtomContentPrivate;
struct _AtomContentPrivate
	{
		AtomContentType type;
		gchar *media_type,
		      *src,
		      *text;
	};

GType
atom_content_get_type (void)
{
	static GType atom_content_type = 0;

	if (!atom_content_type)
		{
			static const GTypeInfo atom_content_info =
			{
				sizeof (AtomContentClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_content_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomContent),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_content_init,
				NULL
			};

			atom_content_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomContent",
			                                         &atom_content_info, 0);
		}

	return atom_content_type;
}

static void
atom_content_class_init (AtomContentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomContentPrivate));

	object_class->set_property = atom_content_set_property;
	object_class->get_property = atom_content_get_property;

	g_object_class_install_property (object_class, PROP_TYPE,
	                                 g_param_spec_uint ("type",
	                                                    "Type",
	                                                    "Type",
	                                                    0, 2, 0,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_MEDIA_TYPE,
	                                 g_param_spec_string ("media-type",
	                                                      "Media Type",
	                                                      "Media Type",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_SRC,
	                                 g_param_spec_string ("src",
	                                                      "Src",
	                                                      "Src",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_TEXT,
	                                 g_param_spec_string ("text",
	                                                      "Text",
	                                                      "Text",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_content_init (AtomContent *atom_content)
{
}

/**
 * atom_content_new:
 *
 * Returns: the newly created #AtomContent object.
 */
AtomContent
*atom_content_new (AtomContentType type)
{
	AtomContent *atomtext = ATOM_CONTENT (g_object_new (atom_content_get_type (), NULL));

	g_object_set (G_OBJECT (atomtext),
	              "type", type,
	              NULL);

	return atomtext;
}

/**
 * atom_content_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomContent object.
 */
AtomContent
*atom_content_new_from_xml (xmlNode *xnode)
{
	AtomContent *atom_content = NULL;
	AtomContentType atomtype;
	gchar *text;

	gchar *type = g_strstrip ((gchar *)xmlGetProp ((xmlNodePtr)xnode, (const xmlChar *)"type"));
	if (type == NULL || strcmp (type, "") == 0 || strcmp (type, "text") == 0)
		{
			atomtype = ATOM_CONTENT_TEXT;
		}
	else if (strcmp (type, "html") == 0)
		{
			atomtype = ATOM_CONTENT_HTML;
		}
	else if (strcmp (type, "xhtml") == 0)
		{
			atomtype = ATOM_CONTENT_XHTML;
		}
	else
		{
			atomtype = ATOM_CONTENT_MEDIA;
		}

	if (atomtype == ATOM_CONTENT_TEXT || atomtype == ATOM_CONTENT_HTML || atomtype == ATOM_CONTENT_MEDIA)
		{
			text = (gchar *)xmlNodeGetContent ((xmlNodePtr)xnode);
		}
	else if (atomtype == ATOM_CONTENT_XHTML)
		{
			xmlXPathObjectPtr xpresult;
			xmlXPathContextPtr xpcontext = xmlXPathNewContext (xnode->doc);

			xmlXPathRegisterNs (xpcontext, (const xmlChar *)"xhtml", (const xmlChar *)"http://www.w3.org/1999/xhtml");

			xpcontext->node = xnode;
			xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::xhtml:div", xpcontext);
			if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
				{
					g_critical ("Invalid AtomContent type xhtml.");
					return NULL;
				}
			else
				{
					text = (gchar *)xmlNodeGetContent ((xmlNodePtr)xpresult->nodesetval->nodeTab[0]);
				}
		}

	atom_content = atom_content_new (atomtype);

	if (atomtype == ATOM_CONTENT_MEDIA)
		{
			gchar *src = (gchar *)xmlGetProp (xnode, (const xmlChar *)"src");
			if (src != NULL)
				{
					text = NULL;
					g_object_set (G_OBJECT (atom_content),
					              "src", src,
					              NULL);
				}
		}

	if (atom_content != NULL)
		{
			atom_common_set_properties_from_xml (ATOM_COMMON (atom_content), xnode);

			if (text != NULL)
				{
					g_object_set (G_OBJECT (atom_content),
					              "text", text,
					              NULL);
				}
		}

	return atom_content;
}

/**
 * atom_content_get_xml:
 * @atom_content: an #AtomContent object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_content_get_xml (AtomContent *atom_content, xmlNode *xnode)
{
	gchar *type;

	AtomContentPrivate *priv = ATOM_CONTENT_GET_PRIVATE (atom_content);

	atom_common_get_xml (ATOM_COMMON (atom_content), xnode);

	switch (priv->type)
		{
			case ATOM_CONTENT_TEXT:
				type = g_strdup ("text");
				break;

			case ATOM_CONTENT_HTML:
				type = g_strdup ("html");
				break;

			case ATOM_CONTENT_XHTML:
				type = g_strdup ("xhtml");
				break;

			case ATOM_CONTENT_MEDIA:
				type = g_strdup (priv->media_type);
				break;
		}

	xmlSetProp (xnode, (const xmlChar *)"type", (const xmlChar *)type);

	if (priv->type == ATOM_CONTENT_MEDIA && strcmp (priv->src, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"src", (const xmlChar *)priv->src);
		}
	else if (priv->type == ATOM_CONTENT_XHTML)
		{
			xmlNode *new_xnode = xmlNewNode (NULL, (const xmlChar *)"xhtml:div");
			xmlNewNs (new_xnode, (const xmlChar *)"http://www.w3.org/1999/xhtml", (const xmlChar *)"xhtml");
			xmlNodeSetContent (new_xnode, (const xmlChar *)priv->text);
			xmlAddChild (xnode, new_xnode);
		}
	else
		{
			xmlNodeSetContent (xnode, (const xmlChar *)priv->text);
		}
}

static void
atom_content_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomContent *atom_content = ATOM_CONTENT (object);

	AtomContentPrivate *priv = ATOM_CONTENT_GET_PRIVATE (atom_content);

	switch (property_id)
		{
			case PROP_TYPE:
				priv->type = g_value_get_uint (value);
				break;

			case PROP_MEDIA_TYPE:
				priv->media_type = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_SRC:
				priv->src = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_TEXT:
				priv->text = g_strstrip (g_value_dup_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_content_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomContent *atom_content = ATOM_CONTENT (object);

	AtomContentPrivate *priv = ATOM_CONTENT_GET_PRIVATE (atom_content);

	switch (property_id)
		{
			case PROP_TYPE:
				g_value_set_int (value, priv->type);
				break;

			case PROP_MEDIA_TYPE:
				g_value_set_string (value, priv->media_type);
				break;

			case PROP_SRC:
				g_value_set_string (value, priv->src);
				break;

			case PROP_TEXT:
				g_value_set_string (value, priv->text);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
