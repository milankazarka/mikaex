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
	Rss *rss;
	xmlDoc *xdoc;

	g_type_init ();

	rss = rss_new_from_file (argv[1]);

	if (rss != NULL)
		{
			xdoc = rss_get_xml_doc (rss);
			if (xdoc != NULL)
				{
					xmlSaveFormatFileEnc ("./test_load.xml", xdoc, "iso-8859-1", 2);
				}
			else
				{
					g_fprintf (stderr, "xdoc is null\n");
				}
			
			rss_save_file (rss, "./test_load_gfeed.xml");
		}
	else
		{
			g_fprintf (stderr, "rss is null\n");
		}

	return 0;
}
