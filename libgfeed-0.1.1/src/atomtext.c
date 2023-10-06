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

#include "atomtext.h"

enum
{
	PROP_0,
	PROP_TYPE,
	PROP_TEXT
};

static void atom_text_class_init (AtomTextClass *klass);
static void atom_text_init (AtomText *atom_text);

static void atom_text_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_text_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_TEXT, AtomTextPrivate))

typedef struct _AtomTextPrivate AtomTextPrivate;
struct _AtomTextPrivate
	{
		AtomTextType type;
		gchar *text;
	};

GType
atom_text_get_type (void)
{
	static GType atom_text_type = 0;

	if (!atom_text_type)
		{
			static const GTypeInfo atom_text_info =
			{
				sizeof (AtomTextClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_text_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomText),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_text_init,
				NULL
			};

			atom_text_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomText",
			                                         &atom_text_info, 0);
		}

	return atom_text_type;
}

static void
atom_text_class_init (AtomTextClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomTextPrivate));

	object_class->set_property = atom_text_set_property;
	object_class->get_property = atom_text_get_property;

	g_object_class_install_property (object_class, PROP_TYPE,
	                                 g_param_spec_uint ("type",
	                                                    "Type",
	                                                    "Type",
	                                                    0, 2, 0,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_TEXT,
	                                 g_param_spec_string ("text",
	                                                      "Text",
	                                                      "Text",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_text_init (AtomText *atom_text)
{
}

/**
 * atom_text_new:
 *
 * Returns: the newly created #AtomText object.
 */
AtomText
*atom_text_new (AtomTextType type, const gchar *text)
{
	AtomText *atomtext = ATOM_TEXT (g_object_new (atom_text_get_type (), NULL));

	g_object_set (G_OBJECT (atomtext),
	              "type", type,
	              "text", text,
	              NULL);

	return atomtext;
}

/**
 * atom_text_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomText object.
 */
AtomText
*atom_text_new_from_xml (xmlNode *xnode)
{
	AtomText *atom_text = NULL;
	AtomTextType atomtype;
	gchar *text;

	gchar *type = (gchar *)xmlGetProp ((xmlNodePtr)xnode, (const xmlChar *)"type");
	if (type == NULL || strcmp (type, "text") == 0)
		{
			atomtype = ATOM_TEXT_TEXT;
		}
	else if (strcmp (type, "html") == 0)
		{
			atomtype = ATOM_TEXT_HTML;
		}
	else if (strcmp (type, "xhtml") == 0)
		{
			atomtype = ATOM_TEXT_XHTML;
		}
	else
		{
			g_critical ("Invalid AtomText type.");
			return NULL;
		}

	if (atomtype == ATOM_TEXT_TEXT || atomtype == ATOM_TEXT_HTML)
		{
			text = (gchar *)xmlNodeGetContent ((xmlNodePtr)xnode);
		}
	else if (atomtype == ATOM_TEXT_XHTML)
		{
			xmlXPathObjectPtr xpresult;
			xmlXPathContextPtr xpcontext = xmlXPathNewContext (xnode->doc);

			xmlXPathRegisterNs (xpcontext, (const xmlChar *)"xhtml", (const xmlChar *)"http://www.w3.org/1999/xhtml");

			xpcontext->node = xnode;
			xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::xhtml:div", xpcontext);
			if (xpresult == NULL || xpresult->nodesetval->nodeNr != 1)
				{
					g_critical ("Invalid AtomText type xhtml.");
					return NULL;
				}
			else
				{
					text = (gchar *)xmlNodeGetContent ((xmlNodePtr)xpresult->nodesetval->nodeTab[0]);
				}
		}

	if (text != NULL)
		{
			atom_text = atom_text_new (atomtype, g_strstrip (text));
			if (atom_text != NULL)
				{
					atom_common_set_properties_from_xml (ATOM_COMMON (atom_text), xnode);
				}
		}

	return atom_text;
}

/**
 * atom_text_get_xml:
 * @atom_text: #AtomText object.
 * @xnode:
 *
 */
void
atom_text_get_xml (AtomText *atom_text, xmlNode *xnode)
{
	gchar *type;

	AtomTextPrivate *priv = ATOM_TEXT_GET_PRIVATE (atom_text);

	atom_common_get_xml (ATOM_COMMON (atom_text), xnode);

	switch (priv->type)
		{
			case ATOM_TEXT_TEXT:
				type = g_strdup ("text");
				break;

			case ATOM_TEXT_HTML:
				type = g_strdup ("html");
				break;

			case ATOM_TEXT_XHTML:
				type = g_strdup ("xhtml");
				break;
		}

	xmlSetProp (xnode, (const xmlChar *)"type", (const xmlChar *)type);
	
	if (priv->type == ATOM_TEXT_XHTML)
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
atom_text_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomText *atom_text = ATOM_TEXT (object);

	AtomTextPrivate *priv = ATOM_TEXT_GET_PRIVATE (atom_text);

	switch (property_id)
		{
			case PROP_TYPE:
				priv->type = g_value_get_uint (value);
				break;

			case PROP_TEXT:
				priv->text = g_value_dup_string (value);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_text_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomText *atom_text = ATOM_TEXT (object);

	AtomTextPrivate *priv = ATOM_TEXT_GET_PRIVATE (atom_text);

	switch (property_id)
		{
			case PROP_TYPE:
				g_value_set_int (value, priv->type);
				break;

			case PROP_TEXT:
				g_value_set_string (value, priv->text);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
