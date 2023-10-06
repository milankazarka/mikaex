<html><head><title>Anti-Grain Geometry - </title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<link rel="stylesheet" type="text/css" href="image1_files/agg.css">
</head><body><table width="640px" border="0" cellpadding="0" cellspacing="0"><tbody><tr><td><pre><span class="kw2">#include</span> <span class="op">&lt;</span>stdio<span class="op">.</span>h<span class="op">&gt;</span>
<span class="kw2">#include</span> <span class="op">&lt;</span>stdlib<span class="op">.</span>h<span class="op">&gt;</span>
<span class="kw2">#include</span> <span class="op">&lt;</span>math<span class="op">.</span>h<span class="op">&gt;</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_rendering_buffer.h.html">agg_rendering_buffer.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_rasterizer_scanline_aa.h.html">agg_rasterizer_scanline_aa.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_ellipse.h.html">agg_ellipse.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html">agg_trans_affine.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_conv_transform.h.html">agg_conv_transform.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgb.h.html">agg_span_image_filter_rgb.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgba.h.html">agg_span_image_filter_rgba.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_gray.h.html">agg_span_image_filter_gray.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_pixfmt_rgba.h.html">agg_pixfmt_rgba.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_scanline_u.h.html">agg_scanline_u.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_renderer_scanline.h.html">agg_renderer_scanline.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_span_allocator.h.html">agg_span_allocator.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html">agg_span_interpolator_linear.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"<a href="http://www.antigrain.com/__code/include/agg_image_accessors.h.html">agg_image_accessors.h</a>"</span>
<span class="kw2">#include</span> <span class="str">"ctrl/agg_slider_ctrl.h"</span>
<span class="kw2">#include</span> <span class="str">"platform/agg_platform_support.h"</span>

<span class="kw1">enum</span> flip_y_e <span class="op">{</span> flip_y <span class="op">=</span> <span class="kw1">true</span> <span class="op">}</span><span class="op">;</span>

<span class="kw2">#define</span> AGG_BGR24
<span class="kw2">#include</span> <span class="str">"pixel_formats.h"</span>


<span class="kw1">class</span> the_application : <span class="kw1">public</span> agg::platform_support
<span class="op">{</span>
    agg::slider_ctrl<span class="op">&lt;</span>agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba8">rgba8</a><span class="op">&gt;</span> m_angle<span class="op">;</span>
    agg::slider_ctrl<span class="op">&lt;</span>agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba8">rgba8</a><span class="op">&gt;</span> m_scale<span class="op">;</span>

<span class="kw1">public</span>:
    the_application<span class="op">(</span>agg::pix_format_e format<span class="op">,</span> <span class="kw1">bool</span> flip_y<span class="op">)</span> :
        agg::platform_support<span class="op">(</span>format<span class="op">,</span> flip_y<span class="op">)</span><span class="op">,</span>
        m_angle<span class="op">(</span><span class="num">5</span><span class="op">,</span>  <span class="num">5</span><span class="op">,</span>    <span class="num">300</span><span class="op">,</span> <span class="num">12</span><span class="op">,</span>    <span class="op">!</span>flip_y<span class="op">)</span><span class="op">,</span>
        m_scale<span class="op">(</span><span class="num">5</span><span class="op">,</span>  <span class="num">5</span><span class="op">+</span><span class="num">15</span><span class="op">,</span> <span class="num">300</span><span class="op">,</span> <span class="num">12</span><span class="op">+</span><span class="num">15</span><span class="op">,</span> <span class="op">!</span>flip_y<span class="op">)</span>
    <span class="op">{</span>
        add_ctrl<span class="op">(</span>m_angle<span class="op">)</span><span class="op">;</span>
        add_ctrl<span class="op">(</span>m_scale<span class="op">)</span><span class="op">;</span>
        m_angle<span class="op">.</span>label<span class="op">(</span><span class="str">"Angle=%3.2f"</span><span class="op">)</span><span class="op">;</span>
        m_scale<span class="op">.</span>label<span class="op">(</span><span class="str">"Scale=%3.2f"</span><span class="op">)</span><span class="op">;</span>
        m_angle<span class="op">.</span>range<span class="op">(</span><span class="op">-</span><span class="num">180</span><span class="op">.</span><span class="num">0</span><span class="op">,</span> <span class="num">180</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
        m_angle<span class="op">.</span>value<span class="op">(</span><span class="num">0</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
        m_scale<span class="op">.</span>range<span class="op">(</span><span class="num">0</span><span class="op">.</span><span class="num">1</span><span class="op">,</span> <span class="num">5</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
        m_scale<span class="op">.</span>value<span class="op">(</span><span class="num">1</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
    <span class="op">}</span>

    <span class="kw1">virtual</span> ~the_application<span class="op">(</span><span class="op">)</span>
    <span class="op">{</span>
    <span class="op">}</span>

    <span class="kw1">virtual</span> <span class="kw1">void</span> on_draw<span class="op">(</span><span class="op">)</span>
    <span class="op">{</span>
        <span class="kw1">typedef</span> agg::<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a><span class="op">&lt;</span><a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a><span class="op">&gt;</span>     <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a><span class="op">;</span>
        <span class="kw1">typedef</span> agg::<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a><span class="op">&lt;</span>pixfmt_pre<span class="op">&gt;</span> renderer_base_pre<span class="op">;</span>
       
        <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a>            pixf<span class="op">(</span>rbuf_window<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        pixfmt_pre        pixf_pre<span class="op">(</span>rbuf_window<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a>     rb<span class="op">(</span>pixf<span class="op">)</span><span class="op">;</span>
        renderer_base_pre rb_pre<span class="op">(</span>pixf_pre<span class="op">)</span><span class="op">;</span>

        rb<span class="op">.</span>clear<span class="op">(</span>agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba">rgba</a><span class="op">(</span><span class="num">1</span><span class="op">.</span><span class="num">0</span><span class="op">,</span> <span class="num">1</span><span class="op">.</span><span class="num">0</span><span class="op">,</span> <span class="num">1</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>

        agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a> src_mtx<span class="op">;</span>
        src_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_translation">trans_affine_translation</a><span class="op">(</span><span class="op">-</span>initial_width<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">-</span> <span class="num">10</span><span class="op">,</span> <span class="op">-</span>initial_height<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">-</span> <span class="num">20</span> <span class="op">-</span> <span class="num">10</span><span class="op">)</span><span class="op">;</span>
        src_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_rotation">trans_affine_rotation</a><span class="op">(</span>m_angle<span class="op">.</span>value<span class="op">(</span><span class="op">)</span> <span class="op">*</span> agg::<a href="http://www.antigrain.com/__code/include/agg_basics.h.html#pi">pi</a> <span class="op">/</span> <span class="num">180</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
        src_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_scaling">trans_affine_scaling</a><span class="op">(</span>m_scale<span class="op">.</span>value<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        src_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_translation">trans_affine_translation</a><span class="op">(</span>initial_width<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span><span class="op">,</span> initial_height<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">+</span> <span class="num">20</span><span class="op">)</span><span class="op">;</span>
        src_mtx <span class="op">*=</span> trans_affine_resizing<span class="op">(</span><span class="op">)</span><span class="op">;</span>

        agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a> img_mtx<span class="op">;</span>
        img_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_translation">trans_affine_translation</a><span class="op">(</span><span class="op">-</span>initial_width<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">+</span> <span class="num">10</span><span class="op">,</span> <span class="op">-</span>initial_height<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">+</span> <span class="num">20</span> <span class="op">+</span> <span class="num">10</span><span class="op">)</span><span class="op">;</span>
        img_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_rotation">trans_affine_rotation</a><span class="op">(</span>m_angle<span class="op">.</span>value<span class="op">(</span><span class="op">)</span> <span class="op">*</span> agg::<a href="http://www.antigrain.com/__code/include/agg_basics.h.html#pi">pi</a> <span class="op">/</span> <span class="num">180</span><span class="op">.</span><span class="num">0</span><span class="op">)</span><span class="op">;</span>
        img_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_scaling">trans_affine_scaling</a><span class="op">(</span>m_scale<span class="op">.</span>value<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        img_mtx <span class="op">*=</span> agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine_translation">trans_affine_translation</a><span class="op">(</span>initial_width<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span><span class="op">,</span> initial_height<span class="op">(</span><span class="op">)</span><span class="op">/</span><span class="num">2</span> <span class="op">+</span> <span class="num">20</span><span class="op">)</span><span class="op">;</span>
        img_mtx <span class="op">*=</span> trans_affine_resizing<span class="op">(</span><span class="op">)</span><span class="op">;</span>
        img_mtx<span class="op">.</span>invert<span class="op">(</span><span class="op">)</span><span class="op">;</span>

        agg::<a href="http://www.antigrain.com/__code/include/agg_span_allocator.h.html#span_allocator">span_allocator</a><span class="op">&lt;</span>color_type<span class="op">&gt;</span> sa<span class="op">;</span>

        <span class="kw1">typedef</span> agg::<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html#span_interpolator_linear">span_interpolator_linear</a><span class="op">&lt;</span><span class="op">&gt;</span> interpolator_type<span class="op">;</span>
        interpolator_type interpolator<span class="op">(</span>img_mtx<span class="op">)</span><span class="op">;</span>

        <span class="kw1">typedef</span> agg::<a href="http://www.antigrain.com/__code/include/agg_image_accessors.h.html#image_accessor_clip">image_accessor_clip</a><span class="op">&lt;</span><a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a><span class="op">&gt;</span> img_source_type<span class="op">;</span>

        <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a> img_pixf<span class="op">(</span>rbuf_img<span class="op">(</span><span class="num">0</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        img_source_type img_src<span class="op">(</span>img_pixf<span class="op">,</span> agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba_pre">rgba_pre</a><span class="op">(</span><span class="num">0</span><span class="op">,</span> <span class="num">0</span><span class="op">.</span><span class="num">4</span><span class="op">,</span> <span class="num">0</span><span class="op">,</span> <span class="num">0</span><span class="op">.</span><span class="num">5</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>

<span class="rem">/*
        // Version without filtering (nearest neighbor)
        //------------------------------------------
        typedef agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgb.h.html#span_image_filter_rgb_nn">span_image_filter_rgb_nn</a>&lt;img_source_type,
                                              interpolator_type&gt; span_gen_type;
        span_gen_type sg(img_src, interpolator);
        //------------------------------------------
*/</span>

        <span class="rem">// Version with "hardcoded" bilinear filter and without </span>
        <span class="rem">// image_accessor (direct filter, the old variant)</span>
        <span class="rem">//------------------------------------------</span>
        <span class="kw1">typedef</span> agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgb.h.html#span_image_filter_rgb_bilinear_clip">span_image_filter_rgb_bilinear_clip</a><span class="op">&lt;</span><a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a><span class="op">,</span>
                                                         interpolator_type<span class="op">&gt;</span> span_gen_type<span class="op">;</span>
        span_gen_type sg<span class="op">(</span>img_pixf<span class="op">,</span> agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba_pre">rgba_pre</a><span class="op">(</span><span class="num">0</span><span class="op">,</span> <span class="num">0</span><span class="op">.</span><span class="num">4</span><span class="op">,</span> <span class="num">0</span><span class="op">,</span> <span class="num">0</span><span class="op">.</span><span class="num">5</span><span class="op">)</span><span class="op">,</span> interpolator<span class="op">)</span><span class="op">;</span>
        <span class="rem">//------------------------------------------</span>

<span class="rem">/*
        // Version with arbitrary 2x2 filter
        //------------------------------------------
        typedef agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgb.h.html#span_image_filter_rgb_2x2">span_image_filter_rgb_2x2</a>&lt;img_source_type,
                                               interpolator_type&gt; span_gen_type;
        agg::<a href="http://www.antigrain.com/__code/include/agg_image_filters.h.html#image_filter">image_filter</a>&lt;agg::<a href="http://www.antigrain.com/__code/include/agg_image_filters.h.html#image_filter_kaiser">image_filter_kaiser</a>&gt; filter;
        span_gen_type sg(img_src, interpolator, filter);
        //------------------------------------------
*/</span>
<span class="rem">/*
        // Version with arbitrary filter
        //------------------------------------------
        typedef agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_rgb.h.html#span_image_filter_rgb">span_image_filter_rgb</a>&lt;img_source_type,
                                           interpolator_type&gt; span_gen_type;
        agg::<a href="http://www.antigrain.com/__code/include/agg_image_filters.h.html#image_filter">image_filter</a>&lt;agg::<a href="http://www.antigrain.com/__code/include/agg_image_filters.h.html#image_filter_spline36">image_filter_spline36</a>&gt; filter;
        span_gen_type sg(img_src, interpolator, filter);
        //------------------------------------------
*/</span>

        agg::<a href="http://www.antigrain.com/__code/include/agg_rasterizer_scanline_aa.h.html#rasterizer_scanline_aa">rasterizer_scanline_aa</a><span class="op">&lt;</span><span class="op">&gt;</span> ras<span class="op">;</span>
        ras<span class="op">.</span>clip_box<span class="op">(</span><span class="num">0</span><span class="op">,</span> <span class="num">0</span><span class="op">,</span> width<span class="op">(</span><span class="op">)</span><span class="op">,</span> height<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        agg::<a href="http://www.antigrain.com/__code/include/agg_scanline_u.h.html#scanline_u8">scanline_u8</a> sl<span class="op">;</span>
        <span class="kw1">double</span> r <span class="op">=</span> initial_width<span class="op">(</span><span class="op">)</span><span class="op">;</span>
        <span class="kw1">if</span><span class="op">(</span>initial_height<span class="op">(</span><span class="op">)</span> <span class="op">-</span> <span class="num">60</span> <span class="op">&lt;</span> r<span class="op">)</span> r <span class="op">=</span> initial_height<span class="op">(</span><span class="op">)</span> <span class="op">-</span> <span class="num">60</span><span class="op">;</span>
        agg::<a href="http://www.antigrain.com/__code/include/agg_ellipse.h.html#ellipse">ellipse</a> ell<span class="op">(</span>initial_width<span class="op">(</span><span class="op">)</span>  <span class="op">/</span> <span class="num">2</span><span class="op">.</span><span class="num">0</span> <span class="op">+</span> <span class="num">10</span><span class="op">,</span> 
                         initial_height<span class="op">(</span><span class="op">)</span> <span class="op">/</span> <span class="num">2</span><span class="op">.</span><span class="num">0</span> <span class="op">+</span> <span class="num">20</span> <span class="op">+</span> <span class="num">10</span><span class="op">,</span> 
                         r <span class="op">/</span> <span class="num">2</span><span class="op">.</span><span class="num">0</span> <span class="op">+</span> <span class="num">16</span><span class="op">.</span><span class="num">0</span><span class="op">,</span> 
                         r <span class="op">/</span> <span class="num">2</span><span class="op">.</span><span class="num">0</span> <span class="op">+</span> <span class="num">16</span><span class="op">.</span><span class="num">0</span><span class="op">,</span> <span class="num">200</span><span class="op">)</span><span class="op">;</span>


        agg::<a href="http://www.antigrain.com/__code/include/agg_conv_transform.h.html#conv_transform">conv_transform</a><span class="op">&lt;</span>agg::<a href="http://www.antigrain.com/__code/include/agg_ellipse.h.html#ellipse">ellipse</a><span class="op">&gt;</span> tr<span class="op">(</span>ell<span class="op">,</span> src_mtx<span class="op">)</span><span class="op">;</span>

        ras<span class="op">.</span>add_path<span class="op">(</span>tr<span class="op">)</span><span class="op">;</span>
        agg::<a href="http://www.antigrain.com/__code/include/agg_renderer_scanline.h.html#render_scanlines_aa">render_scanlines_aa</a><span class="op">(</span>ras<span class="op">,</span> sl<span class="op">,</span> rb_pre<span class="op">,</span> sa<span class="op">,</span> sg<span class="op">)</span><span class="op">;</span>

        agg::render_ctrl<span class="op">(</span>ras<span class="op">,</span> sl<span class="op">,</span> rb<span class="op">,</span> m_angle<span class="op">)</span><span class="op">;</span>
        agg::render_ctrl<span class="op">(</span>ras<span class="op">,</span> sl<span class="op">,</span> rb<span class="op">,</span> m_scale<span class="op">)</span><span class="op">;</span>
    <span class="op">}</span>

<span class="op">}</span><span class="op">;</span>





<span class="kw1">int</span> agg_main<span class="op">(</span><span class="kw1">int</span> argc<span class="op">,</span> <span class="kw1">char</span><span class="op">*</span> argv<span class="op">[</span><span class="op">]</span><span class="op">)</span>
<span class="op">{</span>
    the_application app<span class="op">(</span>pix_format<span class="op">,</span> flip_y<span class="op">)</span><span class="op">;</span>
    app<span class="op">.</span>caption<span class="op">(</span><span class="str">"Image Affine Transformations with filtering"</span><span class="op">)</span><span class="op">;</span>

    <span class="kw1">const</span> <span class="kw1">char</span><span class="op">*</span> img_name <span class="op">=</span> <span class="str">"spheres"</span><span class="op">;</span>
    <span class="kw1">if</span><span class="op">(</span>argc <span class="op">&gt;=</span> <span class="num">2</span><span class="op">)</span> img_name <span class="op">=</span> argv<span class="op">[</span><span class="num">1</span><span class="op">]</span><span class="op">;</span>
    <span class="kw1">if</span><span class="op">(</span><span class="op">!</span>app<span class="op">.</span>load_img<span class="op">(</span><span class="num">0</span><span class="op">,</span> img_name<span class="op">)</span><span class="op">)</span> 
    <span class="op">{</span>
        <span class="kw1">char</span> buf<span class="op">[</span><span class="num">256</span><span class="op">]</span><span class="op">;</span>
        <span class="kw1">if</span><span class="op">(</span>strcmp<span class="op">(</span>img_name<span class="op">,</span> <span class="str">"spheres"</span><span class="op">)</span> <span class="op">==</span> <span class="num">0</span><span class="op">)</span>
        <span class="op">{</span>
            sprintf<span class="op">(</span>buf<span class="op">,</span> <span class="str">"File not found: %s%s. Download http://www.antigrain.com/%s%s\n"</span>
                         <span class="str">"or copy it from another directory if available."</span><span class="op">,</span>
                    img_name<span class="op">,</span> app<span class="op">.</span>img_ext<span class="op">(</span><span class="op">)</span><span class="op">,</span> img_name<span class="op">,</span> app<span class="op">.</span>img_ext<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        <span class="op">}</span>
        <span class="kw1">else</span>
        <span class="op">{</span>
            sprintf<span class="op">(</span>buf<span class="op">,</span> <span class="str">"File not found: %s%s"</span><span class="op">,</span> img_name<span class="op">,</span> app<span class="op">.</span>img_ext<span class="op">(</span><span class="op">)</span><span class="op">)</span><span class="op">;</span>
        <span class="op">}</span>
        app<span class="op">.</span>message<span class="op">(</span>buf<span class="op">)</span><span class="op">;</span>
        <span class="kw1">return</span> <span class="num">1</span><span class="op">;</span>
    <span class="op">}</span>

    <span class="kw1">if</span><span class="op">(</span>app<span class="op">.</span>init<span class="op">(</span>app<span class="op">.</span>rbuf_img<span class="op">(</span><span class="num">0</span><span class="op">)</span><span class="op">.</span>width<span class="op">(</span><span class="op">)</span> <span class="op">+</span> <span class="num">20</span><span class="op">,</span> app<span class="op">.</span>rbuf_img<span class="op">(</span><span class="num">0</span><span class="op">)</span><span class="op">.</span>height<span class="op">(</span><span class="op">)</span> <span class="op">+</span> <span class="num">40</span> <span class="op">+</span> <span class="num">20</span><span class="op">,</span> agg::window_resize<span class="op">)</span><span class="op">)</span>
    <span class="op">{</span>

<span class="rem">// Test the plain/premultiplied issue</span>
<span class="rem">//-------------------</span>
<span class="rem">//typedef agg::<a href="http://www.antigrain.com/__code/include/agg_pixfmt_rgba.h.html#pixfmt_bgra32">pixfmt_bgra32</a>         <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a>; </span>
<span class="rem">//typedef agg::<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a>&lt;<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a>&gt; <a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a>;</span>
<span class="rem">//<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#pixfmt">pixfmt</a>        pixf(app.rbuf_img(0));</span>
<span class="rem">//<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#renderer_base">renderer_base</a> rb(pixf);</span>
<span class="rem">//for(unsigned i = 0; i &lt; app.rbuf_img(0).height(); i += 2)</span>
<span class="rem">//{</span>
<span class="rem">//    // Fully transparent</span>
<span class="rem">//    rb.copy_hline(0, i, app.rbuf_img(0).width(), agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba">rgba</a>(0, 0, 0, 0));  </span>
<span class="rem">//    if(i + 1 &lt; app.rbuf_img(0).height())</span>
<span class="rem">//    {</span>
<span class="rem">//        // Fully opaque white</span>
<span class="rem">//        rb.copy_hline(0, i + 1, app.rbuf_img(0).width(), agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#rgba">rgba</a>(1, 1, 1, 1));  </span>
<span class="rem">//    }</span>
<span class="rem">//}</span>

        <span class="kw1">return</span> app<span class="op">.</span>run<span class="op">(</span><span class="op">)</span><span class="op">;</span>
    <span class="op">}</span>
    <span class="kw1">return</span> <span class="num">0</span><span class="op">;</span>
<span class="op">}</span>



<span class="rem">/*
E:\agg23\examples\<a href="http://www.antigrain.com/demo/image1.cpp.html">image1.cpp</a>(111) : error C2664: 

  '__thiscall agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_gray.h.html#span_image_filter_gray_bilinear">span_image_filter_gray_bilinear</a>&lt;struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_gray.h.html#gray8">gray8</a>,
                                                   struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#order_bgra">order_bgra</a>,
                                                   class agg::<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html#span_interpolator_linear">span_interpolator_linear</a>&lt;class agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a>,8&gt; &gt;::agg::<a href="http://www.antigrain.com/__code/include/agg_span_image_filter_gray.h.html#span_image_filter_gray_bilinear">span_image_filter_gray_bilinear</a>&lt;struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_gray.h.html#gray8">gray8</a>,struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#order_bgra">order_bgra</a>,class agg::<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html#span_interpolator_linear">span_interpolator_linear</a>&lt;class agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a>,8&gt; &gt;(class agg::<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html#span_interpolator_linear">span_interpolator_linear</a>&lt;class agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a>,8&gt; &amp;,const class agg::<a href="http://www.antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html#row_ptr_cache">row_ptr_cache</a>&lt;unsigned char&gt; &amp;,const struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_gray.h.html#gray8">gray8</a> &amp;,struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_rgba.h.html#order_bgra">order_bgra</a> &amp;)' : 

cannot convert parameter 1 from 

'class agg::<a href="http://www.antigrain.com/__code/include/agg_span_allocator.h.html#span_allocator">span_allocator</a>&lt;struct agg::<a href="http://www.antigrain.com/__code/include/agg_color_gray.h.html#gray8">gray8</a>&gt;' to 
'class agg::<a href="http://www.antigrain.com/__code/include/agg_span_interpolator_linear.h.html#span_interpolator_linear">span_interpolator_linear</a>&lt;class agg::<a href="http://www.antigrain.com/__code/include/agg_trans_affine.h.html#trans_affine">trans_affine</a>,8&gt; &amp;'
*/</span>
</pre></td></tr></tbody></table><font style="margin-left: 1em;"><i></i></font><table style="margin: 0px;" height="1px" width="640px" bgcolor="#583927" border="0" cellpadding="0" cellspacing="0"><tbody><tr><td></td></tr></tbody></table>
<table width="640px" border="0" cellpadding="0" cellspacing="0">
<tbody><tr><td><center><span class="authors">
Copyright <span class="larger">©</span> 2002-2006
<a href="http://www.antigrain.com/mcseem/index.html"><b>Maxim Shemanarev</b></a>
</span></center></td></tr>
<tr><td><center><span class="authors">
Web Design and Programming
<a href="http://www.antigrain.com/mcseem/index.html"><b>Maxim Shemanarev</b></a>
</span></center></td></tr>
</tbody></table>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
</body></html>