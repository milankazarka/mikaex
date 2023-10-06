/*
 * Copyright (C) 2005-2006 Andrea Zagli <azagli@inwind.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <time.h>

#include <gfeed.h>

int
main (int argc, char **argv)
{
	Atom *atom;
	AtomID *id;
	AtomText *title;
	AtomDate *updated;
	GList *authors = NULL,
	      *entries = NULL;
	AtomEntry *entry;
	AtomContent *content;
	time_t timet;
	struct tm tm;
	xmlDoc *xdoc;

	g_type_init ();

	id = atom_id_new ("http://foo.mydomain.com/");
	title = atom_text_new (ATOM_TEXT_XHTML, "dive into mark");
	g_object_set (G_OBJECT (title),
	              "lang", "it",
	              NULL);

	timet = time (NULL);

	gmtime_r ((const time_t *)&timet, (struct tm *)&tm);
	tm.tm_sec = 0;
	tm.tm_min = 0;
	tm.tm_hour = 0;

	updated = atom_date_new (tm);

	atom = atom_new (id, title, updated);

	authors = g_list_append (authors, (gpointer)atom_person_new ("Andrea Zagli"));
	g_object_set (G_OBJECT (authors->data),
	              "uri", "http://saettaz.altervista.org            ",
	              "email", "       azagli@inwind.it",
	              NULL);

	g_object_set (G_OBJECT (atom),
	              "author", authors,
	              NULL);

	content = atom_content_new (ATOM_CONTENT_XHTML);
	g_object_set (G_OBJECT (content),
	              "text", "content's text",
	              NULL);

	entries = g_list_append (entries, (gpointer)atom_entry_new (
	                                               atom_id_new ("http://foo.mydomain.com/e1.html"),
	                                               atom_text_new (ATOM_TEXT_TEXT, "the title"),
	                                               updated));
	g_object_set (G_OBJECT (entries->data),
	              "content", content,
	              NULL);

	entry = atom_entry_new (atom_id_new ("http://foo.mydomain.com/e2.html"),
	                        atom_text_new (ATOM_TEXT_TEXT, "the second entry"),
	                        updated);

	g_object_set (G_OBJECT (entry),
	              "content", content,
	              NULL);

	entries = g_list_append (entries, (gpointer)entry);

	g_object_set (G_OBJECT (atom),
	              "entry", entries,
	              NULL);

	xdoc = atom_get_xml_doc (atom);
	xmlSaveFormatFileEnc ("./test_atom.xml", xdoc, "iso-8859-1", 2);

	atom_save_file (atom, "./test_atom_gfeed.xml");

	return 0;
}
