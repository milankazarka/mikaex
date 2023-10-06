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

#include "atomentry.h"
#include "atomperson.h"
#include "atomcategory.h"
#include "atomcontent.h"
#include "atomlink.h"
#include "atomsource.h"

enum
{
	PROP_0,
	PROP_AUTHOR,
	PROP_CATEGORY,
	PROP_CONTENT,
	PROP_CONTRIBUTOR,
	PROP_ID,
	PROP_LINK,
	PROP_PUBLISHED,
	PROP_RIGHTS,
	PROP_SOURCE,
	PROP_SUMMARY,
	PROP_TITLE,
	PROP_UPDATED
};

static void atom_entry_class_init (AtomEntryClass *klass);
static void atom_entry_init (AtomEntry *atome);

static void atom_entry_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void atom_entry_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

#define ATOM_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_ENTRY, AtomEntryPrivate))

typedef struct _AtomEntryPrivate AtomEntryPrivate;
struct _AtomEntryPrivate
	{
		GList *author,
		      *category,
		      *contributor;
		AtomContent *content;
		AtomID *id;
		AtomLink *link;
		AtomText *title,
		         *rights,
		         *summary;
		AtomDate *published,
		         *updated;
		AtomSource *source;
	};

GType
atom_entry_get_type (void)
{
	static GType atom_entry_type = 0;

	if (!atom_entry_type)
		{
			static const GTypeInfo atom_entry_info =
			{
				sizeof (AtomEntryClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_entry_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomEntry),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_entry_init,
				NULL
			};

			atom_entry_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomEntry",
			                                          &atom_entry_info, 0);
		}

	return atom_entry_type;
}

static void
atom_entry_class_init (AtomEntryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomEntryPrivate));

	object_class->set_property = atom_entry_set_property;
	object_class->get_property = atom_entry_get_property;

	g_object_class_install_property (object_class, PROP_AUTHOR,
	                                 g_param_spec_pointer ("author",
	                                                       "Author",
	                                                       "An #AtomPerson object that indicates the author of the entry.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CATEGORY,
	                                 g_param_spec_pointer ("category",
	                                                       "Category",
	                                                       "Information about a category associated with an entry.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CONTENT,
	                                 g_param_spec_object ("content",
	                                                      "Content",
	                                                      "Either contains or links to the content of the entry.",
	                                                      TYPE_ATOM_CONTENT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CONTRIBUTOR,
	                                 g_param_spec_pointer ("contributor",
	                                                       "Contributor",
	                                                       "Indicates a person or other entity who contributed to the entry or feed.",
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_ID,
	                                 g_param_spec_object ("id",
	                                                      "Id",
	                                                      "A permanent, universally unique identifier for an entry.",
	                                                      TYPE_ATOM_ID,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LINK,
	                                 g_param_spec_object ("link",
	                                                      "Link",
	                                                      "An #AtomLink defines a reference from an entry or feed to a Web resource.",
	                                                      TYPE_ATOM_LINK,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_PUBLISHED,
	                                 g_param_spec_object ("published",
	                                                      "Published",
	                                                      "An #AtomDate object indicating an instant in time associated with an event early in the life cycle of the entry.",
	                                                      TYPE_ATOM_DATE,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_RIGHTS,
	                                 g_param_spec_object ("rights",
	                                                      "Rights",
	                                                      "Conveys information about rights held in and over an entry.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SOURCE,
	                                 g_param_spec_object ("source",
	                                                      "Source",
	                                                      "Source",
	                                                      TYPE_ATOM_SOURCE,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SUMMARY,
	                                 g_param_spec_object ("summary",
	                                                      "Summary",
	                                                      "Conveys a short summary, abstract, or excerpt of an entry.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TITLE,
	                                 g_param_spec_object ("title",
	                                                      "Title",
	                                                      "A human-readable title for the entry.",
	                                                      TYPE_ATOM_TEXT,
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_UPDATED,
	                                 g_param_spec_object ("updated",
	                                                      "Updated",
	                                                      "The most recent instant in time when the entry was modified in a way the publisher considers significant.",
	                                                      TYPE_ATOM_DATE,
	                                                      G_PARAM_READWRITE));
}

static void
atom_entry_init (AtomEntry *atome)
{
}

/**
 * atom_entry_new:
 * @id: a permanent, universally unique identifier for an entry or feed.
 * @title: a human-readable title for an entry or feed.
 * @updated: the most recent instant in time when an entry or feed was modified in a way the publisher considers significant.
 *
 * Returns: the newly created #AtomEntry object.
 */
AtomEntry
*atom_entry_new (AtomID *id, AtomText *title, AtomDate *updated)
{
	AtomEntry *atom_entry = ATOM_ENTRY (g_object_new (atom_entry_get_type (), NULL));

	g_object_set (G_OBJECT (atom_entry),
	              "id", id,
	              "title", title,
	              "updated", updated,
	              NULL);

	return atom_entry;
}

/**
 * atom_entry_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomEntry object.
 */
AtomEntry
*atom_entry_new_from_xml (xmlNode *xnode)
{
	AtomEntry *atom_entry = NULL;

	AtomID *id = NULL;
	AtomText *title = NULL;
	AtomDate *updated = NULL;
										
	xmlXPathObjectPtr xpresult;
	xmlXPathContextPtr xpcontext = xmlXPathNewContext (xnode->doc);

	xmlXPathRegisterNs (xpcontext, (const xmlChar *)"atom", (const xmlChar *)"http://www.w3.org/2005/Atom");

	/* searching for id */
	xpcontext->node = xnode;
	xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:id", xpcontext);
	if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
		{
			g_critical ("Atom entry must contain one and only id element.");
		}
	else
		{
			id = atom_id_new_from_xml (xpresult->nodesetval->nodeTab[0]);
			if (id == NULL)
				{
					g_critical ("Invalid id element.");
				}
		}

	/* searching for title */
	xpcontext->node = xnode;
	xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:title", xpcontext);
	if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
		{
			g_critical ("Atom entry must contain one and only title element.");
		}
	else
		{
			title = atom_text_new_from_xml (xpresult->nodesetval->nodeTab[0]);
			if (title == NULL)
				{
					g_critical ("Invalid title element.");
				}
		}

	/* searching for updated */
	xpcontext->node = xnode;
	xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:updated", xpcontext);
	if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
		{
			g_critical ("Atom entry must contain one and only updated element.");
		}
	else
		{
			updated = atom_date_new_from_xml (xpresult->nodesetval->nodeTab[0]);
			if (updated == NULL)
				{
					g_critical ("Invalid updated element.");
				}
		}

	if (id != NULL && title != NULL && updated != NULL)
		{
			atom_entry = atom_entry_new (id, title, updated);

			AtomEntryPrivate *priv = ATOM_ENTRY_GET_PRIVATE (atom_entry);

			atom_common_set_properties_from_xml (ATOM_COMMON (atom_entry), xnode);

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
					else if (xmlStrcmp (cur->name, (const xmlChar *)"content") == 0)
						{
							AtomContent *ac = atom_content_new_from_xml (cur);
							if (ac != NULL)
								{
									priv->content = ac;
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
					else if (xmlStrcmp (cur->name, (const xmlChar *)"id") == 0)
						{
							AtomID *id = atom_id_new_from_xml (cur);
							if (id == NULL)
								{
									priv->id = id;
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
					else if (xmlStrcmp (cur->name, (const xmlChar *)"published") == 0)
						{
							AtomDate *published = atom_date_new_from_xml (cur);
							if (published != NULL)
								{
									priv->published = published;
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
					else if (xmlStrcmp (cur->name, (const xmlChar *)"source") == 0)
						{
							AtomSource *source = atom_source_new_from_xml (cur);
							if (source != NULL)
								{
									priv->source = source;
								}
						}
					else if (xmlStrcmp (cur->name, (const xmlChar *)"summary") == 0)
						{
							AtomText *summary = atom_text_new_from_xml (cur);
							if (summary != NULL)
								{
									priv->summary = summary;
								}
						}
					else if (xmlStrcmp (cur->name, (const xmlChar *)"title") == 0)
						{
							AtomText *title = atom_text_new_from_xml (cur);
							if (title != NULL)
								{
									priv->title = title;
								}
						}
					else if (xmlStrcmp (cur->name, (const xmlChar *)"updated") == 0)
						{
							AtomDate *updated = atom_date_new_from_xml (cur);
							if (updated != NULL)
								{
									priv->updated = updated;
								}
						}

					cur = cur->next;
				}
		}

	return atom_entry;
}

/**
 * atatom_entry_get_xml:
 * @atom_entry: an #AtomEntry object.
 * @xnode: an #xmlNode.
 *
 */
void
atom_entry_get_xml (AtomEntry *atom_entry, xmlNode *xnode)
{
	xmlNode *xnodenew;
	GList *lst;

	AtomEntryPrivate *priv = ATOM_ENTRY_GET_PRIVATE (atom_entry);

	atom_common_get_xml (ATOM_COMMON (atom_entry), xnode);

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

	if (priv->content != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"content");
			xmlAddChild (xnode, xnodenew);
			atom_content_get_xml (priv->content, xnodenew);
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

	if (priv->published != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"published");
			xmlAddChild (xnode, xnodenew);
			atom_date_get_xml (priv->published, xnodenew);
		}

	if (priv->rights != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"rights");
			xmlAddChild (xnode, xnodenew);
			atom_text_get_xml (priv->rights, xnodenew);
		}

	if (priv->source != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"source");
			xmlAddChild (xnode, xnodenew);
			atom_source_get_xml (priv->source, xnodenew);
		}

	if (priv->summary != NULL)
		{
			xnodenew = xmlNewNode (NULL, (const xmlChar *)"summary");
			xmlAddChild (xnode, xnodenew);
			atom_text_get_xml (priv->summary, xnodenew);
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
atom_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GObject *obj;

	AtomEntry *atome = ATOM_ENTRY (object);

	AtomEntryPrivate *priv = ATOM_ENTRY_GET_PRIVATE (atome);

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

			case PROP_CONTENT:
				if (g_value_get_object (value) != NULL && IS_ATOM_CONTENT (g_value_get_object (value)))
					{
						priv->content = (AtomContent *)g_value_dup_object (value);
					}
				break;

			case PROP_CONTRIBUTOR:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->contributor = (GList *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (GList));
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

			case PROP_RIGHTS:
				if (g_value_get_object (value) != NULL && IS_ATOM_TEXT (g_value_get_object (value)))
					{
						priv->rights = (AtomText *)g_value_dup_object (value);
					}
				break;

			case PROP_SOURCE:
				if (g_value_get_object (value) != NULL && IS_ATOM_SOURCE (g_value_get_object (value)))
					{
						priv->source = (AtomSource *)g_value_dup_object (value);
					}
				break;

			case PROP_SUMMARY:
				if (g_value_get_object (value) != NULL && IS_ATOM_TEXT (g_value_get_object (value)))
					{
						priv->summary = (AtomText *)g_value_dup_object (value);
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
atom_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomEntry *atome = ATOM_ENTRY (object);

	AtomEntryPrivate *priv = ATOM_ENTRY_GET_PRIVATE (atome);
	
	switch (property_id)
		{
			case PROP_AUTHOR:
				g_value_set_pointer (value, priv->author);
				break;

			case PROP_CATEGORY:
				g_value_set_pointer (value, priv->category);
				break;

			case PROP_CONTENT:
				g_value_set_object (value, priv->content);
				break;

			case PROP_CONTRIBUTOR:
				g_value_set_pointer (value, priv->contributor);
				break;

			case PROP_ID:
				g_value_set_object (value, priv->id);
				break;

			case PROP_LINK:
				g_value_set_object (value, priv->link);
				break;

			case PROP_SOURCE:
				g_value_set_object (value, priv->source);
				break;

			case PROP_RIGHTS:
				g_value_set_object (value, priv->rights);
				break;

			case PROP_SUMMARY:
				g_value_set_object (value, priv->summary);
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
