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

#include "atomdate.h"

enum
{
	PROP_0,
	PROP_DATE
};

static void atom_date_class_init (AtomDateClass *klass);
static void atom_date_init (AtomDate *atom_date);

static void atom_date_set_property (GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec);
static void atom_date_get_property (GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

#define ATOM_DATE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ATOM_DATE, AtomDatePrivate))

typedef struct _AtomDatePrivate AtomDatePrivate;
struct _AtomDatePrivate
	{
		struct tm *dateTime;
	};

GType
atom_date_get_type (void)
{
	static GType atom_date_type = 0;

	if (!atom_date_type)
		{
			static const GTypeInfo atom_date_info =
			{
				sizeof (AtomDateClass),
				NULL,	/* base_init */
				NULL,	/* base_finalize */
				(GClassInitFunc) atom_date_class_init,
				NULL,	/* class_finalize */
				NULL,	/* class_data */
				sizeof (AtomDate),
				0,	/* n_preallocs */
				(GInstanceInitFunc) atom_date_init,
				NULL
			};

			atom_date_type = g_type_register_static (TYPE_ATOM_COMMON, "AtomDate",
			                                         &atom_date_info, 0);
		}

	return atom_date_type;
}

static void
atom_date_class_init (AtomDateClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AtomDatePrivate));

	object_class->set_property = atom_date_set_property;
	object_class->get_property = atom_date_get_property;

	g_object_class_install_property (object_class, PROP_DATE,
	                                 g_param_spec_pointer ("date",
	                                                       "Date",
	                                                       "Date",
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
atom_date_init (AtomDate *atom_date)
{
}

/**
 * atom_date_new:
 * @dateTime: a tm struct.
 *
 * Returns: the newly created #AtomDate object.
 */
AtomDate
*atom_date_new (struct tm dateTime)
{
	AtomDate *atomdate = ATOM_DATE (g_object_new (atom_date_get_type (), NULL));

	g_object_set (G_OBJECT (atomdate),
	              "date", &dateTime,
	              NULL);

	return atomdate;
}

/**
 * atom_date_new_from_xml:
 * @xnode: an #xmlNode.
 *
 * Returns: the newly created #AtomDate object.
 */
AtomDate
*atom_date_new_from_xml (xmlNode *xnode)
{
	AtomDate *atom_date = NULL;

	gchar *date = (gchar *)xmlNodeGetContent ((xmlNodePtr)xnode);
	if (date == NULL)
		{
			g_critical ("Invalid date content.");
		}
	else
		{
			struct tm dateTime;

			gchar *ret = (gchar *)strptime (g_strstrip (date), "%Y-%m-%dT%H:%M:%SZ", &dateTime);
			if (ret != NULL)
				{
					atom_date = atom_date_new (dateTime);
					if (atom_date != NULL)
						{
							atom_common_set_properties_from_xml (ATOM_COMMON (atom_date), xnode);
						}
				}
		}

	return atom_date;
}

/**
 * atom_date_get_xml:
 * @atom_date: an #AtomDate object.
 * @xnode: the #xmlNode to fill.
 *
 */
void
atom_date_get_xml (AtomDate *atom_date, xmlNode *xnode)
{
	gchar *buf;

	AtomDatePrivate *priv = ATOM_DATE_GET_PRIVATE (atom_date);

	atom_common_get_xml (ATOM_COMMON (atom_date), xnode);

	buf = g_strdup_printf ("%04d-%02d-%02dT%02d:%02d:%02dZ",
	                       priv->dateTime->tm_year + 1900,
	                       priv->dateTime->tm_mon + 1,
	                       priv->dateTime->tm_mday,
	                       priv->dateTime->tm_hour,
	                       priv->dateTime->tm_min,
	                       priv->dateTime->tm_sec);

	xmlNodeSetContent (xnode, (const xmlChar *)buf);
}

static void
atom_date_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AtomDate *atom_date = ATOM_DATE (object);

	AtomDatePrivate *priv = ATOM_DATE_GET_PRIVATE (atom_date);

	switch (property_id)
		{
			case PROP_DATE:
				if (g_value_get_pointer (value) != NULL)
					{
						priv->dateTime = (struct tm *)g_memdup ((gconstpointer)g_value_get_pointer (value), sizeof (struct tm));
					}
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
atom_date_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AtomDate *atom_date = ATOM_DATE (object);

	AtomDatePrivate *priv = ATOM_DATE_GET_PRIVATE (atom_date);

	switch (property_id)
		{
			case PROP_DATE:
				g_value_set_pointer (value, priv->dateTime);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
