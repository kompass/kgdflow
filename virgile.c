#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "boid.h"
#include "vecteur.h"
#include "modele.h"

int init(int xPix, int yPix){
    SDL_Surface *ecran = NULL;
//initialisation de la SDL
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL");
        exit(EXIT_FAILURE);
        return -1;
    }
    ecran = SDL_SetVideoMode(xPix,yPix, 32, SDL_OPENGL);
    if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
    {
        fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
        return -1;
    }
    SDL_WM_SetCaption("boid v0.0",NULL);
    //mdéfinition du mode de projection
    glMatrixMode( GL_PROJECTION );
    //réinitialisation de la matrice
    glLoadIdentity();
    //projection en persepective fovy = 70° le ratio est calculé automatiquement
    gluPerspective(70,(double)xPix/yPix,1,10000000);
    glEnable(GL_DEPTH_TEST);
    return 1;
}
void pre_dessin(int x,int y, int z, int dim3){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    if(dim3){
       gluLookAt(x,y,z,1000,1000,1000,1,0,0);
   }else{
       gluLookAt(1000,1000,600,1000,1000,0,0,1,0);
   }
   
}
void dessin_point(double x, double y, double z,int taille, int r, int g, int b,int dim3){
    if (taille<=0) {
        taille = 1;
    }
    if (r<0 || r>255) {
        r=0;
    }
    if (g<0 || g>255) {
        g=0;
    }
    if (b<0 || b>255) {
        b=0;
    }
    glPointSize(taille);
    glBegin(GL_POINTS);
    glColor3ub(r,g,b);
    if(dim3){
        glVertex3d(x,y,z);
    }else{
        glVertex2d(x,y);
    }	
    glEnd();
}
void dessin_point_from_vect(vecteur_t *v,int taille, int r, int g, int b, int dim3){
    dessin_point(v->x,v->y,v->z,taille,r,g,b,dim3);
}
void dessin_boid(boid_t *b ,int dim3){
    dessin_point_from_vect(b->pos,50,b->r,b->g,b->b,dim3);
}
void dessin_pre(boid_t *b ,int dim3){
    dessin_point_from_vect(b->pos,80,255,0,0,dim3);
}
void dessin_food(vecteur_t *v, int dim3){
    dessin_point_from_vect(v,80,0,0,255,dim3);
}
void dessin_modele(modele_t *m){
    int i;
    for(i=0;i<m->nbBoid;i++){
        dessin_boid(m->tabBoid[i],m->dimention3);
    }
    for(i=0;i<m->nbPre;i++){
        dessin_pre(m->tabPre[i],m->dimention3);
    }
    for(i=0;i<m->nbFood;i++){
        dessin_food(m->tabFood[i],m->dimention3);
    }
}
void maj_affichage(){
    glFlush();
    SDL_GL_SwapBuffers();
}