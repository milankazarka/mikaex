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

#include "atomsource.h"
#include "atomperson.h"
#include "atomcategory.h"
#include "atomgenerator.h"
#include "atomlink.h"

enum
{
	PROP_0,
	PROP_AUTHOR,
	PROP_CATEGORY,
	PROP_CONTRIBUTOR,
	PROP_GENERATOR,
	PROP_ICON,
	PROP_ID,
	PROP_LINK,
	PROP_LOGO,
	PROP_RIGHTS,
	PROP_SUBTITLE,
	PROP_TITLE,
	PROP_UPDATED
};

static void atom_source_class_init (AtomSourceClass *klass);
static void atom_source_init (AtomSource *atome);

static void atom_source_set_property (GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec);
static void atom_source_get_property (GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

#define ATOM_SOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_SOURCE, AtomSourcePrivate))

typedef struct _AtomSourcePrivate AtomSourcePrivate;
struct _AtomSourcePrivate
	{
		GList *author,
		      *category,
		      *contributor;
		AtomGenerator *generator;
		AtomID *icon,
		       *id,
		       *logo;
		AtomLink *link;
		AtomText *title,
		         *rights,
		         *subtitle;
		AtomDate *updated;
	};

GType
atom_source_get_type (void)
{
	static GType atom_source_type = 0;

	if (!atom_source_type)
		{
			static const GTypeInfo atom_source_info =
			{
				sizeof (AtomSourceClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_source_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomSource),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_source_init,
				NULL
			};

			atom_source_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomSource",
			                                           &atom_source_info, 0);
		}

	return atom_source_type;
}

static void
atom_source_class_init (AtomSourceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomSourcePrivate));

	object_class->set_property = atom_source_set_property;
	object_class->get_property = atom_source_get_property;

	g_object_class_install_property (object_class, PROP_AUTHOR,
	                                 g_param_spec_pointer ("author",
	                                                       "Author",
	                                                       "A #GList of #AtomPerson objects that indicates the author of the entry or feed.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CATEGORY,
	                                 g_param_spec_pointer ("category",
	                                                       "Category",
	                                                       "A #GList of #AtomCategory objects convey information about a category associated with an entry or feed.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CONTRIBUTOR,
	                                 g_param_spec_pointer ("contributor",
	                                                       "Contributor",
	                                                       "A #GList of #AtomPerson objects that indicate a person or other entity who contributed to the entry or feed.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_GENERATOR,
	                                 g_param_spec_object ("generator",
	                                                      "Generato",
	                                                      "An #AtomGenerator object that identifies the agent used to generate a feed, for debugging and other purposes.",
	                                                      TYPE_ATOM_GENERATOR,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_ICON,
	                                 g_param_spec_object ("icon",
	                                                      "Icon",
	                                                      "An #AtomID object that contains an IRI reference [RFC3987] that identifies an image that provides iconic visual identification for a feed.",
	                                                      TYPE_ATOM_ID,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_ID,
	                                 g_param_spec_object ("id",
	                                                      "Id",
	                                                      "An #AtomID object that conveys a permanent, universally unique identifier for an entry or feed.",
	                                                      TYPE_ATOM_ID,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_object ("link",
	                                                      "Link",
	                                                      "An #AtomLink object that defines a reference from an entry or feed to a Web resource.",
	                                                      TYPE_ATOM_LINK,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LOGO,
	                                 g_param_spec_object ("logo",
	                                                      "Logo",
	                                                      "An #AtonID object that contains an IRI reference [RFC3987] that identifies an image that provides visual identification for a feed.",
	                                                      TYPE_ATOM_ID,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_RIGHTS,
	                                 g_param_spec_object ("rights",
	                                                      "Rights",
	                                                      "An #AtomText object that conveys information about rights held in and over an entry or feed.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SUBTITLE,
	                                 g_param_spec_object ("subtitle",
	                                                      "Subtitle",
	                                                      "An #AtomText object that conveys a human-readable description or subtitle for a feed.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_object ("title",
	                                                      "Title",
	                                                      "An #AtomText object that conveys a human-readable title for an entry or feed.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_UPDATED,
	                                 g_param_spec_object ("updated",
	                                                      "Updated",
	                                                      "An #AtomDate object indicating the most recent instant in time when an entry or feed was modified in a way the publisher considers significant.",
	                                                      TYPE_ATOM_DATE,
	                                                      G_PARAM_READWRITE));
}

static void
atom_source_init (AtomSource *atome)
{
}

/**
 * atom_source_new:
 *
 * Returns: the newly created #AtomSource object.
 */
AtomSource
*atom_source_new ()
{
	return ATOM_SOURCE (g_object_new (atom_source_get_type (), NULL));
}

/**
 * atom_source_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomSource object.
 */
AtomSource
*atom_source_new_from_xml (xmlNode *xnode)
{
	AtomSource *atom_source = atom_source_new ();

	AtomSourcePrivate *priv = ATOM_SOURCE_GET_PRIVATE (atom_source);

	atom_common_set_properties_from_xml (ATOM_COMMON (atom_source), xnode);

	xmlNode *cur = xnode->children;

	while (cur != NULL)
		{
			if (xmlStrcmp (cur->name, (const xmlChar *)"author") == 0)
				{
					AtomPerson *ap = atom_person_new_from_xml (cur);
					if (ap != NULL)
						{
							priv->author = g_list_append (priv->author, ap);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"category") == 0)
				{
					AtomCategory *ac = atom_category_new_from_xml (cur);
					if (ac != NULL)
						{
							priv->category = g_list_append (priv->category, ac);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"contributor") == 0)
				{
					AtomPerson *ap = atom_person_new_from_xml (cur);
					if (ap != NULL)
						{
							priv->contributor = g_list_append (priv->contributor, ap);
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"generator") == 0)
				{
					AtomGenerator *ag = atom_generator_new_from_xml (cur);
					if (ag != NULL)
						{
							priv->generator = ag;
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"icon") == 0)
				{
					AtomID *icon = atom_id_new_from_xml (cur);
					if (icon == NULL)
						{
							priv->icon = icon;
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"link") == 0)
				{
					AtomLink *link = atom_link_new_from_xml (cur);
					if (link == NULL)
						{
							priv->link = link;
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"logo") == 0)
				{
					AtomID *logo = atom_id_new_from_xml (cur);
					if (logo == NULL)
						{
							priv->logo = logo;
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"rights") == 0)
				{
					AtomText *rights = atom_text_new_from_xml (cur);
					if (rights != NULL)
						{
							priv->rights = rights;
						}
				}
			else if (xmlStrcmp (cur->name, (const xmlChar *)"subtitle") == 0)
				{
					AtomText *subtitle = atom_text_new_from_xml (cur);
					if (subtitle != NULL)
						{
							priv->subtitle = subtitle;
						}
				}

			cur = cur->next;
		}

	return atom_source;
}

/**
 * atatom_source_get_xml:
 * @atom_source: an #AtomSource object.
 *
 */
void
atom_source_get_xml (AtomSource *atom_source, xmlNode *xnode)
{
	xmlNode *xnodenew;
	GList *lst;

	AtomSourcePrivate *priv = ATOM_SOURCE_GET_PRIVATE (atom_source);

	atom_common_get_xml (ATOM_COMMON (atom_source), xnodenew);

	lst = g_list_first (priv->author);
	while (lst != NULL)
		{
			if (IS_ATOM_PERSON (lst->data))
				{
					xnodenew = xmlNewNode (NULL, (const xmlChar *)"author");
					xmlAddChild (xnode, xnodenew);
					atom_person_get_xml (ATOM_PERSON (lst->data), xnodenew);
				}

			lst = g_list_next (lst);
		}

	lst = g_list_first (priv->category);
	while (lst != NULL)
		{
			if (IS_ATOM_CATEGORY (lst->data))
				{
					xnodenew = xmlNewNode (NULL, (const xmlChar *)"category");
					xmlAddChild (xnode, xnodenew);
					atom_category_get_xml (ATOM_CATEGORY (lst->data), xnodenew);
				}

			lst = g_list_next (lst);
		}

	lst = g_list_first (priv->contributor);
	while (lst != NULL)
		{
			if (IS_ATOM_PERSON (lst->data))
				{
					xnodenew = xmlNewNode (NULL, (const xmlChar *)"contributor");
					xmlAddChild (xnode, xnodenew);
					atom_person_get_xml (ATOM_PERSON (lst->data), xnodenew);
				}

			lst = g_list_next (lst);
		}

	if (priv->generator != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"generator");
			xmlAddChild (xnode, xnodenew);
			atom_generator_get_xml (priv->generator, xnodenew);		
		}

	if (priv->icon != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"icon");
			xmlAddChild (xnode, xnodenew);
			atom_id_get_xml (priv->icon, xnodenew);
		}

	if (priv->id != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"id");
			xmlAddChild (xnode, xnodenew);
			atom_id_get_xml (priv->id, xnodenew);
		}

	if (priv->link != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"link");
			xmlAddChild (xnode, xnodenew);
			atom_link_get_xml (priv->link, xnodenew);
		}

	if (priv->logo != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"logo");
			xmlAddChild (xnode, xnodenew);
			atom_id_get_xml (priv->logo, xnodenew);
		}

	if (priv->rights != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"rights");
			xmlAddChild (xnode, xnodenew);
			atom_text_get_xml (priv->rights, xnodenew);
		}

	if (priv->subtitle != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"subtitle");
			xmlAddChild (xnode, xnodenew);
			atom_text_get_xml (priv->subtitle, xnodenew);
		}

	if (priv->title != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"title");
			xmlAddChild (xnode, xnodenew);
			atom_text_get_xml (priv->title, xnodenew);
		}

	if (priv->updated != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"updated");
			xmlAddChild (xnode, xnodenew);
			atom_date_get_xml (priv->updated, xnodenew);
		}
}

static void
atom_source_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GObject *obj;

	AtomSource *atome = ATOM_SOURCE (object);

	AtomSourcePrivate *priv = ATOM_SOURCE_GET_PRIVATE (atome);

	switch (property_id)
		{
			case PROP_AUTHOR:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->author = (GList *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (GList));
					}
				break;

			case PROP_CATEGORY:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->category = (GList *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (GList));
					}
				break;

			case PROP_CONTRIBUTOR:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->contributor = (GList *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (GList));
					}
				break;

			case PROP_GENERATOR:
				if (g_value_get_object (value) != NULL && IS_ATOM_GENERATOR (g_value_get_object (value)))
					{
						priv->generator = (AtomGenerator *)g_value_dup_object (value);
					}
				break;

			case PROP_ICON:
				if (g_value_get_object (value) != NULL && IS_ATOM_ID (g_value_get_object (value)))
					{
						priv->icon = (AtomID *)g_value_dup_object (value);
					}
				break;

			case PROP_ID:
				if (g_value_get_object (value) != NULL && IS_ATOM_ID (g_value_get_object (value)))
					{
						priv->id = (AtomID *)g_value_dup_object (value);
					}
				break;

			case PROP_LINK:
				if (g_value_get_object (value) != NULL && IS_ATOM_LINK (g_value_get_object (value)))
					{
						priv->link = (AtomLink *)g_value_dup_object (value);
					}
				break;

			case PROP_LOGO:
				if (g_value_get_object (value) != NULL && IS_ATOM_ID (g_value_get_object (value)))
					{
						priv->logo = (AtomID *)g_value_dup_object (value);
					}
				break;

			case PROP_RIGHTS:
				if (g_value_get_object (value) != NULL && IS_ATOM_TEXT (g_value_get_object (value)))
					{
						priv->rights = (AtomText *)g_value_dup_object (value);
					}
				break;

			case PROP_SUBTITLE:
				if (g_value_get_object (value) != NULL && IS_ATOM_TEXT (g_value_get_object (value)))
					{
						priv->subtitle = (AtomText *)g_value_dup_object (value);
					}
				break;

			case PROP_TITLE:
				if (g_value_get_object (value) != NULL && IS_ATOM_TEXT (g_value_get_object (value)))
					{
						priv->title = (AtomText *)g_value_dup_object (value);
					}
				break;

			case PROP_UPDATED:
				if (g_value_get_object (value) != NULL && IS_ATOM_DATE (g_value_get_object (value)))
					{
						priv->updated = (AtomDate *)g_value_dup_object (value);
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_source_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomSource *atome = ATOM_SOURCE (object);

	AtomSourcePrivate *priv = ATOM_SOURCE_GET_PRIVATE (atome);
	
	switch (property_id)
		{
			case PROP_AUTHOR:
				g_value_set_pointer (value, priv->author);
				break;

			case PROP_CATEGORY:
				g_value_set_pointer (value, priv->category);
				break;

			case PROP_CONTRIBUTOR:
				g_value_set_pointer (value, priv->contributor);
				break;

			case PROP_GENERATOR:
				g_value_set_pointer (value, priv->generator);
				break;

			case PROP_ICON:
				g_value_set_object (value, priv->icon);
				break;

			case PROP_ID:
				g_value_set_object (value, priv->id);
				break;

			case PROP_LINK:
				g_value_set_object (value, priv->link);
				break;

			case PROP_LOGO:
				g_value_set_object (value, priv->logo);
				break;

			case PROP_RIGHTS:
				g_value_set_object (value, priv->rights);
				break;

			case PROP_SUBTITLE:
				g_value_set_object (value, priv->subtitle);
				break;

			case PROP_TITLE:
				g_value_set_object (value, priv->title);
				break;

			case PROP_UPDATED:
				g_value_set_object (value, priv->updated);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
