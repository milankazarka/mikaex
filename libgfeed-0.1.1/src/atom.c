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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <libxml/xpath.h>

#include "atom.h"
#include "atomperson.h"
#include "atomcategory.h"
#include "atomgenerator.h"
#include "atomlink.h"
#include "atomentry.h"

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
	PROP_UPDATED,
	PROP_ENTRY
};

static void atom_class_init (AtomClass *klass);
static void atom_init (Atom *atom);

static void atom_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void atom_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static Atom *parse_xml (xmlDoc *xdoc);


#define ATOM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM, AtomPrivate))

typedef struct _AtomPrivate AtomPrivate;
struct _AtomPrivate
	{
		GList *author,
		      *category,
		      *contributor,
		      *entry;
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
atom_get_type (void)
{
	static GType atom_type = 0;

	if (!atom_type)
		{
			static const GTypeInfo atom_info =
			{
				sizeof (AtomClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (Atom),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_init,
				NULL
			};

			atom_type = g_type_register_static (TYPE_ATOM_COMMON, "Atom",
			                                    &atom_info, 0);
		}

	return atom_type;
}

static void
atom_class_init (AtomClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomPrivate));

	object_class->set_property = atom_set_property;
	object_class->get_property = atom_get_property;

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

	g_object_class_install_property (object_class, PROP_ENTRY,
	                                 g_param_spec_pointer ("entry",
	                                                       "Entry",
	                                                       "A #GList of #AtomEntry.",
	                                                       G_PARAM_READWRITE));
}

static void
atom_init (Atom *atom)
{
}

/**
 * atom_new:
 * @id: an #AtomID object that conveys a permanent, universally unique identifier for an entry or feed.
 * @title: an #AtomText object that conveys a human-readable title for an entry or feed.
 * @updated: an #AtomDate object indicating the most recent instant in time when an entry or feed was modified in a way the publisher considers significant.
 *
 * Returns: the newly created #Atom object.
 */
Atom
*atom_new (AtomID *id, AtomText *title, AtomDate *updated)
{
	Atom *atom = ATOM (g_object_new (atom_get_type (), NULL));

	g_object_set (G_OBJECT (atom),
	              "id", id,
	              "title", title,
	              "updated", updated,
	              NULL);

	return atom;
}

/**
 * atom_new_from_file:
 * @filename:
 *
 * Returns: the newly created #Atom object.
 */
Atom
*atom_new_from_file (const gchar *filename)
{
	Atom *atom = NULL;

	xmlDoc *xdoc = xmlParseFile (filename);

	atom = parse_xml (xdoc);

	return atom;
}

/**
 * atom_new_from_buffer:
 * @buffer: a pointer to an array of #gchar.
 *
 * Returns: the newly created #Atom object.
 */
Atom
*atom_new_from_buffer (const gchar *buffer)
{
	Atom *atom = NULL;

	xmlDoc *xdoc = xmlParseDoc (buffer);

	atom = parse_xml (xdoc);

	return atom;
}

/**
 * atom_get_xml_doc:
 * @atom: an #Atom object.
 *
 * Returns: the #xmlDoc correspondent to the entire #Atom object.
 */
xmlDoc
*atom_get_xml_doc (Atom *atom)
{
	xmlNode *xnode;
	GList *lst;

	AtomPrivate *priv = ATOM_GET_PRIVATE (atom);

	xmlDoc *xdoc = xmlNewDoc ((const xmlChar *)"1.0");
	xmlNode *xroot = xmlNewNode (NULL, (const xmlChar *)"feed");

  /* adding root element */
  if (xroot == NULL) return NULL;

  xmlSetProp (xroot, (const xmlChar *)"xmlns", (const xmlChar *)"http://www.w3.org/2005/Atom");
  xmlDocSetRootElement (xdoc, xroot);

	lst = g_list_first (priv->author);
	while (lst != NULL)
		{
			if (IS_ATOM_PERSON (lst->data))
				{
					xnode = xmlNewNode (NULL, (const xmlChar *)"author");
					xmlAddChild (xroot, xnode);
					atom_person_get_xml (ATOM_PERSON (lst->data), xnode);
				}

			lst = g_list_next (lst);
		}

	lst = g_list_first (priv->category);
	while (lst != NULL)
		{
			if (IS_ATOM_CATEGORY (lst->data))
				{
					xnode = xmlNewNode (NULL, (const xmlChar *)"category");
					xmlAddChild (xroot, xnode);
					atom_category_get_xml (ATOM_CATEGORY (lst->data), xnode);
				}

			lst = g_list_next (lst);
		}

	lst = g_list_first (priv->contributor);
	while (lst != NULL)
		{
			if (IS_ATOM_PERSON (lst->data))
				{
					xnode = xmlNewNode (NULL, (const xmlChar *)"contributor");
					xmlAddChild (xroot, xnode);
					atom_person_get_xml (ATOM_PERSON (lst->data), xnode);
				}

			lst = g_list_next (lst);
		}

	if (priv->generator != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"generator");
			xmlAddChild (xroot, xnode);
			atom_generator_get_xml (priv->generator, xnode);		
		}

	if (priv->icon != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"icon");
			xmlAddChild (xroot, xnode);
			atom_id_get_xml (priv->icon, xnode);
		}

	if (priv->id != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"id");
			xmlAddChild (xroot, xnode);
			atom_id_get_xml (priv->id, xnode);
		}
	else
		{
			/* TO DO */
			return NULL;
		}

	if (priv->link != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"link");
			xmlAddChild (xroot, xnode);
			atom_link_get_xml (priv->link, xnode);
		}

	if (priv->logo != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"logo");
			xmlAddChild (xroot, xnode);
			atom_id_get_xml (priv->logo, xnode);
		}

	if (priv->rights != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"rights");
			xmlAddChild (xroot, xnode);
			atom_text_get_xml (priv->rights, xnode);
		}

	if (priv->subtitle != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"subtitle");
			xmlAddChild (xroot, xnode);
			atom_text_get_xml (priv->subtitle, xnode);
		}

	if (priv->title != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"title");
			xmlAddChild (xroot, xnode);
			atom_text_get_xml (priv->title, xnode);
		}
	else
		{
			/* TO DO */
			return NULL;
		}

	if (priv->updated != NULL)
		{
			xnode = xmlNewNode (NULL, (const xmlChar *)"updated");
			xmlAddChild (xroot, xnode);
			atom_date_get_xml (priv->updated, xnode);
		}
	else
		{
			/* TO DO */
			return NULL;
		}

	lst = g_list_first (priv->entry);
	while (lst != NULL)
		{
			if (IS_ATOM_ENTRY (lst->data))
				{
					xnode = xmlNewNode (NULL, (const xmlChar *)"entry");
					xmlAddChild (xroot, xnode);
					atom_entry_get_xml (ATOM_ENTRY (lst->data), xnode);
				}

			lst = g_list_next (lst);
		}

	return xdoc;
}

/**
 * atom_save_file:
 * @atom: an #Atom object.
 * @filename:
 *
 */
gboolean
atom_save_file (Atom *atom, const gchar *filename)
{
	return (xmlSaveFileEnc (filename, atom_get_xml_doc (atom), "utf-8") > -1);
}

static void
atom_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GObject *obj;

	Atom *atom = ATOM (object);

	AtomPrivate *priv = ATOM_GET_PRIVATE (atom);

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

			case PROP_ENTRY:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->entry = (GList *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (GList));
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Atom *atom = ATOM (object);

	AtomPrivate *priv = ATOM_GET_PRIVATE (atom);
	
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

			case PROP_ENTRY:
				g_value_set_object (value, priv->entry);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static Atom
*parse_xml (xmlDoc *xdoc)
{
	Atom *atom;

	if (xdoc != NULL)
		{
			xmlNode *cur = xmlDocGetRootElement (xdoc);
			if (cur != NULL)
				{
					if (xmlStrcmp (cur->name, (const xmlChar *)"feed") == 0)
						{
							xmlNsPtr *xmlns = xmlGetNsList (xdoc, cur);
							if (xmlns != NULL && xmlStrcmp (xmlns[0]->href, (const xmlChar *)"http://www.w3.org/2005/Atom") == 0)
								{
									if (cur->children != NULL)
										{
											AtomID *id = NULL;
											AtomText *title = NULL;
											AtomDate *updated = NULL;
										
											xmlXPathObjectPtr xpresult;
											xmlXPathContextPtr xpcontext = xmlXPathNewContext (xdoc);

											xmlXPathRegisterNs (xpcontext, (const xmlChar *)"atom", xmlns[0]->href);

											/* searching for id */
											xpcontext->node = cur;
											xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:id", xpcontext);
											if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
												{
													g_critical ("Atom document must contain one and only id element.");
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
											xpcontext->node = cur;
											xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:title", xpcontext);
											if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
												{
													g_critical ("Atom document must contain one and only title element.");
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
											xpcontext->node = cur;
											xpresult = xmlXPathEvalExpression ((const xmlChar *)"child::atom:updated", xpcontext);
											if (xpresult == NULL || xpresult->nodesetval == NULL || xpresult->nodesetval->nodeNr != 1)
												{
													g_critical ("Atom document must contain one and only updated element.");
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
													atom = atom_new (id, title, updated);

													AtomPrivate *priv = ATOM_GET_PRIVATE (atom);

													cur = cur->children;
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
															else if (xmlStrcmp (cur->name, (const xmlChar *)"entry") == 0)
																{
																	AtomEntry *ae = atom_entry_new_from_xml (cur);
																	if (ae != NULL)
																		{
																			priv->entry = g_list_append (priv->entry, ae);
																		}
																}

															cur = cur->next;
														}
												}
										}
									else
										{
											g_critical ("Empty atom document.");
										}
								}
							else
								{
									g_critical ("Wrong atom version.");
								}
						}
					else
						{
							g_critical ("XML document is not an atom feed document.");
						}
				}
			else
				{
					g_critical ("Empty document.");
				}
		}

	return atom;
}
