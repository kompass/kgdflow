#include "view.h"

#include "error.h"

#include <math.h>


void init_vect2d(vect2d_t *vect, int x, int y)
{
    vect->x = x;
    vect->y = y;
}

void init_button(button_t *butt, int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*))
{
    init_vect2d(&(butt->pos), xpos, ypos);
    init_vect2d(&(butt->size), xsize, ysize);
    butt->callback = callback;
}

button_t* new_button(int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*))
{
    button_t *butt = malloc(sizeof(button_t));

    init_button(butt, xpos, ypos, xsize, ysize, callback);

    return butt;
}

void init_button_list(button_list_t *list)
{
    list->size = 0;
    list->tab = NULL;
}

void add_button(button_list_t *list, button_t *butt)
{
    button_t **new_list = malloc((list->size + 1) * sizeof(button_t*));
    int i = 0;

    for(i = 0; i < list->size; i++)
    {
        new_list[i] = list->tab[i];
    }

    new_list[list->size] = butt;
    free(list->tab);
    list->tab = new_list;
    list->size++;
}

int is_in_butt(button_t *butt, int x, int y)
{
    return x > butt->pos.x && x < (butt->pos.x + butt->size.x) &&
    y > butt->pos.y && y < (butt->pos.y + butt->size.y);
}

void apply_click(button_list_t *list, int x, int y, event_t *event)
{
    int i = 0;

    for(i = 0; i < list->size; i++)
    {
        if(is_in_butt(list->tab[i], x, y))
        {
            event->click_callback = list->tab[i]->callback;
        }
    }
}

void init_event(event_t *event)
{
    event->exit_wanted = 0;
    event->key = NO_KEY;
    event->click_callback = NULL;
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
            if(sdl_event.key.keysym.mod & KMOD_CTRL)
            {
                switch(sdl_event.key.keysym.sym)
                {
                    case SDLK_UP:
                    event->key = KEY_FORWARD;
                    break;
                    case SDLK_DOWN:
                    event->key = KEY_BACKWARD;
                    break;
                    default:
                    event->key = NO_KEY;
                    break;
                }
            }
            else
            {
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
            }
            break;

            case SDL_MOUSEBUTTONDOWN:
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
                apply_click(&(view->button_list),
                    sdl_event.button.x, sdl_event.button.y, event);
            break;
        }
    }
}

double temporize(config_t *conf)
{
    static Uint32 ticks = 0;
    static int fps = 0;
    static int seconds = 0;
    Uint32 new_ticks = 0;
    double real_delay = 0;

    //Initialisation des variables static à leur première utilisation
    if(ticks == 0)
    {
        ticks = SDL_GetTicks();
        seconds = ticks / 1000;
    }

    int wanted_delay = conf->delay - (SDL_GetTicks() - ticks);
    if(wanted_delay > 0)
        SDL_Delay(wanted_delay);

    new_ticks = SDL_GetTicks();
    real_delay = (double) (new_ticks - ticks) / 1000;
    ticks = new_ticks;

    fps++;
    if(ticks / 1000 - seconds >= 1)
    {
        printf("FPS: %i\n", fps);
        fps = 0;
        seconds = ticks / 1000;
    }

    return real_delay; 
}

void reset_not_implemented(model_t *model)
{
    printf("Attention: Reset n'est pas encore implémenté.\n");
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

    init_button_list(&(view->button_list));

    button_t *start_butt = new_button(conf->screen_width - 110, 50, 100, 50, start_model);
    add_button(&(view->button_list), start_butt);

    button_t *stop_butt = new_button(conf->screen_width - 110, 125, 100, 50, stop_model);
    add_button(&(view->button_list), stop_butt);

    button_t *pause_butt = new_button(conf->screen_width - 110, 200, 100, 50, pause_model);
    add_button(&(view->button_list), pause_butt);

    button_t *reset_butt = new_button(conf->screen_width - 110, 275, 100, 50, reset_not_implemented);
    add_button(&(view->button_list), reset_butt);

    view->part_size = 1;
    view->angle1 = 0;
    view->angle2 = 0;
    view->d = 2;
    view->screen_width = conf->screen_width;
    view->screen_height = conf->screen_height;

    create_texture(view);

    view->cube = glGenLists(1);
    GLfloat cube_color[] = {1, 1, 1, 0.05};
    glNewList(view->cube, GL_COMPILE);
    glBegin(GL_QUADS);

    glColor4fv(cube_color);
    glNormal3d(-1.0,0.0,0.0);
    glVertex3d( 0, 1, 1);
    glVertex3d( 0, 1, 0);
    glVertex3d( 0, 0, 0);
    glVertex3d( 0, 0, 1);
    glNormal3d(0.0,1.0,0.0);
    glVertex3d( 0, 1, 1);
    glVertex3d( 1, 1, 1);
    glVertex3d( 1, 1, 0);
    glVertex3d( 0, 1, 0);
    glNormal3d(0.0,-1.0,0.0);
    glVertex3d( 0, 0, 1);
    glVertex3d( 0, 0, 0);
    glVertex3d( 1, 0, 0);
    glVertex3d( 1, 0, 1);
    glNormal3d(0.0,0.0,-1.0);
    glVertex3d( 1, 1, 0);
    glVertex3d( 1, 0, 0);
    glVertex3d( 0, 0, 0);
    glVertex3d( 0, 1, 0);
    glNormal3d(1.0,0.0,0.0);
    glVertex3d( 1, 1, 1);
    glVertex3d( 1, 0, 1);
    glVertex3d( 1, 0, 0);
    glVertex3d( 1, 1, 0);
    glNormal3d(0.0,0.0,1.0);
    glVertex3d(0, 1, 1);
    glVertex3d(0, 0, 1);
    glVertex3d( 1, 0, 1);
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
        view->d -= delta;
        break;

        case KEY_BACKWARD:
        view->d += delta;
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
    glTranslated(-model->part_grid.delta*model->part_grid.size/2,
        -model->part_grid.delta*model->part_grid.size/2,
        -model->part_grid.delta*model->part_grid.size/2);

    glBegin(GL_POINTS);
    glColor3ub(255, 0, 0);

    particule_t *part = NULL;
    int i=0, j=0;

    for(i = 0; i < model->num_chunk; i++)
    {
        int size = model->size_of_chunk;
        for(j = 0; j < size*size*size; j++)
        {
            part = &(model->chunk_list[i][j]);
            glColor3ub(255, (part->press-9)*100, (part->press-9)*100);
            glVertex3d(part->pos.x, part->pos.y, part->pos.z);
        }
    }

    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glScaled(model->part_grid.delta*model->part_grid.size,
        model->part_grid.delta*model->part_grid.size,
        model->part_grid.delta*model->part_grid.size);
    glCallList(view->cube);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix(); 
    glLoadIdentity();
    gluOrtho2D(-(view->screen_width - 110), 110, 0, view->screen_height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, view->id_start_tex);
    create_rec(view->screen_height - 50);
    glBindTexture(GL_TEXTURE_2D, view->id_stop_tex);
    create_rec(view->screen_height - 125);
    glBindTexture(GL_TEXTURE_2D, view->id_pause_tex);
    create_rec(view->screen_height - 200);
    glBindTexture(GL_TEXTURE_2D, view->id_reset_tex);
    create_rec(view->screen_height - 275);
    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); 

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