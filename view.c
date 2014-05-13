#include "view.h"

#include "error.h"

#include <math.h>

/*
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
*/

void init_event(event_t *event)
{
    event->exit_wanted = 0;
}

void get_event(event_t *event, view_t *view)
{
    SDL_Event sdl_event;

    init_event(event);

    event->key = NO_KEY;

    if(SDL_PollEvent(&sdl_event))
    {
        switch(sdl_event.type)
        {
            case SDL_QUIT:
            event->exit_wanted = 1;
            break;
            case SDL_KEYDOWN:
            switch(sdl_event.key.keysym.sym)
            {
                case SDLK_UP:
                event->key = KEY_UP;
                break;
                case SDLK_DOWN:
                event->key = KEY_DOWN;
                break;
                case SDLK_RIGHT:
                event->key = KEY_RIGHT;
                break;
                case SDLK_LEFT:
                event->key = KEY_LEFT;
                break;
                default:
                event->key = NO_KEY;
                break;
            }
            break;
        }
    }
}

double temporize(config_t *conf)
{
    static Uint32 ticks = 0;
    Uint32 new_ticks = 0;
    double real_delay = 0;

    if(ticks == 0)
        ticks = SDL_GetTicks();

    SDL_Delay(conf->delay);

    new_ticks = SDL_GetTicks();
    real_delay = (double) (new_ticks - ticks) / 1000;
    ticks = new_ticks;

    return real_delay; 
}

view_t* init_view(config_t *conf)
{
	view_t *view = malloc(sizeof(view_t));

	if(view == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR, "Erreur d'allocation de view_t dans init_view.");
		return NULL;
	}

    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        printf("Erreur: erreur d'initialisation de SDL.\n");
        exit(1);
    }

    view->screen = SDL_SetVideoMode(conf->screen_width, conf->screen_height, 32, SDL_OPENGL);
    SDL_EnableKeyRepeat(10, 10);

    if(view->screen == NULL)
    {
        new_error(SDL_ERROR, SDL_GetError());
        return NULL;
    }

    view->part_size = 4;
    view->angle1 = 0;
    view->angle2 = 0;
    view->d = 3;

    create_texture(view);

    view->cube = glGenLists(1);
    GLfloat Rose[] = {0.99f, 0.99f, 0.99f, 0.50f};
    glNewList(view->cube, GL_COMPILE);
    glBegin(GL_QUADS);

    glColor4fv(Rose);
    glNormal3d(-1.0,0.0,0.0);
    glVertex3d( -1, 1, 1);
    glVertex3d( -1, 1, -1);
    glVertex3d( -1, -1, -1);
    glVertex3d( -1, -1, 1);
    glNormal3d(0.0,1.0,0.0);
    glVertex3d( -1, 1, 1);
    glVertex3d( 1, 1, 1);
    glVertex3d( 1, 1, -1);
    glVertex3d( -1, 1, -1);
    glNormal3d(0.0,-1.0,0.0);
    glVertex3d( -1, -1, 1);
    glVertex3d( -1, -1, -1);
    glVertex3d( 1, -1, -1);
    glVertex3d( 1, -1, 1);
    glNormal3d(0.0,0.0,-1.0);
    glVertex3d( 1, 1, -1);
    glVertex3d( 1, -1, -1);
    glVertex3d( -1, -1, -1);
    glVertex3d( -1, 1, -1);
    glNormal3d(1.0,0.0,0.0);
    glVertex3d( 1, 1, 1);
    glVertex3d( 1, -1, 1);
    glVertex3d( 1, -1, -1);
    glVertex3d( 1, 1, -1);
    glNormal3d(0.0,0.0,1.0);
    glVertex3d(-1, 1, 1);
    glVertex3d(-1, -1, 1);
    glVertex3d( 1, -1, 1);
    glVertex3d( 1, 1, 1);
    glEnd();
    glEndList();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70,conf->screen_width/conf->screen_height,1,10000000);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.129, 0.169, 0.133, 1);
    glEnable(GL_TEXTURE_2D);
    return view;
}

void update_view(view_t *view, model_t *model, event_t *event)
{
    const double delta = M_PI/60;
    switch(event->key)
    {
        case KEY_UP: 
        view->angle1 += delta;
        break;

        case KEY_DOWN:
        view->angle1 -= delta;
        break;

        case KEY_LEFT:
        view->angle2 += delta;
        break;

        case KEY_RIGHT:
        view->angle2 -= delta;
        break;

        case KEY_FORWARD:
        view->d++;
        break;

        case KEY_BACKWARD:
        view->d--;
        break;
    }

    if(view->angle1 > M_PI/2)
        view->angle1 = M_PI/2;

    if(view->angle1 < -M_PI/2)
        view->angle1 = -M_PI/2;

    double t = view->d*cos(view->angle1);
    double z = view->d*sin(view->angle1);
    double x = t*cos(view->angle2);
    double y = t*sin(view->angle2);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPointSize(view->part_size);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x,y,z,0,0,0,0,0,1);


    glBegin(GL_POINTS);
    glColor3ub(50, 50, 255);
    glVertex3d(0,0,0);
    glVertex3d(0.75,0,0);

    particule_t *part = NULL;
    int i=0, j=0;

    for(i = 0; i < model->num_chunk; i++)
    {
        for(j = 0; j < model->size_of_chunk; j++)
        {
            part = &(model->chunk_list[i][j]);
            glVertex3d(part->pos.x, part->pos.y, part->pos.z);
        }
    }

    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCallList(view->cube);
    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
glPushMatrix(); // on enregistre les paramètres pour la 3D
glLoadIdentity();
gluOrtho2D(-690, 110, 0, 500);

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glBindTexture(GL_TEXTURE_2D, view->id_start_tex);
create_rec(450);
glBindTexture(GL_TEXTURE_2D, view->id_stop_tex);
create_rec(375);
glBindTexture(GL_TEXTURE_2D, view->id_pause_tex);
create_rec(300);
glBindTexture(GL_TEXTURE_2D, view->id_reset_tex);
create_rec(225);
glBindTexture(GL_TEXTURE_2D, 0);

glMatrixMode(GL_PROJECTION);
glPopMatrix(); // on restitue les paramètres pour la 3D

glEnable(GL_DEPTH_TEST);

glFlush();
SDL_GL_SwapBuffers();
}

void create_rec(double but_height)
{
    glBegin(GL_QUADS);

     
    glTexCoord2d(0,0); glVertex2d(0,but_height);
    glTexCoord2d(1,0); glVertex2d(100, but_height);
    glTexCoord2d(1,1); glVertex2d(100, but_height-50);
    glTexCoord2d(0,1); glVertex2d(0, but_height-50);

    glEnd();    
}

GLuint load_text(char *filename)
{
    GLuint id;
    SDL_Surface *image = IMG_Load(filename);

    if(image == 0)
    {
        printf("Erreur: Impossible de charger une images du menu");
        exit(1);
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    GLint formatInterne;
    GLint format;

    if(image->format->BytesPerPixel == 3)
    {
             // Format interne
        formatInterne = GL_RGB;
        // Format
        if(image->format->Rmask == 0xff)
            format = GL_RGB;
        else
            format = GL_BGR;
    }
    else if(image->format->BytesPerPixel == 4)
    {
                    // Format interne

        formatInterne = GL_RGBA;
        // Format
        if(image->format->Rmask == 0xff)
            format = GL_RGBA;
        else
            format = GL_BGRA;
    }
    else
    {
        printf("Erreur: Format de l'image inconnue");
        SDL_FreeSurface(image);
        exit(1);
    }
// Copie des pixels
    glTexImage2D(GL_TEXTURE_2D, 0, formatInterne, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    // Application des filtres
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Déverrouillage
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

void create_texture(view_t *view)
{
    view->id_start_tex = load_text("buttons/start.png");
    view->id_stop_tex = load_text("buttons/stop.png");
    view->id_pause_tex = load_text("buttons/pause.png");
    view->id_reset_tex = load_text("buttons/reset.png");
}

void close_view(view_t *view)
{
    SDL_Quit();
}