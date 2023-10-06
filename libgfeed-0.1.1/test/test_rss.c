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
	RssChannel *rssc;
	RssCategory *rssca;
	RssCloud *rsscl;
	RssImage *rssi;
	RssTextInput *rssti;
	RssChannelItem *rssci;
	time_t timet;
	struct tm pubDate, lastBuildDate;
	xmlDoc *xdoc;
	guint skipDays, skipHours;

	g_type_init ();

	rss = rss_new ();

	rssc = rss_channel_new ("Channel",
	                        "http://www.channels.org/link1.html",
	                        "First channel - Description");

	if (rssc == NULL) return 0;

	g_object_set (G_OBJECT (rss),
	              "channel", rssc,
	              NULL);

	rssca = rss_category_new ("Comics");
	rss_channel_add_category (rssc, rssca);
	
	rssca = rss_category_new ("Horror");
	g_object_set (G_OBJECT (rssca),
	              "domain", "the domain",
	              NULL);
	rss_channel_add_category (rssc, rssca);

	timet = time (NULL);

	gmtime_r ((const time_t *)&timet, (struct tm *)&pubDate);
	pubDate.tm_sec = 0;
	pubDate.tm_min = 0;
	pubDate.tm_hour = 0;

	gmtime_r ((const time_t *)&timet, (struct tm *)&lastBuildDate);

	rsscl = rss_cloud_new ("radio.xmlstoragesystem.com", 80, "/RPC2", "xmlStorageSystem.rssPleaseNotify", "xml-rpc");
	
	rssi = rss_image_new ("http://www.image.com/01.png", "the title", "http://www.channels.org/link1.html");

	rssti = rss_text_input_new ("The text input's title", "a short description", "ti1", "http://www.google.com");

	skipDays = RSSCHANNEL_DAY_MONDAY | RSSCHANNEL_DAY_TUESDAY | RSSCHANNEL_DAY_WEDNESDAY |
	           RSSCHANNEL_DAY_THURSDAY | RSSCHANNEL_DAY_FRIDAY | RSSCHANNEL_DAY_SATURDAY | RSSCHANNEL_DAY_SUNDAY;

	skipHours = RSSCHANNEL_HOUR_23;
	
	g_object_set (G_OBJECT (rssc),
	              "language", "en",
	              "copyright", "mine",
	              "pub-date", &pubDate,
	              "last-build-date", &lastBuildDate,
	              "cloud", rsscl,
	              "image", rssi,
	              "text-input", rssti,
	              "skip-days", skipDays,
								"skip-hours", skipHours,
	              NULL);

	rssci = rss_channel_item_new ("Title item 1", "with this description");

	rssca = rss_category_new ("Sci-Fi");
	g_object_set (G_OBJECT (rssca),
	              "domain", "my domain",
	              NULL);
	rss_channel_item_add_category (rssci, rssca);

	g_object_set (G_OBJECT (rssci),
	              "author", "author@myhouse.it",
	              "comments", "http://www.google.com",
	              NULL);

	rss_channel_add_item (rssc, rssci);

	xdoc = rss_get_xml_doc (rss);
	xmlSaveFormatFileEnc ("./test.xml", xdoc, "iso-8859-1",2);

	rss_save_file (rss, "./test_gfeed.xml");

	return 0;
}
