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

#include "atomgenerator.h"

enum
{
	PROP_0,
	PROP_URI,
	PROP_VERSION,
	PROP_TEXT
};

static void atom_generator_class_init (AtomGeneratorClass *klass);
static void atom_generator_init (AtomGenerator *atom_generator);

static void atom_generator_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_generator_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_GENERATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_GENERATOR, AtomGeneratorPrivate))

typedef struct _AtomGeneratorPrivate AtomGeneratorPrivate;
struct _AtomGeneratorPrivate
	{
		gchar *uri,
		      *version,
		      *text;
	};

GType
atom_generator_get_type (void)
{
	static GType atom_generator_type = 0;

	if (!atom_generator_type)
		{
			static const GTypeInfo atom_generator_info =
			{
				sizeof (AtomGeneratorClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_generator_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomGenerator),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_generator_init,
				NULL
			};

			atom_generator_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomGenerator",
			                                              &atom_generator_info, 0);
		}

	return atom_generator_type;
}

static void
atom_generator_class_init (AtomGeneratorClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomGeneratorPrivate));

	object_class->set_property = atom_generator_set_property;
	object_class->get_property = atom_generator_get_property;

	g_object_class_install_property (object_class, PROP_URI,
	                                 g_param_spec_string ("uri",
	                                                      "URI",
	                                                      "URI",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_VERSION,
	                                 g_param_spec_string ("version",
	                                                      "Version",
	                                                      "The version of the generating agent.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_TEXT,
	                                 g_param_spec_string ("text",
	                                                      "Text",
	                                                      "A string that is a human-readable name for the generating agent.",
	                                                      "",
	                                                      G_PARAM_READWRITE));
}

static void
atom_generator_init (AtomGenerator *atom_generator)
{
}

/**
 * atom_generator_new:
 * @text: a string that is a human-readable name for the generating agent.
 *
 * Returns: the newly created #AtomGenerator object.
 */
AtomGenerator
*atom_generator_new (const gchar *text)
{
	AtomGenerator *atom_generator = ATOM_GENERATOR (g_object_new (atom_generator_get_type (), NULL));

	g_object_set (G_OBJECT (atom_generator),
	              "text", text,
	              NULL);

	return atom_generator;
}

/**
 * atom_generator_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomGenerator object.
 */
AtomGenerator
*atom_generator_new_from_xml (xmlNode *xnode)
{
	AtomGenerator *atom_generator = NULL;

	gchar *text = (gchar *)xmlNodeGetContent (xnode);
	if (text != NULL && strcmp (g_strstrip (text), "") == 0)
		{
			g_critical ("AtomGenerator must have a text content.");
			return NULL;
		}

	atom_generator = atom_generator_new (text);
	if (atom_generator != NULL)
		{
			atom_common_set_properties_from_xml (ATOM_COMMON (atom_generator), xnode);
		
			text = (gchar *)xmlGetProp (xnode, (const xmlChar *)"uri");
			if (text != NULL && strcmp (g_strstrip (text), "") != 0)
				{
					g_object_set (G_OBJECT (atom_generator),
					              "uri", text,
					              NULL);
				}

			text = (gchar *)xmlGetProp (xnode, (const xmlChar *)"version");
			if (text != NULL && strcmp (g_strstrip (text), "") != 0)
				{
					g_object_set (G_OBJECT (atom_generator),
					              "version", text,
					              NULL);
				}
		}

	return atom_generator;
}

/**
 * atom_generator_get_xml:
 * @atom_generator: an #AtomGenerator object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_generator_get_xml (AtomGenerator *atom_generator, xmlNode *xnode)
{
	xmlNode *new_xnode;

	AtomGeneratorPrivate *priv = ATOM_GENERATOR_GET_PRIVATE (atom_generator);

	atom_common_get_xml (ATOM_COMMON (atom_generator), xnode);

	if (strcmp (priv->uri, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"uri", (const xmlChar *)priv->uri);
		}
	if (strcmp (priv->version, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"version", (const xmlChar *)priv->version);
		}

	new_xnode = xmlNewNode (NULL, (const xmlChar *)"text");
	xmlNodeSetContent (new_xnode, (const xmlChar *)priv->text);
	xmlAddChild (xnode, new_xnode);
}

static void
atom_generator_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomGenerator *atom_generator = ATOM_GENERATOR (object);

	AtomGeneratorPrivate *priv = ATOM_GENERATOR_GET_PRIVATE (atom_generator);

	switch (property_id)
		{
			case PROP_URI:
				priv->uri = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_VERSION:
				priv->version = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_TEXT:
				{
					gchar *text = g_strstrip (g_value_dup_string (value));
					if (strcmp (text, "") == 0)
						{
							g_warning ("Property text must not be empty.");
						}
					else
						{
							priv->text = text;
						}
					break;
				}

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_generator_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomGenerator *atom_generator = ATOM_GENERATOR (object);

	AtomGeneratorPrivate *priv = ATOM_GENERATOR_GET_PRIVATE (atom_generator);

	switch (property_id)
		{
			case PROP_URI:
				g_value_set_string (value, priv->uri);
				break;

			case PROP_VERSION:
				g_value_set_string (value, priv->version);
				break;

			case PROP_TEXT:
				g_value_set_string (value, priv->text);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
