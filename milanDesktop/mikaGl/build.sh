#!/bin/bash
rm mikaGlTest.o
rm mikaGlTest
make -f Makefile agg.o
make -f Makefile libagg
make -f Makefile libjpeg
make -f Makefile libaggfreetype
make -f Makefile libfreetype
make -f Makefile mikaGl.o
make -f Makefile mikaGlImage.o
make -f Makefile mikaGlSurface.o
make -f Makefile mikaGlWindow.o
make -f Makefile mikaGlTest.o
rm mikaGl.a
ar rvs mikaGl.a \
	agg.o \
	agg_arc.o \
	agg_arrowhead.o \
	agg_bezier_arc.o \
	agg_bspline.o \
	agg_curves.o \
	agg_embedded_raster_fonts.o \
	agg_font_freetype.o \
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
	agg_vpgen_segmentator.o \
        basepic.o \
	ftadvanc.o \
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
	rastpic.o \
	mikaGl.o \
	mikaGlImage.o \
	mikaGlSurface.o \
        mikaGlWindow.o \
	truetype.o \
	ttdriver.o \
	ttgload.o \
	ttgxvar.o \
	ttinterp.o \
	ttobjs.o \
	ttpic.o \
	ttpload.o \
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
make -f Makefile mikaGlTest
