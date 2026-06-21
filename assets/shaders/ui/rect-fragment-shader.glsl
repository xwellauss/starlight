#version 300 es

precision mediump float;

in vec4 f_color;
in vec2 f_norm_pos;
in vec2 f_rect_pos;
in vec2 f_rect_size;
in vec4 f_corner_radius;
in vec4 f_border_width;

out vec4 FragColor;

uniform sampler2D sampler;
uniform bool has_texture;

// Used Claude and clay example renderer

void main()
{
	vec2 pixel = f_rect_pos + f_norm_pos * f_rect_size;
	vec2 local = pixel - f_rect_pos;
	float w = f_rect_size.x;
    float h = f_rect_size.y;

	float tl = f_corner_radius.x;
    float tr = f_corner_radius.y;
    float bl = f_corner_radius.z;
    float br = f_corner_radius.w;

	float outer_alpha = 1.0;
    if(tl > 0.0 && local.x < tl && local.y < tl)
        outer_alpha = step(length(local - vec2(tl, tl)), tl);
    if(tr > 0.0 && local.x > w - tr && local.y < tr)
        outer_alpha *= step(length(local - vec2(w - tr, tr)), tr);
    if(bl > 0.0 && local.x < bl && local.y > h - bl)
        outer_alpha *= step(length(local - vec2(bl, h - bl)), bl);
    if(br > 0.0 && local.x > w - br && local.y > h - br)
        outer_alpha *= step(length(local - vec2(w - br, h - br)), br);

    if(outer_alpha < 0.5) discard;

    bool has_border = f_border_width.x > 0.0 || f_border_width.y > 0.0 || f_border_width.z > 0.0 || f_border_width.w > 0.0;

    if(has_border)
    {
        float T = f_border_width.x;
        float R = f_border_width.y;
        float B = f_border_width.z;
        float L = f_border_width.w;

        float iw = w - L - R;
        float ih = h - T - B;
        vec2 inner_local = local - vec2(L, T);

        float itl = max(tl - min(T, L), 0.0);
        float itr = max(tr - min(T, R), 0.0);
        float ibl = max(bl - min(B, L), 0.0);
        float ibr = max(br - min(B, R), 0.0);

        bool inside_inner = true;
        if(itl > 0.0 && inner_local.x < itl && inner_local.y < itl)
            inside_inner = length(inner_local - vec2(itl, itl)) <= itl;
        if(itr > 0.0 && inner_local.x > iw - itr && inner_local.y < itr)
            inside_inner = inside_inner && length(inner_local - vec2(iw - itr, itr)) <= itr;
        if(ibl > 0.0 && inner_local.x < ibl && inner_local.y > ih - ibl)
            inside_inner = inside_inner && length(inner_local - vec2(ibl, ih - ibl)) <= ibl;
        if(ibr > 0.0 && inner_local.x > iw - ibr && inner_local.y > ih - ibr)
            inside_inner = inside_inner && length(inner_local - vec2(iw - ibr, ih - ibr)) <= ibr;

        if(inside_inner && inner_local.x >= 0.0 && inner_local.x <= iw && inner_local.y >= 0.0 && inner_local.y <= ih) discard;
    }

    if(has_texture)
        FragColor = texture(sampler, f_norm_pos) * f_color;
    else
        FragColor = f_color;
}
