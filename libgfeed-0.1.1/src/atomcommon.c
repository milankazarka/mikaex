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

#include "atomcommon.h"

enum
{
	PROP_0,
	PROP_BASE,
	PROP_LANG
};

static void atom_common_class_init (AtomCommonClass *klass);
static void atom_common_init (AtomCommon *atom_common);

static void atom_common_set_property (GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec);
static void atom_common_get_property (GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

#define ATOM_COMMON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_COMMON, AtomCommonPrivate))

typedef struct _AtomCommonPrivate AtomCommonPrivate;
struct _AtomCommonPrivate
	{
		gchar *base,
		      *lang;
	};

GType
atom_common_get_type (void)
{
	static GType atom_common_type = 0;

	if (!atom_common_type)
		{
			static const GTypeInfo atom_common_info =
			{
				sizeof (AtomCommonClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_common_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomCommon),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_common_init,
				NULL
			};

			atom_common_type = g_type_register_static (G_TYPE_OBJECT, "AtomCommon",
			                                           &atom_common_info, 0);
		}

	return atom_common_type;
}

static void
atom_common_class_init (AtomCommonClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomCommonPrivate));

	object_class->set_property = atom_common_set_property;
	object_class->get_property = atom_common_get_property;

	g_object_class_install_property (object_class, PROP_BASE,
	                                 g_param_spec_string ("base",
	                                                      "Base",
	                                                      "Base",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_LANG,
	                                 g_param_spec_string ("lang",
	                                                      "Lang",
	                                                      "Lang",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_common_init (AtomCommon *atom_common)
{
}

/**
 * atom_common_new:
 *
 * Returns: the newly created #AtomCommon object.
 */
AtomCommon
*atom_common_new ()
{
	return ATOM_COMMON (g_object_new (atom_common_get_type (), NULL));
}

/**
 * atom_common_set_properties_from_xml:
 * @atom_common: an #AtomCommon object.
 * #xnode: the #xmlNode from which to get properties.
 *
 */
void
atom_common_set_properties_from_xml (AtomCommon *atom_common, xmlNode *xnode)
{
	g_object_set (G_OBJECT (atom_common),
	              "base", xmlGetProp (xnode, (const xmlChar *)"base"),
	              "lang", xmlGetProp (xnode, (const xmlChar *)"lang"),
	              NULL);
}

/**
 * atom_common_get_xml:
 * @atom_common: an #AtomCommon object.
 * @xnode: the #xmlNode to fill.
 *
 */
void
atom_common_get_xml (AtomCommon *atom_common, xmlNode *xnode)
{
	AtomCommonPrivate *priv = ATOM_COMMON_GET_PRIVATE (atom_common);

	xmlChar *prop = (xmlChar *)g_strstrip (priv->base);
	if (!xmlStrEqual (prop, (const xmlChar *)""))
		{
			xmlSetProp (xnode, (const xmlChar *)"xml:base", prop);
		}

	prop = (xmlChar *)g_strstrip (priv->lang);
	if (!xmlStrEqual (prop, (const xmlChar *)""))
		{
			xmlSetProp (xnode, (const xmlChar *)"xml:lang", prop);
		}
}

static void
atom_common_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomCommon *atom_common = ATOM_COMMON (object);

	AtomCommonPrivate *priv = ATOM_COMMON_GET_PRIVATE (atom_common);

	switch (property_id)
		{
			case PROP_BASE:
				if (value != NULL)
					{
						gchar *str = g_value_dup_string (value);
						if (str == NULL)
							{
								priv->base = g_strdup ("");
							}
						else
							{
								priv->base = g_strstrip (str);
							}
					}
				break;

			case PROP_LANG:
				if (value != NULL)
					{
						gchar *str = g_value_dup_string (value);
						if (str == NULL)
							{
								priv->lang = g_strdup ("");
							}
						else
							{
								priv->lang = g_strstrip (str);
							}
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_common_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomCommon *atom_common = ATOM_COMMON (object);

	AtomCommonPrivate *priv = ATOM_COMMON_GET_PRIVATE (atom_common);

	switch (property_id)
		{
			case PROP_BASE:
				g_value_set_string (value, priv->base);
				break;

			case PROP_LANG:
				g_value_set_string (value, priv->lang);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
