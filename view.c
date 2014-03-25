#include "view.h"

Uint32 get_pixel(view_t *view, int x, int y)
{
    int bpp = view->screen->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)view->screen->pixels + y * view->screen->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;
    }
}

void set_pixel(view_t *view, int x, int y, Uint32 pixel)
{
    int bpp = view->screen->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)view->screen->pixels + y * view->screen->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void set_transp_pixel(view_t *view, int x, int y, Uint32 pixel, int coef)
{
	Uint32 old_pixel = get_pixel(view, x, y);
	Uint8 *old_pixel_colors = (Uint8 *) &old_pixel;
	Uint8 *pixel_colors = (Uint8 *) &pixel;

	if(coef < 0)
		coef = 0;

	if(coef > 255)
		coef = 255;

	int i = 0;
	for(i = 0; i < 3; i++)
		pixel_colors[i] = (coef*pixel_colors[i] + (255 - coef)*old_pixel_colors[i])/255;

	set_pixel(view, x, y, pixel);
}

view_t* init_view(config_t *conf)
{
	view_t *view = malloc(sizeof(view_t));

	if(view == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR, "Erreur d'allocation de view_t dans init_view.");
		return NULL;
	}

	view->screen = SDL_SetVideoMode(conf->screen_width, conf->screen_height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if(view->screen == NULL)
	{
		new_error(SDL_ERROR, SDL_GetError());
		return NULL;
	}

	return view;
}

void close_view(view_t *view)
{
	SDL_Quit();
}