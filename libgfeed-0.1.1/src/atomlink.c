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

#include "atomlink.h"

enum
{
	PROP_0,
	PROP_HREF,
	PROP_REL,
	PROP_TYPE,
	PROP_HREFLANG,
	PROP_TITLE,
	PROP_LENGTH
};

static void atom_link_class_init (AtomLinkClass *klass);
static void atom_link_init (AtomLink *atom_link);

static void atom_link_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_link_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_LINK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_LINK, AtomLinkPrivate))

typedef struct _AtomLinkPrivate AtomLinkPrivate;
struct _AtomLinkPrivate
	{
		gchar *href,
		      *rel,
		      *type,
		      *hreflang,
		      *title,
		      *length;
	};

GType
atom_link_get_type (void)
{
	static GType atom_link_type = 0;

	if (!atom_link_type)
		{
			static const GTypeInfo atom_link_info =
			{
				sizeof (AtomLinkClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_link_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomLink),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_link_init,
				NULL
			};

			atom_link_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomLink",
			                                              &atom_link_info, 0);
		}

	return atom_link_type;
}

static void
atom_link_class_init (AtomLinkClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomLinkPrivate));

	object_class->set_property = atom_link_set_property;
	object_class->get_property = atom_link_get_property;

	g_object_class_install_property (object_class, PROP_HREF,
	                                 g_param_spec_string ("href",
	                                                      "Href",
	                                                      "Href",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_REL,
	                                 g_param_spec_string ("rel",
	                                                      "Rel",
	                                                      "Indicates the link relation type.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TYPE,
	                                 g_param_spec_string ("type",
	                                                      "Type",
	                                                      "An advisory media type: it is a hint about the type of the representation that is expected to be returned when the value of the href attribute is dereferenced.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_HREFLANG,
	                                 g_param_spec_string ("hreflang",
	                                                      "Hreflang",
	                                                      "Describes the language of the resource pointed to by the href attribute.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_string ("title",
	                                                      "Title",
	                                                      "A human-readable information about the link.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LENGTH,
	                                 g_param_spec_string ("length",
	                                                      "Length",
	                                                      "An advisory length of the linked content in octets; it is a hint about the content length of the representation returned when the IRI in the href attribute is mapped to a URI and dereferenced.",
	                                                      "",
	                                                      G_PARAM_READWRITE));
}

static void
atom_link_init (AtomLink *atom_link)
{
}

/**
 * atom_link_new:
 * @href:
 *
 * Returns: the newly created #AtomLink object.
 */
AtomLink
*atom_link_new (const gchar *href)
{
	AtomLink *atom_link = ATOM_LINK (g_object_new (atom_link_get_type (), NULL));

	g_object_set (G_OBJECT (atom_link),
	              "href", href,
	              NULL);

	return atom_link;
}

/**
 * atom_link_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomLink object.
 */
AtomLink
*atom_link_new_from_xml (xmlNode *xnode)
{
	AtomLink *atom_link = NULL;

	gchar *href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"href");
	if (href == NULL || strcmp (g_strstrip (href), "") == 0)
		{
			g_critical ("AtomLink must have an href attribute.");
			return NULL;
		}
	else
		{
			atom_link = atom_link_new (href);
			if (atom_link != NULL)
				{
					atom_common_set_properties_from_xml (ATOM_COMMON (atom_link), xnode);

					href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"rel");
					if (href != NULL && strcmp (g_strstrip (href), "") != 0)
						{
							g_object_set (G_OBJECT (atom_link),
							              "rel", href,
							              NULL);
						}

					href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"type");
					if (href != NULL && strcmp (g_strstrip (href), "") != 0)
						{
							g_object_set (G_OBJECT (atom_link),
							              "type", href,
							              NULL);
						}

					href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"hreflang");
					if (href != NULL && strcmp (g_strstrip (href), "") != 0)
						{
							g_object_set (G_OBJECT (atom_link),
							              "hreflang", href,
							              NULL);
						}

					href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"title");
					if (href != NULL && strcmp (g_strstrip (href), "") != 0)
						{
							g_object_set (G_OBJECT (atom_link),
							              "title", href,
							              NULL);
						}

					href = (gchar *)xmlGetProp (xnode, (const xmlChar *)"length");
					if (href != NULL && strcmp (g_strstrip (href), "") != 0)
						{
							g_object_set (G_OBJECT (atom_link),
							              "length", href,
							              NULL);
						}
				}
		}

	return atom_link;
}

/**
 * atom_link_get_xml:
 * @atom_link: an #AtomLink object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_link_get_xml (AtomLink *atom_link, xmlNode *xnode)
{
	xmlNode *new_xnode;

	AtomLinkPrivate *priv = ATOM_LINK_GET_PRIVATE (atom_link);

	atom_common_get_xml (ATOM_COMMON (atom_link), xnode);

	if (strcmp (priv->href, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"href", (const xmlChar *)priv->href);
		}
	if (strcmp (priv->rel, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"rel", (const xmlChar *)priv->rel);
		}
	if (strcmp (priv->type, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"type", (const xmlChar *)priv->type);
		}
	if (strcmp (priv->hreflang, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"hreflang", (const xmlChar *)priv->hreflang);
		}
	if (strcmp (priv->title, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"title", (const xmlChar *)priv->title);
		}
	if (strcmp (priv->length, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"length", (const xmlChar *)priv->length);
		}
}

static void
atom_link_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomLink *atom_link = ATOM_LINK (object);

	AtomLinkPrivate *priv = ATOM_LINK_GET_PRIVATE (atom_link);

	switch (property_id)
		{
			case PROP_HREF:
				{
					gchar *href = g_strstrip (g_value_dup_string (value));
					if (strcmp (href, "") == 0)
						{
							g_warning ("Property href must not be empty.");
						}
					else
						{
							priv->href = href;
						}
					break;
				}

			case PROP_REL:
				priv->rel = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_TYPE:
				priv->type = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_HREFLANG:
				priv->hreflang = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_TITLE:
				priv->title = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_LENGTH:
				priv->length = g_strstrip (g_value_dup_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_link_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomLink *atom_link = ATOM_LINK (object);

	AtomLinkPrivate *priv = ATOM_LINK_GET_PRIVATE (atom_link);

	switch (property_id)
		{
			case PROP_HREF:
				g_value_set_string (value, priv->href);
				break;

			case PROP_REL:
				g_value_set_string (value, priv->rel);
				break;

			case PROP_TYPE:
				g_value_set_string (value, priv->type);
				break;

			case PROP_HREFLANG:
				g_value_set_string (value, priv->hreflang);
				break;

			case PROP_TITLE:
				g_value_set_string (value, priv->title);
				break;

			case PROP_LENGTH:
				g_value_set_string (value, priv->length);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
