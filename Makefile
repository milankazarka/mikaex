CC = gcc
CCO = g++

CFLAGS = -c -DUNIX -DGUI -DUSEX -DJPEG \
	-g -O2 \
	-DUSESSL -I/ssl/include/openssl -Lssl/lib \
	-Ifreetype/include \
	-I/usr/include \
	-Ifreetype/ft2_app/graph \
	-I/usr/local/lib/gtk-2.0/include -I/usr/local/include/atk-1.0 \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include \
	-I/usr/local/include/pixman-1 -I/usr/local/include/freetype2 \
	-I/usr/local/include -I/usr/local/include/webkit-1.0 \
	-I/usr/include/libpng12 \
	-I/opt/mika/include -L/opt/mika/lib \
	-I/opt/mika/gtk-2.0
	
LDFLAGS = -pthread \
	-lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 \
	-lgdk_pixbuf-2.0 -lpangocairo-1.0 -lcairo -lpango-1.0 \
	-lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 \
	-lpthread -lX11 -lXext \
	-lXrender -DGUI -DUSEX -DUNIX -DJPEG -lXpm \
	-DUSESSL -ltcl8.5 -ltk8.5 \
	-lz -lm -lcrypto -lcurl -lnxml
        
OBJECTS = \
	basic.o \
	config.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	custom.o \
	langSetup.o \
	wrappers.o \
	menu.o \
	mikaListbox.o \
	mikaList.o \
	mikaListItem.o \
	mikaListData.o \
	mikaPanel.o \
	mikaRss.o \
	mikaSkinEx.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        sys.o \
        xipc.o \
        backplane.o \
        player.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_NEWS = \
	basic.o \
	config.o \
	mikaSkinEx.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	news.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_SHOP = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	shop.o \
	mikaListbox.o \
	mikaSkinEx.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_SHOP = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	shop.o \
	mikaListbox.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_SUSHIMENU = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	sushimenu.o \
	mikaList.o \
	mikaListData.o \
	mikaListItem.o \
	mikaListbox.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_PHONE = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	phone.o \
	mikaList.o \
	mikaListData.o \
	mikaListItem.o \
	mikaListbox.o \
	mikaEvent.o \
	mikaEventReceiver.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_BUDDY = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	buddy.o \
	mikaList.o \
	mikaItem.o \
	mikaListData.o \
	mikaListItem.o \
	mikaListbox.o \
	mikaCanvas.o \
	mikaVideoWidget.o \
	mikaRotateWidget.o \
	mikaClockWidget.o \
	mikaEventReceiver.o \
	mikaEvent.o \
	mikaWidget.o \
	mikaWindow.o \
	mikaWidgetData.o \
	mikaSignageWidget.o \
	mikaGalleryWidget.o \
	mikaWidgetDelegate.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_GALLERY = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	gallery.o \
	mikaList.o \
	mikaItem.o \
	mikaListData.o \
	mikaListItem.o \
	mikaListbox.o \
	mikaCanvas.o \
	mikaVideoWidget.o \
	mikaRotateWidget.o \
	mikaClockWidget.o \
	mikaEventReceiver.o \
	mikaEvent.o \
	mikaWidget.o \
	mikaWindow.o \
	mikaWidgetData.o \
	mikaSignageWidget.o \
	mikaGalleryWidget.o \
	mikaWidgetDelegate.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_VIEWER = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	viewer.o \
	listener.o \
	mikaList.o \
	mikaItem.o \
	mikaListData.o \
	mikaListItem.o \
	mikaListbox.o \
	mikaCanvas.o \
	mikaVideoWidget.o \
	mikaRotateWidget.o \
	mikaClockWidget.o \
	mikaEventReceiver.o \
	mikaEvent.o \
	mikaWidget.o \
	mikaWindow.o \
	mikaWidgetData.o \
	mikaSignageWidget.o \
	mikaGalleryWidget.o \
	mikaWidgetDelegate.o \
	mikaPanel.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_BROWSER = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkinEx.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	browser.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_MIKARSSCLIENT = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	mikaRssClient.o \
	mikaListbox.o \
	mikaTextfield.o \
	mikaRss.o \
	mikaRssClass.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_ROBOTIC = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	robotic.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	mikaRoboticClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_CNAPP = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	cnApp.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_MIKAAPP = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	mikaListbox.o \
	mikaApp.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_COCKTAILS = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	cocktails.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_SUSHI = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	sushi.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_POS = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	posapp.o \
	mikaListbox.o \
	mikaList.o \
	mikaListItem.o \
	mikaListData.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_PRESENTATION = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	presentation.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_KITCHEN = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	posKitchen.o \
	kitchen.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_MUSEUM = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	posKitchen.o \
	museum.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_OLYMPIC = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	olympic.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o


OBJECTS_BACKGROUND = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	background.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_ADIDAS = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	adidas.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_WEB = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	xipc.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	web.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_FULLSCREEN = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	fullscreen.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        sys.o \
        custom.o \
        xipc.o \
        player.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_POSTER = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	poster.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        xipc.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_POSTER2 = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	poster2.o \
	mikaListbox.o \
	mikaRss.o \
	mikaPosClient.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        xipc.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_CONTROLLER = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaSkinEx.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootController.o \
	rootControllerWatchdog.o \
	rootControllerSync.o \
	rootSectors.o \
	rootStress.o \
	rootDraw.o \
	rootWm.o \
	rootSetup.o \
	rootWmInterface.o \
	rootEvents.o \
	rootModules.o \
	rootScheduller.o \
	rootPointer.o \
	rootActivity.o \
	mikaWatchdog.o \
	mikaRss.o \
	mikaPosClient.o \
	global.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        xipc.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_WIDGETAPI = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	xipc.o \
	agg.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_WM2A = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	wm2a.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	xipc.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	rotation.h \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

OBJECTS_WIDGET = \
	basic.o \
	config.o \
	mikaConfig.o \
	mikaSockets.o \
	mikaSerial.o \
	mikaEvent.o \
	mikaSkin.o \
	mikaIcon.o \
	api.o \
	layout.o \
	slidebar.o \
	app.o \
	os.o \
	activity.o \
	activityObject.o \
	rootControllerSync.o \
	global.o \
	interface.o \
        lowdraw.o \
        langSetup.o \
        player.o \
        sys.o \
        custom.o \
        backplane.o \
        messaging.o \
        system.o \
        systemip.o \
	rss.o \
	agg.o \
	xipc.o \
	canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o guirelay.o guimsg.o window.o wm.o \
	canvasDataModel.o \
	panel.o \
	listbox.o \
	networkSetup.o \
	setupParser.o \
	posSetup.o \
	connectionWatchdog.o \
	listboxItem.o \
	simple.o \
	gui.o \
	frame.o \
	guiEventLoop.o \
	guiMain.o \
	csv.o \
        guiBounds.o

SOURCES_LIBJPEG = \
        libJpeg/cdjpeg.c \
        libJpeg/ckconfig.c \
        libJpeg/djpeg.c \
        libJpeg/jaricom.c \
        libJpeg/jcapimin.c \
        libJpeg/jcapistd.c \
        libJpeg/jcarith.c \
        libJpeg/jccoefct.c \
        libJpeg/jccolor.c \
        libJpeg/jcdctmgr.c \
        libJpeg/jchuff.c \
        libJpeg/jcinit.c \
        libJpeg/jcmainct.c \
        libJpeg/jcmarker.c \
        libJpeg/jcmaster.c \
        libJpeg/jcomapi.c \
        libJpeg/jcparam.c \
        libJpeg/jcprepct.c \
        libJpeg/jcsample.c \
        libJpeg/jctrans.c \
        libJpeg/jdapimin.c \
        libJpeg/jdapistd.c \
        libJpeg/jdarith.c \
        libJpeg/jdatadst.c \
        libJpeg/jdatasrc.c \
        libJpeg/jdcoefct.c \
        libJpeg/jdcolor.c \
        libJpeg/jddctmgr.c \
        libJpeg/jdhuff.c \
        libJpeg/jdinput.c \
        libJpeg/jdmainct.c \
        libJpeg/jdmarker.c \
        libJpeg/jdmaster.c \
        libJpeg/jdmerge.c \
        libJpeg/jdpostct.c \
        libJpeg/jdsample.c \
        libJpeg/jdtrans.c \
        libJpeg/jerror.c \
        libJpeg/jfdctflt.c \
        libJpeg/jfdctfst.c \
        libJpeg/jfdctint.c \
        libJpeg/jidctflt.c \
        libJpeg/jidctfst.c \
        libJpeg/jidctint.c \
        libJpeg/jmemansi.c \
        libJpeg/jmemmgr.c \
        libJpeg/jmemnobs.c \
        libJpeg/jpegtran.c \
        libJpeg/jquant1.c \
        libJpeg/jquant2.c \
        libJpeg/jutils.c

OBJECTS_LIBJPEG = \
        jaricom.o \
        jcapimin.o \
        jcapistd.o \
        jcarith.o \
        jccoefct.o \
        jccolor.o \
        jcdctmgr.o \
        jchuff.o \
        jcinit.o \
        jcmainct.o \
        jcmarker.o \
        jcmaster.o \
        jcomapi.o \
        jcparam.o \
        jcprepct.o \
        jcsample.o \
        jctrans.o \
        jdapimin.o \
        jdapistd.o \
        jdarith.o \
        jdatadst.o \
        jdatasrc.o \
        jdcoefct.o \
        jdcolor.o \
        jddctmgr.o \
        jdhuff.o \
        jdinput.o \
        jdmainct.o \
        jdmarker.o \
        jdmaster.o \
        jdmerge.o \
        jdpostct.o \
        jdsample.o \
        jdtrans.o \
        jerror.o \
        jfdctflt.o \
        jfdctfst.o \
        jfdctint.o \
        jidctflt.o \
        jidctfst.o \
        jidctint.o \
        jmemansi.o \
        jmemmgr.o \
        jquant1.o \
        jquant2.o \
        jutils.o

SOURCES_LIBCAIRO = \
    libCairo/cairo-analysis-surface.c \
    libCairo/cairo-arc.c \
    libCairo/cairo-array.c \
    libCairo/cairo-atomic.c \
    libCairo/cairo-base85-stream.c \
    libCairo/cairo-bentley-ottmann.c \
    libCairo/cairo-cache.c \
    libCairo/cairo-cff-subset.c \
    libCairo/cairo-clip.c \
    libCairo/cairo-color.c \
    libCairo/cairo-debug.c \
    libCairo/cairo-deflate-stream.c \
    libCairo/cairo-directfb-surface.c \
    libCairo/cairo-fixed.c \
    libCairo/cairo-freelist.c \
    libCairo/cairo-glitz-surface.c \
    libCairo/cairo-gstate.c \
    libCairo/cairo-hash.c \
    libCairo/cairo-hull.c \
    libCairo/cairo-image-surface.c \
    libCairo/cairo-lzw.c \
    libCairo/cairo-matrix.c \
    libCairo/cairo-meta-surface.c \
    libCairo/cairo-misc.c \
    libCairo/cairo-mutex.c \
    libCairo/cairo-output-stream.c \
    libCairo/cairo-paginated-surface.c \
    libCairo/cairo-path-bounds.c \
    libCairo/cairo-path-fill.c \
    libCairo/cairo-path-fixed.c \
    libCairo/cairo-path-stroke.c \
    libCairo/cairo-path.c \
    libCairo/cairo-pattern.c \
    libCairo/cairo-pen.c \
    libCairo/cairo-png.c \
    libCairo/cairo-polygon.c \
    libCairo/cairo-rectangle.c \
    libCairo/cairo-region.c \
    libCairo/cairo-skiplist.c \
    libCairo/cairo-slope.c \
    libCairo/cairo-spline.c \
    libCairo/cairo-stroke-style.c \
    libCairo/cairo-surface-fallback.c \
    libCairo/cairo-surface.c \
    libCairo/cairo-system.c \
    libCairo/cairo-traps.c \
    libCairo/cairo-unicode.c \
    libCairo/cairo-version.c \
    libCairo/cairo-wideint.c \
    libCairo/cairo.c \
    libCairo/check-has-hidden-symbols.c \
    libCairo/check-link.c \
    libCairo/test-fallback-surface.c \
    libCairo/test-meta-surface.c \
    libCairo/test-paginated-surface.c

OBJECTS_LIBCAIRO = \
    cairo-analysis-surface.o \
    cairo-arc.o \
    cairo-array.o \
    cairo-atomic.o \
    cairo-base85-stream.o \
    cairo-bentley-ottmann.o \
    cairo-cache.o \
    cairo-cff-subset.o \
    cairo-clip.o \
    cairo-color.o \
    cairo-debug.o \
    cairo-deflate-stream.o \
    cairo-directfb-surface.o \
    cairo-fixed.o \
    cairo-freelist.o \
    cairo-glitz-surface.o \
    cairo-gstate.o \
    cairo-hash.o \
    cairo-hull.o \
    cairo-image-surface.o \
    cairo-lzw.o \
    cairo-matrix.o \
    cairo-meta-surface.o \
    cairo-misc.o \
    cairo-mutex.o \
    cairo-output-stream.o \
    cairo-paginated-surface.o \
    cairo-path-bounds.o \
    cairo-path-fill.o \
    cairo-path-fixed.o \
    cairo-path-stroke.o \
    cairo-path.o \
    cairo-pattern.o \
    cairo-pen.o \
    cairo-png.o \
    cairo-polygon.o \
    cairo-rectangle.o \
    cairo-region.o \
    cairo-skiplist.o \
    cairo-slope.o \
    cairo-spline.o \
    cairo-stroke-style.o \
    cairo-surface-fallback.o \
    cairo-surface.o \
    cairo-system.o \
    cairo-traps.o \
    cairo-unicode.o \
    cairo-version.o \
    cairo-wideint.o \
    cairo.o \
    check-has-hidden-symbols.o \
    check-link.o \
    test-fallback-surface.o \
    test-meta-surface.o \
    test-paginated-surface.o

SOURCES_LIBAGG = \
	libAgg/agg_arc.cpp \
	libAgg/agg_arrowhead.cpp \
	libAgg/agg_bezier_arc.cpp \
	libAgg/agg_bspline.cpp \
	libAgg/agg_curves.cpp \
	libAgg/agg_embedded_raster_fonts.cpp \
	libAgg/agg_gsv_text.cpp \
	libAgg/agg_image_filters.cpp \
	libAgg/agg_line_aa_basics.cpp \
	libAgg/agg_line_profile_aa.cpp \
	libAgg/agg_rounded_rect.cpp \
	libAgg/agg_sqrt_tables.cpp \
	libAgg/agg_trans_affine.cpp \
	libAgg/agg_trans_double_path.cpp \
	libAgg/agg_trans_single_path.cpp \
	libAgg/agg_trans_warp_magnifier.cpp \
	libAgg/agg_vcgen_bspline.cpp \
	libAgg/agg_vcgen_contour.cpp \
	libAgg/agg_vcgen_dash.cpp \
	libAgg/agg_vcgen_markers_term.cpp \
	libAgg/agg_vcgen_smooth_poly1.cpp \
	libAgg/agg_vcgen_stroke.cpp \
	libAgg/agg_vpgen_clip_polygon.cpp \
	libAgg/agg_vpgen_clip_polyline.cpp \
	libAgg/agg_vpgen_segmentator.cpp

SOURCES_LIBAGGFREETYPE = \
	libAggFreetype/agg_font_freetype.cpp \
	libAggFreetype/agg_font_freetype.h

OBJECTS_LIBAGGFREETYPE = \
	agg_font_freetype.o

SOURCES_LIBFREETYPE = \
	libFreetype/base/basepic.c \
	libFreetype/base/ftadvanc.c \
	libFreetype/base/ftapi.c \
	libFreetype/base/ftbase.c \
	libFreetype/base/ftbbox.c \
	libFreetype/base/ftbdf.c \
	libFreetype/base/ftbitmap.c \
	libFreetype/base/ftcalc.c \
	libFreetype/base/ftcid.c \
	libFreetype/base/ftdbgmem.c \
	libFreetype/base/ftdebug.c \
	libFreetype/base/ftfstype.c \
	libFreetype/base/ftgasp.c \
	libFreetype/base/ftgloadr.c \
	libFreetype/base/ftglyph.c \
	libFreetype/base/ftgxval.c \
	libFreetype/base/ftinit.c \
	libFreetype/base/ftlcdfil.c \
	libFreetype/base/ftmm.c \
	libFreetype/base/ftobjs.c \
	libFreetype/base/ftotval.c \
	libFreetype/base/ftoutln.c \
	libFreetype/base/ftpatent.c \
	libFreetype/base/ftpfr.c \
	libFreetype/base/ftpic.c \
	libFreetype/base/ftrfork.c \
	libFreetype/base/ftsnames.c \
	libFreetype/base/ftstream.c \
	libFreetype/base/ftstroke.c \
	libFreetype/base/ftsynth.c \
	libFreetype/base/ftsystem.c \
	libFreetype/base/fttrigon.c \
	libFreetype/base/fttype1.c \
	libFreetype/base/ftutil.c \
	libFreetype/base/ftwinfnt.c \
	libFreetype/truetype/truetype.c \
	libFreetype/truetype/ttdriver.c \
	libFreetype/truetype/ttgload.c \
	libFreetype/truetype/ttgxvar.c \
	libFreetype/truetype/ttinterp.c \
	libFreetype/truetype/ttobjs.c \
	libFreetype/truetype/ttpic.c \
	libFreetype/truetype/ttpload.c \
	libFreetype/raster/ftraster.c \
	libFreetype/raster/ftrend1.c \
	libFreetype/raster/raster.c \
	libFreetype/raster/rastpic.c \

OBJECTS_LIBFREETYPE = \
	basepic.o \
	ftadvanc.o \
	#ftbase.o \
	ftapi.o \
	ftbbox.o \
	ftbdf.o \
	ftbitmap.o \
	ftcalc.o \
	ftcid.o \
	ftdbgmem.o \
	ftdebug.o \
	ftfstype.o \
	ftgasp.o \
	ftgloadr.o \
	ftglyph.o \
	ftgxval.o \
	ftinit.o \
	ftlcdfil.o \
	ftmm.o \
	ftobjs.o \
	ftotval.o \
	ftoutln.o \
	ftpatent.o \
	ftpfr.o \
	ftpic.o \
	ftrfork.o \
	ftsnames.o \
	ftstream.o \
	ftstroke.o \
	ftsynth.o \
	ftsystem.o \
	fttrigon.o \
	fttype1.o \
	ftutil.o \
	ftwinfnt.o \
	truetype.o \
	ttdriver.o \
	ttgload.o \
	ttgxvar.o \
	ttinterp.o \
	ttobjs.o \
	ttpic.o \
	ttpload.o \
	ftraster.o \
	ftrend1.o \
	raster.o \
	rastpic.o

OBJECTS_LIBFREETYPE2 = \
	ftapi.o \
	ftbase.o \
	ftbbox.o \
	ftbdf.o \
	ftbitmap.o \
	ftcalc.o \
	ftcid.o \
	ftdbgmem.o \
	ftdebug.o \
	ftfstype.o \
	ftgasp.o \
	ftgloadr.o \
	ftglyph.o \
	ftgxval.o \
	ftinit.o \
	ftlcdfil.o \
	ftmm.o \
	ftotval.o \
	ftoutln.o \
	ftpatent.o \
	ftpfr.o \
	ftpic.o \
	ftrfork.o \
	ftsnames.o \
	ftstream.o \
	ftstroke.o \
	ftsynth.o \
	ftsystem.o \
	fttrigon.o \
	fttype1.o \
	ftutil.o \
	ftwinfnt.o \
	truetype.o \
	ttdriver.o \
	ttgload.o \
	ttgxvar.o \
	ttinterp.o \
	ttobjs.o \
	ttpic.o \
	ttpload.o \
	ftraster.o \
	ftrend1.o \
	raster.o \
	rastpic.o

OBJECTS_LIBAGG = \
	agg_arc.o \
	agg_arrowhead.o \
	agg_bezier_arc.o \
	agg_bspline.o \
	agg_curves.o \
	agg_embedded_raster_fonts.o \
	agg_gsv_text.o \
	agg_image_filters.o \
	agg_line_aa_basics.o \
	agg_line_profile_aa.o \
	agg_rounded_rect.o \
	agg_sqrt_tables.o \
	agg_trans_affine.o \
	agg_trans_double_path.o \
	agg_trans_single_path.o \
	agg_trans_warp_magnifier.o \
	agg_vcgen_bspline.o \
	agg_vcgen_contour.o \
	agg_vcgen_dash.o \
	agg_vcgen_markers_term.o \
	agg_vcgen_smooth_poly1.o \
	agg_vcgen_stroke.o \
	agg_vpgen_clip_polygon.o \
	agg_vpgen_clip_polyline.o \
	agg_vpgen_segmentator.o

SOURCES_LIBTINYXML = \
        libTinyXml/tinystr.cpp \
        libTinyXml/tinyxml.cpp \
        libTinyXml/tinyxmlerror.cpp \
        libTinyXml/tinyxmlparser.cpp

OBJECTS_LIBTINYXML = \
        tinystr.o \
        tinyxml.o \
        tinyxmlerror.o \
        tinyxmlparser.o

SOURCES_LIBMRSS = \
        libMrss/mrss_download.c \
        libMrss/mrss_edit.c \
        libMrss/mrss_free.c \
        libMrss/mrss_generic.c \
        libMrss/mrss_options.c \
        libMrss/mrss_parser.c \
        libMrss/mrss_search.c \
        libMrss/mrss_write.c

OBJECTS_LIBMRSS = \
        mrss_download.o \
        mrss_edit.o \
        mrss_free.o \
        mrss_generic.o \
        mrss_options.o \
        mrss_parser.o \
        mrss_search.o \
        mrss_write.o

OBJECTS_WMANAGER = wmanager.o

menu : $(OBJECTS)
	$(CCO) $(LDFLAGS) $(OBJECTS) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o menu

news : $(OBJECTS_NEWS)
	$(CCO) $(LDFLAGS) $(OBJECTS_NEWS) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o news

cnApp : $(OBJECTS_CNAPP)
	$(CCO) $(LDFLAGS) $(OBJECTS_CNAPP) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o cnApp

mikaApp : $(OBJECTS_MIKAAPP)
	$(CCO) $(LDFLAGS) $(OBJECTS_MIKAAPP) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o mikaApp

browser : $(OBJECTS_BROWSER)
	$(CCO) $(LDFLAGS) $(OBJECTS_BROWSER) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o browser

mikaRssClient : $(OBJECTS_MIKARSSCLIENT)
	$(CCO) $(LDFLAGS) $(OBJECTS_MIKARSSCLIENT) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o mikaRssClient

robotic : $(OBJECTS_ROBOTIC)
	$(CCO) $(LDFLAGS) $(OBJECTS_ROBOTIC) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o robotic

cocktails : $(OBJECTS_COCKTAILS)
	$(CCO) $(LDFLAGS) $(OBJECTS_COCKTAILS) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o cocktails

sushi : $(OBJECTS_SUSHI)
	$(CCO) $(LDFLAGS) $(OBJECTS_SUSHI) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o sushi

shop : $(OBJECTS_SHOP)
	$(CCO) $(LDFLAGS) $(OBJECTS_SHOP) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o shop

sushimenu : $(OBJECTS_SUSHIMENU)
	$(CCO) $(LDFLAGS) $(OBJECTS_SUSHIMENU) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o sushimenu

phone : $(OBJECTS_PHONE)
	$(CCO) $(LDFLAGS) $(OBJECTS_PHONE) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o phone

buddy : $(OBJECTS_BUDDY)
	$(CCO) $(LDFLAGS) $(OBJECTS_BUDDY) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o buddy

gallery : $(OBJECTS_GALLERY)
	$(CCO) $(LDFLAGS) $(OBJECTS_GALLERY) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o gallery

viewer : $(OBJECTS_VIEWER)
	$(CCO) $(LDFLAGS) $(OBJECTS_VIEWER) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o viewer

posapp : $(OBJECTS_POS)
	$(CCO) $(LDFLAGS) $(OBJECTS_POS) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o posapp

presentation : $(OBJECTS_PRESENTATION)
	$(CCO) $(LDFLAGS) $(OBJECTS_PRESENTATION) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o presentation

kitchen : $(OBJECTS_KITCHEN)
	$(CCO) $(LDFLAGS) $(OBJECTS_KITCHEN) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o kitchen

museum : $(OBJECTS_MUSEUM)
	$(CCO) $(LDFLAGS) $(OBJECTS_MUSEUM) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o museum

olympic : $(OBJECTS_OLYMPIC)
	$(CCO) $(LDFLAGS) $(OBJECTS_OLYMPIC) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o olympic

background : $(OBJECTS_BACKGROUND)
	$(CCO) $(LDFLAGS) $(OBJECTS_BACKGROUND) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o background

adidas : $(OBJECTS_ADIDAS)
	$(CCO) $(LDFLAGS) $(OBJECTS_ADIDAS) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o adidas

web : $(OBJECTS_WEB)
	$(CCO) $(LDFLAGS) $(OBJECTS_WEB) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o web

fullscreen : $(OBJECTS_FULLSCREEN)
	$(CCO) $(LDFLAGS) $(OBJECTS_FULLSCREEN) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o fullscreen

poster : $(OBJECTS_POSTER)
	$(CCO) $(LDFLAGS) $(OBJECTS_POSTER) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o poster

poster2 : $(OBJECTS_POSTER2)
	$(CCO) $(LDFLAGS) $(OBJECTS_POSTER2) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o poster2

rootController : $(OBJECTS)
	$(CCO) $(LDFLAGS) $(OBJECTS_CONTROLLER) $(OBJECTS_LIBAGG) $(OBJECTS_LIBMRSS) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o rootController

widgetapi : $(OBJECTS_WIDGETAPI)
	$(CCO) $(LDFLAGS) $(OBJECTS_WIDGETAPI) $(OBJECTS_LIBAGG) \
	$(OBJECTS_LIBFREETYPE) $(OBJECTS_LIBAGGFREETYPE) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o widgetapi

wm2a : $(OBJECTS_WM2A)
	$(CCO) $(LDFLAGS) $(OBJECTS_WM2A) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o wm2a

wmanager : $(OBJECTS_WMANAGER)
	$(CCO) $(LDFLAGS) $(OBJECTS_WMANAGER) $(OBJECTS_LIBTINYXML) \
        $(OBJECTS_LIBJPEG) -o wmanager

libjpeg :
	$(CCO) -c $(SOURCES_LIBJPEG) -IlibJpeg

libcairo:
	$(CC) -c $(SOURCES_LIBCAIRO) -IlibCairo -DCAIRO_NO_MUTEX=1

libagg :
	$(CCO) -c $(SOURCES_LIBAGG) -IlibAgg/include

libfreetype :
	$(CC) -c $(SOURCES_LIBFREETYPE) \
	-IlibFreetype/base \
	-IlibFreetype/truetype \
	-IlibFreetype/raster \
	-IlibFreetype/autofit \
	-IlibFreetype/include -IlibFreetype \
	-DFT_CONFIG_CONFIG_H="<ftconfig.h>" -DFT2_BUILD_LIBRARY \
	-DFT_CONFIG_MODULES_H="<ftmodule.h>" \
	-DFT_OPTION_AUTOFIT2 \
	-IlibFreetype/include/freetype/config

libaggfreetype :
	$(CCO) -c $(SOURCES_LIBAGGFREETYPE) -IlibAgg/include -IlibAggFreetype \
	-Ifreetype/include

libtinyxml :
	$(CCO) -c $(SOURCES_LIBTINYXML) -IlibTinyXml

libmrss :
	$(CC) -c $(SOURCES_LIBMRSS) -IlibMrss -DHAVE_CONFIG_H

mikaRss.o :
	$(CCO) $(CFLAGS) -c mikaRss.c

xipc.o :
	$(CCO) $(CFLAGS) -c xipc.c

mikaRssClass.o :
	$(CCO) $(CFLAGS) -c mikaRssClass.c

mikaPosClient.o :
	$(CCO) $(CFLAGS) -c mikaPosClient.c

posKitchen.o :
	$(CCO) $(CFLAGS) -c posKitchen.c

mikaSerial.o : mikaSerial.c
	$(CC) $(CFLAGS) -c mikaSerial.c

mikaEvent.o : mikaEvent.c
	$(CC) $(CFLAGS) -c mikaEvent.c

os.o : os.c
	$(CC) $(CFLAGS) -c os.c

activity.o : activity.c
	$(CC) $(CFLAGS) -c activity.c	

activityObject.o : activityObject.c
	$(CC) $(CFLAGS) -c activityObject.c
	
wmanager.o : wmanager.cpp
	$(CC) $(CFLAGS) -c wmanager.cpp

player.o : player.c
	$(CC) $(CFLAGS) -c player.c

menu.o : menu.c
	$(CCO) $(CFLAGS) -c menu.c

news.o : news.c
	$(CCO) $(CFLAGS) -c news.c

sushi.o : sushi.c
	$(CCO) $(CFLAGS) -c sushi.c

shop.o : shop.c
	$(CCO) $(CFLAGS) -c shop.c

posapp.o : posapp.c
	$(CCO) $(CFLAGS) -c posapp.c

presentation.o : presentation.c
	$(CCO) $(CFLAGS) -c presentation.c

olympic.o : olympic.c
	$(CCO) $(CFLAGS) -c olympic.c

menuSetup.o : menuSetup.c
	$(CCO) $(CFLAGS) -c menuSetup.c

sushimenu.o : sushimenu.c
	$(CCO) $(CFLAGS) -c sushimenu.c

phone.o : phone.c
	$(CCO) $(CFLAGS) -c phone.c

buddy.o : buddy.c
	$(CCO) $(CFLAGS) -c buddy.c

gallery.o : gallery.c
	$(CCO) $(CFLAGS) -c gallery.c

viewer.o : viewer.c
	$(CCO) $(CFLAGS) -c viewer.c

background.o : background.c
	$(CCO) $(CFLAGS) -c background.c

adidas.o : adidas.c
	$(CCO) $(CFLAGS) -c adidas.c

fullscreen.o : fullscreen.c
	$(CCO) $(CFLAGS) -c fullscreen.c

web.o : web.c
	$(CCO) $(CFLAGS) -c web.c

browser.o : browser.c
	$(CCO) $(CFLAGS) -c browser.c

mikaRssClient.o : mikaRssClient.c
	$(CCO) $(CFLAGS) -c mikaRssClient.c

robotic.o : robotic.c
	$(CCO) $(CFLAGS) -c robotic.c

cnApp.o : cnApp.c
	$(CCO) $(CFLAGS) -c cnApp.c

mikaApp.o : mikaApp.c
	$(CCO) $(CFLAGS) -c mikaApp.c

cocktails.o : cocktails.c
	$(CCO) $(CFLAGS) -c cocktails.c

poster.o : poster.c
	$(CCO) $(CFLAGS) -c poster.c

kitchen.o : kitchen.c
	$(CCO) $(CFLAGS) -c kitchen.c

museum.o : museum.c
	$(CCO) $(CFLAGS) -c museum.c

poster2.o : poster2.c
	$(CCO) $(CFLAGS) -c poster2.c

rootController.o : rootController.c
	$(CC) $(CFLAGS) -c rootController.c

rootWm.o : rootWm.c
	$(CCO) $(CFLAGS) -c rootWm.c

rootWmInterface.o : rootWmInterface.c
	$(CCO) $(CFLAGS) -c rootWmInterface.c

rootDraw.o : rootDraw.c
	$(CCO) $(CFLAGS) -c rootDraw.c

rootSetup.o : rootSetup.c
	$(CCO) $(CFLAGS) -c rootSetup.c

rootControllerSync.o : rootControllerSync.c
	$(CC) $(CFLAGS) -c rootControllerSync.c

rootSectors.o : rootSectors.c
	$(CC) $(CFLAGS) -c rootSectors.c

rootEvents.o : rootEvents.c
	$(CC) $(CFLAGS) -c rootEvents.c

rootModules.o : rootModules.c
	$(CC) $(CFLAGS) -c rootModules.c

rootStress.o : rootStress.c
	$(CC) $(CFLAGS) -c rootStress.c

rootScheduller.o : rootScheduller.c
	$(CC) $(CFLAGS) -c rootScheduller.c

rootPointer.o : rootPointer.c
	$(CC) $(CFLAGS) -c rootPointer.c

rootActivity.o : rootActivity.c
	$(CC) $(CFLAGS) -c rootActivity.c

wm2a.o : wm2a.c
	$(CC) $(CFLAGS) -c wm2a.c

lowdraw.o : lowdraw.c
	$(CC) $(CFLAGS) -c lowdraw.c

sys.o : sys.c
	$(CC) $(CFLAGS) -c sys.c

backplane.o : backplane.c
	$(CC) $(CFLAGS) -c backplane.c

messaging.o : messaging.c
	$(CCO) $(CFLAGS) -c messaging.c

system.o : system.c
	$(CC) $(CFLAGS) -c system.c

config.o : config.c
	$(CCO) $(CFLAGS) -c config.c

rss.o : rss.c
	$(CC) $(CFLAGS) -c rss.c

agg.o : agg.c
	$(CCO) $(CFLAGS) -c agg.c

api.o : api.cpp
	$(CCO) $(CFLAGS) -c api.cpp

canvas.o : canvas.c
	$(CC) $(CFLAGS) -c canvas.c

listbox.o : listbox.c
	$(CC) $(CFLAGS) -c listbox.c

networkSetup.o : networkSetup.c
	$(CC) $(CFLAGS) -c networkSetup.c

mikaListbox.o : mikaListbox.c
	$(CCO) $(CFLAGS) -c mikaListbox.c

mikaPanel.o : mikaPanel.c
	$(CCO) $(CFLAGS) -c mikaPanel.c

mikaTextfield.o : mikaTextfield.c
	$(CCO) $(CFLAGS) -c mikaTextfield.c

setupParser.o : setupParser.c
	$(CCO) $(CFLAGS) -c setupParser.c

posSetup.o : posSetup.c
	$(CCO) $(CFLAGS) -c posSetup.c

connectionWatchdog.o : connectionWatchdog.c
	$(CC) $(CFLAGS) -c connectionWatchdog.c

listboxItem.o : listboxItem.c
	$(CC) $(CFLAGS) -c listboxItem.c

keyboard.o : keyboard.c
	$(CC) $(CFLAGS) -c keyboard.c

text.o : text.c
	$(CC) $(CFLAGS) -c text.c

slidebar.o : slidebar.c
	$(CC) $(CFLAGS) -c slidebar.c

editable.o : editable.c
	$(CC) $(CFLAGS) -c editable.c

xmlsetup.o : xmlsetup.c
	$(CC) $(CFLAGS) -c xmlsetup.c

xml.o : xml.c
	$(CC) $(CFLAGS) -c xml.c

guirelay.o : guirelay.c
	$(CC) $(CFLAGS) -c guirelay.c

guimsg.o : guimsg.c
	$(CC) $(CFLAGS) -c guimsg.c

window.o : window.c
	$(CC) $(CFLAGS) -c window.c

wm.o : wm.c
	$(CC) $(CFLAGS) -c wm.c

wm.h.gch : wm.h
	$(CCO) -c wm.h
	
interface.o : interface.c
	$(CC) $(CFLAGS) -c interface.c	

panel.o : panel.c
	$(CC) $(CFLAGS) -c panel.c

custom.o : custom.c
	$(CC) $(CFLAGS) -c custom.c

simple.o : simple.c
	$(CC) $(CFLAGS) -c simple.c

systemip.o : systemip.c
	$(CC) $(CFLAGS) -c systemip.c

global.o : global.c
	$(CC) $(CFLAGS) -c global.c

gui.o : gui.c
	$(CC) $(CFLAGS) -c gui.c

app.o : app.c
	$(CC) $(CFLAGS) -c app.c

layout.o : layout.c
	$(CCO) $(CFLAGS) -c layout.c

listener.o : listener.c
	$(CCO) $(CFLAGS) -c listener.c

langSetup.o : langSetup.c
	$(CCO) $(CFLAGS) -c langSetup.c

frame.o : frame.c
	$(CC) $(CFLAGS) -c frame.c
	
guiEventLoop.o : guiEventLoop.c
	$(CC) $(CFLAGS) -c guiEventLoop.c

guiMain.o : guiMain.c
	$(CC) $(CFLAGS) -c guiMain.c

csv.o : csv.c
	$(CC) $(CFLAGS) -c csv.c

basic.o : basic.c
	$(CC) $(CFLAGS) -c basic.c

guiBounds.o : guiBounds.c
	$(CC) $(CFLAGS) -c guiBounds.c

xipc.o : xipc.c
	$(CC) $(CFLAGS) -c xipc.c

mikaHttp.o : mikaHttp.c
	$(CC) $(CFLAGS) -c mikaHttp.c

wrappers.o : wrappers.c
	$(CCO) $(CFLAGS) -c wrappers.c

mikaRoboticClient.o : mikaRoboticClient.c
	$(CCO) $(CFLAGS) -c mikaRoboticClient.c

mikaConfig.o : mikaConfig.c
	$(CCO) $(CFLAGS) -c mikaConfig.c

mikaSockets.o : mikaSockets.c
	$(CCO) $(CFLAGS) -c mikaSockets.c

mikaSkin.o : mikaSkin.c
	$(CCO) $(CFLAGS) -c mikaSkin.c

mikaSkinEx.o : mikaSkinEx.c
	$(CCO) $(CFLAGS) -c mikaSkinEx.c
	
mikaIcon.o : mikaIcon.c
	$(CCO) $(CFLAGS) -c mikaIcon.c

mikaEventReceiver.o : mikaEventReceiver.c
	$(CCO) $(CFLAGS) -c mikaEventReceiver.c

mikaEvent.o : mikaEvent.c
	$(CCO) $(CFLAGS) -c mikaEvent.c

mikaWidget.o : mikaWidget.c
	$(CCO) $(CFLAGS) -c mikaWidget.c

mikaWindow.o : mikaWindow.c
	$(CCO) $(CFLAGS) -c mikaWindow.c

mikaWidgetData.o : mikaWidgetData.c
	$(CCO) $(CFLAGS) -c mikaWidgetData.c

mikaSignageWidget.o : mikaSignageWidget.c
	$(CCO) $(CFLAGS) -c mikaSignageWidget.c

mikaGalleryWidget.o : mikaGalleryWidget.c
	$(CCO) $(CFLAGS) -c mikaGalleryWidget.c

mikaRotateWidget.o : mikaRotateWidget.c
	$(CCO) $(CFLAGS) -c mikaRotateWidget.c

mikaClockWidget.o : mikaClockWidget.c
	$(CCO) $(CFLAGS) -c mikaClockWidget.c

mikaCanvas.o : mikaCanvas.c
	$(CCO) $(CFLAGS) -c mikaCanvas.c

mikaVideoWidget.o : mikaVideoWidget.c
	$(CCO) $(CFLAGS) -c mikaVideoWidget.c

mikaItem.o : mikaItem.c
	$(CCO) $(CFLAGS) -c mikaItem.c

mikaWidgetDelegate.o : mikaWidgetDelegate.c
	$(CCO) $(CFLAGS) -c mikaWidgetDelegate.c

mikaList.o : mikaList.c
	$(CCO) $(CFLAGS) -c mikaList.c

mikaListItem.o : mikaListItem.c
	$(CCO) $(CFLAGS) -c mikaListItem.c

mikaListData.o : mikaListData.c
	$(CCO) $(CFLAGS) -c mikaListData.c

mikaWatchdog.o : mikaWatchdog.c
	$(CCO) $(CFLAGS) -c mikaWatchdog.c

rootControllerWatchdog.o : rootControllerWatchdog.c
	$(CCO) $(CFLAGS) -c rootControllerWatchdog.c
	