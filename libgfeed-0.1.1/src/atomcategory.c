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

#include "atomcategory.h"

enum
{
	PROP_0,
	PROP_TERM,
	PROP_SCHEME,
	PROP_LABEL,
};

static void atom_category_class_init (AtomCategoryClass *klass);
static void atom_category_init (AtomCategory *atom_category);

static void atom_category_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_category_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_CATEGORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_CATEGORY, AtomCategoryPrivate))

typedef struct _AtomCategoryPrivate AtomCategoryPrivate;
struct _AtomCategoryPrivate
	{
		gchar *term,
		      *scheme,
		      *label;
	};

GType
atom_category_get_type (void)
{
	static GType atom_category_type = 0;

	if (!atom_category_type)
		{
			static const GTypeInfo atom_category_info =
			{
				sizeof (AtomCategoryClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_category_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomCategory),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_category_init,
				NULL
			};

			atom_category_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomCategory",
			                                             &atom_category_info, 0);
		}

	return atom_category_type;
}

static void
atom_category_class_init (AtomCategoryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomCategoryPrivate));

	object_class->set_property = atom_category_set_property;
	object_class->get_property = atom_category_get_property;

	g_object_class_install_property (object_class, PROP_TERM,
	                                 g_param_spec_string ("term",
	                                                      "Term",
	                                                      "A string that identifies the category to which the entry or feed belongs.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SCHEME,
	                                 g_param_spec_string ("scheme",
	                                                      "Scheme",
	                                                      "An IRI that identifies a categorization scheme.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (object_class, PROP_LABEL,
	                                 g_param_spec_string ("label",
	                                                      "Label",
	                                                      "A human-readable label for display in end-user applications.",
	                                                      "",
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_category_init (AtomCategory *atom_category)
{
}

/**
 * atom_category_new:
 * @term: a string that identifies the category to which the entry or feed belongs.
 *
 * Returns: the newly created #AtomCategory object.
 */
AtomCategory
*atom_category_new (const gchar *term)
{
	AtomCategory *atom_category = ATOM_CATEGORY (g_object_new (atom_category_get_type (), NULL));

	g_object_set (G_OBJECT (atom_category),
	              "term", term,
	              NULL);

	return atom_category;
}

/**
 * atom_category_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomCategory object.
 */
AtomCategory
*atom_category_new_from_xml (xmlNode *xnode)
{
	AtomCategory *atom_category = NULL;

	gchar *term = (gchar *)xmlGetProp (xnode, (const xmlChar *)"term");
	if (term == NULL || strcmp (g_strstrip (term), "") == 0)
		{
			g_critical ("AtomCategory must have a term attribute.");
			return NULL;
		}
	else
		{
			atom_category = atom_category_new (term);
			if (atom_category != NULL)
				{
					atom_common_set_properties_from_xml (ATOM_COMMON (atom_category), xnode);

					term = (gchar *)xmlGetProp (xnode, (const xmlChar *)"scheme");
					if (term != NULL && strcmp (g_strstrip (term), "") != 0)
						{
							g_object_set (G_OBJECT (atom_category),
							              "scheme", term,
							              NULL);
						}

					term = (gchar *)xmlGetProp (xnode, (const xmlChar *)"label");
					if (term != NULL && strcmp (g_strstrip (term), "") != 0)
						{
							g_object_set (G_OBJECT (atom_category),
							              "label", term,
							              NULL);
						}
				}
		}

	return atom_category;
}

/**
 * atom_category_get_xml:
 * @atom_category: an #AtomCategory object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_category_get_xml (AtomCategory *atom_category, xmlNode *xnode)
{
	AtomCategoryPrivate *priv = ATOM_CATEGORY_GET_PRIVATE (atom_category);

	atom_common_get_xml (ATOM_COMMON (atom_category), xnode);

	xmlSetProp (xnode, (const xmlChar *)"term", (const xmlChar *)priv->term);

	if (strcmp (priv->scheme, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"scheme", (const xmlChar *)priv->scheme);
		}
	if (strcmp (priv->label, "") != 0)
		{
			xmlSetProp (xnode, (const xmlChar *)"label", (const xmlChar *)priv->label);
		}
}

static void
atom_category_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomCategory *atom_category = ATOM_CATEGORY (object);

	AtomCategoryPrivate *priv = ATOM_CATEGORY_GET_PRIVATE (atom_category);

	switch (property_id)
		{
			case PROP_TERM:
				{
					gchar *term = g_strstrip (g_value_dup_string (value));
					if (strcmp (term, "") == 0)
						{
							g_warning ("Property term must not be empty.");
						}
					else
						{
							priv->term = term;
						}
					break;
				}

			case PROP_SCHEME:
				priv->scheme = g_strstrip (g_value_dup_string (value));
				break;

			case PROP_LABEL:
				priv->label = g_strstrip (g_value_dup_string (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_category_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomCategory *atom_category = ATOM_CATEGORY (object);

	AtomCategoryPrivate *priv = ATOM_CATEGORY_GET_PRIVATE (atom_category);

	switch (property_id)
		{
			case PROP_TERM:
				g_value_set_string (value, priv->term);
				break;

			case PROP_SCHEME:
				g_value_set_string (value, priv->scheme);
				break;

			case PROP_LABEL:
				g_value_set_string (value, priv->label);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
