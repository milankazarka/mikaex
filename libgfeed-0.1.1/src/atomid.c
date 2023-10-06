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

#include "atomid.h"

enum
{
	PROP_0,
	PROP_URI
};

static void atom_id_class_init (AtomIDClass *klass);
static void atom_id_init (AtomID *atom_id);

static void atom_id_set_property (GObject *object,
                                  guint property_id,
                                  const GValue *value,
                                  GParamSpec *pspec);
static void atom_id_get_property (GObject *object,
                                  guint property_id,
                                  GValue *value,
                                  GParamSpec *pspec);

#define ATOM_ID_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_ID, AtomIDPrivate))

typedef struct _AtomIDPrivate AtomIDPrivate;
struct _AtomIDPrivate
	{
		gchar *uri;
	};

GType
atom_id_get_type (void)
{
	static GType atom_id_type = 0;

	if (!atom_id_type)
		{
			static const GTypeInfo atom_id_info =
			{
				sizeof (AtomIDClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_id_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomID),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_id_init,
				NULL
			};

			atom_id_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomID",
			                                       &atom_id_info, 0);
		}

	return atom_id_type;
}

static void
atom_id_class_init (AtomIDClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomIDPrivate));

	object_class->set_property = atom_id_set_property;
	object_class->get_property = atom_id_get_property;

	g_object_class_install_property (object_class, PROP_URI,
	                                 g_param_spec_string ("uri",
	                                                      "URI",
	                                                      "URI",
	                                                      "",
	                                                      G_PARAM_READWRITE));
}

static void
atom_id_init (AtomID *atom_id)
{
}

/**
 * atom_id_new:
 * @uri:
 *
 * Returns: the newly created #AtomID object.
 */
AtomID
*atom_id_new (const gchar *uri)
{
	gchar *uri_new = "";
	AtomID *atomid = ATOM_ID (g_object_new (atom_id_get_type (), NULL));

	g_object_set (G_OBJECT (atomid),
	              "uri", uri,
	              NULL);

	g_object_get (G_OBJECT (atomid),
	              "uri", &uri_new,
	              NULL);

	return (strcmp (uri_new, "") != 0 ? atomid : NULL);
}

/**
 * atom_id_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomID object.
 */
AtomID
*atom_id_new_from_xml (xmlNode *xnode)
{
	AtomID *atom_id = NULL;

	gchar *uri = (gchar *)xmlNodeGetContent ((xmlNodePtr)xnode);
	if (uri != NULL)
		{
			atom_id = atom_id_new (uri);
			if (atom_id != NULL)
				{
					atom_common_set_properties_from_xml (ATOM_COMMON (atom_id), xnode);
				}
		}

	return atom_id;
}

/**
 * atom_id_get_xml:
 * @atom_id: #AtomID object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_id_get_xml (AtomID *atom_id, xmlNode *xnode)
{
	AtomIDPrivate *priv = ATOM_ID_GET_PRIVATE (atom_id);

	atom_common_get_xml (ATOM_COMMON (atom_id), xnode);

	xmlNodeSetContent (xnode, (const xmlChar *)priv->uri);
}

static void
atom_id_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomID *atom_id = ATOM_ID (object);

	AtomIDPrivate *priv = ATOM_ID_GET_PRIVATE (atom_id);

	switch (property_id)
		{
			case PROP_URI:
				{
					gchar *uri = g_value_dup_string (value);
					if (strcmp (g_strstrip (uri), "") != 0)
						{
							priv->uri = g_strdup (uri);
						}
					else
						{
							g_warning ("Property uri must not be empty.");
						}
					g_free (uri);
					break;
				}

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_id_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomID *atom_id = ATOM_ID (object);

	AtomIDPrivate *priv = ATOM_ID_GET_PRIVATE (atom_id);

	switch (property_id)
		{
			case PROP_URI:
				g_value_set_string (value, priv->uri);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
