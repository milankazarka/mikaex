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

#include "atomperson.h"

enum
{
	PROP_0,
	PROP_NAME,
	PROP_URI,
	PROP_EMAIL
};

static void atom_person_class_init (AtomPersonClass *klass);
static void atom_person_init (AtomPerson *atom_person);

static void atom_person_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_person_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_PERSON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_PERSON, AtomPersonPrivate))

typedef struct _AtomPersonPrivate AtomPersonPrivate;
struct _AtomPersonPrivate
	{
		gchar *name,
		      *uri,
		      *email;
	};

GType
atom_person_get_type (void)
{
	static GType atom_person_type = 0;

	if (!atom_person_type)
		{
			static const GTypeInfo atom_person_info =
			{
				sizeof (AtomPersonClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_person_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomPerson),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_person_init,
				NULL
			};

			atom_person_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomPerson",
			                                           &atom_person_info, 0);
		}

	return atom_person_type;
}

static void
atom_person_class_init (AtomPersonClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomPersonPrivate));

	object_class->set_property = atom_person_set_property;
	object_class->get_property = atom_person_get_property;

	g_object_class_install_property (object_class, PROP_NAME,
	                                 g_param_spec_string ("name",
	                                                      "Name",
	                                                      "Conveys a human-readable name for the person.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_URI,
	                                 g_param_spec_string ("uri",
	                                                      "URI",
	                                                      "Conveys an IRI associated with the person.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_EMAIL,
	                                 g_param_spec_string ("email",
	                                                      "EMail",
	                                                      "Conveys an e-mail address associated with the person.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_person_init (AtomPerson *atom_person)
{
}

/**
 * atom_person_new:
 * @name: a string with a human-readable name for the person.
 *
 * Returns: the newly created #AtomPerson object.
 */
AtomPerson
*atom_person_new (const gchar *name)
{
	AtomPerson *atom_person = ATOM_PERSON (g_object_new (atom_person_get_type (), NULL));

	g_object_set (G_OBJECT (atom_person),
	              "name", name,
	              NULL);

	return atom_person;
}

/**
 * atom_person_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomPerson object.
 */
AtomPerson
*atom_person_new_from_xml (xmlNode *xnode)
{
	gchar *name;

	AtomPerson *atom_person = NULL;

	xmlXPathObjectPtr xpresult;
	xmlXPathContextPtr xpcontext = xmlXPathNewContext (xnode->doc);

	xmlXPathRegisterNs (xpcontext, (const xmlChar *)"atom", (const xmlChar *)"http://www.w3.org/2005/Atom");

	xpcontext->node = xnode;
	xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:name", xpcontext);
	if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
		{
			g_critical ("AtomPerson must have a name element.");
			return NULL;
		}
	else
		{
			name = (gchar *)xmlNodeGetContent ((xmlNodePtr)xpresult->nodesetval->nodeTab[0]);
			if (strcmp (g_strstrip (name), "") == 0)
				{
					g_critical ("Element name must be filled.");
					return NULL;
				}
		}

	atom_person = atom_person_new (name);
	if (atom_person != NULL)
		{
			atom_common_set_properties_from_xml (ATOM_COMMON (atom_person), xnode);

			xpcontext->node = xnode;
			xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:uri", xpcontext);
			if (xpresult != NULL && xpresult->nodesetval != NULL && xpresult->nodesetval->nodeNr != 0)
				{
					g_object_set (G_OBJECT (atom_person),
					              "uri", (gchar *)xmlNodeGetContent ((xmlNodePtr)xpresult->nodesetval->nodeTab[0]),
					              NULL);
				}

			xpcontext->node = xnode;
			xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:email", xpcontext);
			if (xpresult != NULL && xpresult->nodesetval != NULL && xpresult->nodesetval->nodeNr != 0)
				{
					g_object_set (G_OBJECT (atom_person),
					              "email", (gchar *)xmlNodeGetContent ((xmlNodePtr)xpresult->nodesetval->nodeTab[0]),
					              NULL);
				}
		}

	return atom_person;
}

/**
 * atom_person_get_xml:
 * @atom_person: an #AtomPerson object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_person_get_xml (AtomPerson *atom_person, xmlNode *xnode)
{
	xmlNode *new_xnode;

	AtomPersonPrivate *priv = ATOM_PERSON_GET_PRIVATE (atom_person);

	atom_common_get_xml (ATOM_COMMON (atom_person), xnode);

	new_xnode = xmlNewNode (NULL, (const xmlChar *)"name");
	xmlNodeSetContent (new_xnode, (const xmlChar *)priv->name);
	xmlAddChild (xnode, new_xnode);

	if (strcmp (priv->uri, "") != 0)
		{
			new_xnode = xmlNewNode (NULL, (const xmlChar *)"uri");
			xmlNodeSetContent (new_xnode, (const xmlChar *)priv->uri);
			xmlAddChild (xnode, new_xnode);
		}
	if (strcmp (priv->email, "") != 0)
		{
			new_xnode = xmlNewNode (NULL, (const xmlChar *)"email");
			xmlNodeSetContent (new_xnode, (const xmlChar *)priv->email);
			xmlAddChild (xnode, new_xnode);
		}
}

static void
atom_person_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomPerson *atom_person = ATOM_PERSON (object);

	AtomPersonPrivate *priv = ATOM_PERSON_GET_PRIVATE (atom_person);

	switch (property_id)
		{
			case PROP_NAME:
				{
					gchar *name = g_strstrip (g_value_dup_string (value));
					if (strcmp (name, "") == 0)
						{
							g_warning ("Property name must not be empty.");
						}
					else
						{
							priv->name = name;
						}
					break;
				}

			case PROP_URI:
				priv->uri = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_EMAIL:
				priv->email = g_strstrip (g_value_dup_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_person_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomPerson *atom_person = ATOM_PERSON (object);

	AtomPersonPrivate *priv = ATOM_PERSON_GET_PRIVATE (atom_person);

	switch (property_id)
		{
			case PROP_NAME:
				g_value_set_string (value, priv->name);
				break;

			case PROP_URI:
				g_value_set_string (value, priv->uri);
				break;

			case PROP_EMAIL:
				g_value_set_string (value, priv->email);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
