#ifndef GLOBAL_H_INCLUDED

#include <MATH.H>
#include <FLOAT.H>
#include <DOS.H>
#include <STDLIB.H>
#include <STDIO.H>

#if __WATCOMC__
#include "LIB.H"
#endif

// ----------------------------------------------------- DECLARATIONS GLOBALES

  #define NB_OBJET_MAX 20000
  #define NB_TRIANGLE_MAX 20000
  #define NB_POLY_MAX 30
  #define NB_OMNI_MAX 30
  #define NB_SPOT_MAX 30
  #define NB_CYLLIGHT_MAX 30
  #define NB_AREA_MAX 30
  #define NB_CAMERA_MAX 10
  #define NB_SPLINE_MAX 20


#define PI 3.14159265358979323846264338327950288419716939937511
#define M_PI_2 (PI/2)
#define PIs180 (PI/180.0)
#define DBL_MIN_ -FLT_MAX
#define DBL_MAX_  FLT_MAX
#define NULLC '\0'

#define SCALE		 0
#define ROTATE		 1
#define TRANSLATE	 2
#define DIVERS		 3
#define MIROIR       4

#define CYLINDRE	 0
#define SPHERE		 1
#define CUBE		 2
#define CONE		 3
#define TORE		 4
#define TUBE         5
#define PLANX		 6
#define PLANY		 7
#define PLANZ		 8
#define CERCLE12	 9
#define CERCLEX24   10
#define CERCLEY24	11
#define CERCLEZ24	12
#define TRIANGLE    13
#define ANNEAU      14
#define DISQUE      15
#define PRISME      16
#define QTUBE       17
#define CONET       18
#define PYRAMIDE    19
#define CARREZ		20
#define CARREY		21
#define CARREX		22
#define CAMERA		23
#define OMNI        24
#define SPOT        25
#define OEIL		26
#define CIBLE		27
#define AMPOULE     28
#define CYLLIGHT    29
#define AXES		30
#define GRILLE      31
#define EXTRUDE     32
#define CUBE_R      33
#define CUBE_C      34
#define DSPHERE     35
#define QTORE       36
#define BLOB        37
#define HFIELD      38
#define BLOBC       39
#define SPLINE      40
#define MAPPING     41
#define SUPEREL     42
#define LATHE       43
#define SOR         44
#define AUTOMAP     45
#define AREA        46
#define BEZIER      47

#define COEIL       10
#define CCIBLE      11
#define SOEIL       12
#define SCIBLE      13

#define VRAI 1
#define FAUX 0
#define NVRAI +29999  // vrai en num‚rique
#define NFAUX -29999  // faux en num‚rique

#define CLIP_ON  1
#define CLIP_OFF 0

#define MS_SELECTEUR 0
#define MS_FLECHE	 1
#define MS_X		 2	 // Droite-Gauche
#define MS_Y		 3	 // Haut-Bas
#define MS_XY		 4	 // Haut-Bas/Droite-Gauche
#define MS_CROIX     5   // Croix

#define GAUCHE 0
#define CENTRE 1
#define DROITE 2

#define PCHAMP 255

#define EFFACE  0
#define AFFICHE 1
#define SAUVE	2
#define GELE    3
#define LAISSE  4

#define ATTEND 0
#define PASSE  1

#define INVISIBLE 256
#define MODIF	   14
#define OBJET		7

#define CONFIG 1
#define IMPORT 1

#define PAS_CSG 0
#define OK_CSG  1
#define UNION   2
#define INTERSE 3
#define DIFFERE 4
#define MERGE   5
#define INVERSE 6

#define _X 0
#define _Y 1
#define _Z 2

#define GIF 1
#define TGA 2

#define LEN_TEXT_MAX 33
#define MAX_BIT_TEXTURE 20

#if __WATCOMC__

// ---------------------------------------------------------- MATRICE.C

#define Efface_Transform(x) if ((x)!=NULL) free(x)

typedef DBL Vecteur[3];
typedef DBL MATRIX [4][4];

struct Vector_Struct { DBL x, y, z; };

typedef struct Vector_Struct VECTOR;

struct Transform_Struct {
  MATRIX matrix;
  MATRIX inverse;
};

typedef struct Transform_Struct TRANSFORM;

#define VScale(a, b, k) {(a).x=(b).x*(k);(a).y=(b).y*(k);(a).z=(b).z*(k);}
#define VLength(a, b) {a=sqrt((b).x*(b).x+(b).y*(b).y+(b).z*(b).z);}
#define Make_Vector(v,a,b,c) { (v)->x=(a);(v)->y=(b);(v)->z=(c); }
#define VInverseScaleEq(a, k) {(a).x/=(k);(a).y/=(k);(a).z/=(k);}
#define signe(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

void MIdentity (MATRIX *result);
void MTimes (MATRIX *result,MATRIX *matrix1,MATRIX *matrix2);
void MTranspose (MATRIX *result,MATRIX *matrix1);
void Compute_Scaling_Transform (TRANSFORM *result,VECTOR *vector);
void Compute_Translation_Transform (TRANSFORM *transform,VECTOR *vector);
void Compute_Rotation_Transform (TRANSFORM *transform,VECTOR *vector);
void Compose_Transforms (TRANSFORM *Original_Transform,TRANSFORM *New_Transform);
void Compute_Axis_Rotation_Transform (TRANSFORM *transform,VECTOR *V,DBL angle);
TRANSFORM *Create_Transform(void);
TRANSFORM *Copy_Transform (TRANSFORM *Old);
VECTOR *Create_Vector(void);
VECTOR *Copy_Vector (VECTOR *Old);
void matrix_decode (MATRIX mat,Vecteur scale,Vecteur shear,Vecteur rotate,Vecteur transl);
void translation_objet(TRANSFORM *Object,Vecteur V,byte Signe);
void rotation_objet(TRANSFORM *Object,Vecteur V,byte Signe);
void ajustement_objet(TRANSFORM *Object,Vecteur V,byte Signe);
void mat_identity (MATRIX mat);
void mat_rotate (MATRIX mat1,MATRIX mat2,int axis,DBL angle);
void mat_axis_rotate (MATRIX mat1,MATRIX mat2,Vecteur axis,DBL angle);
void mat_mult (MATRIX mat1,MATRIX mat2,MATRIX mat3);
void mat_decode (MATRIX mat,Vecteur scale,Vecteur shear,Vecteur rotate,Vecteur transl);
DBL  mat_inv (MATRIX mat1,MATRIX mat2);
void adjoint (MATRIX mat);
DBL det4x4 (MATRIX mat);
DBL det3x3 (DBL a1,DBL a2,DBL a3,DBL b1,DBL b2,DBL b3,DBL c1,DBL c2,DBL c3);
DBL det2x2 (DBL a,DBL b,DBL c,DBL d);

// ---------------------------------------------------------- STRUCTURES

typedef struct Oeil_Camera {
  DBL OX,OY,OZ,CX,CY,CZ;  // Oeil X,Y,Z - Cible X,Y,Z
  byte Cache;
  DBL Ouverture;
  DBL ProChamp;
  byte OnOff;
  byte F_Blur;
  DBL Samples;
  DBL Aperture;
  DBL Variance;
  DBL Confidence;
  DBL Roll;
};

typedef struct Lumiere_Spot {
  DBL OX,OY,OZ,CX,CY,CZ;  // Oeil X,Y,Z - Cible X,Y,Z
  byte Cache;
  byte RVB[3];
  byte Taille;
  DBL Angle1;
  DBL Angle2;
  byte OnOff;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  byte Cone;
  byte Area;
  Vecteur Axis1;
  Vecteur Axis2;
  int Size1;
  int Size2;
  byte Jitter;
  DBL Adaptive;
  char Flare[MAXPATH];
};

typedef struct Lumiere_CylLight {
  DBL OX,OY,OZ,CX,CY,CZ;  // Oeil X,Y,Z - Cible X,Y,Z
  byte Cache;
  byte RVB[3];
  byte Taille;
  DBL Angle1;
  DBL Angle2;
  byte OnOff;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  byte Cone;
  char Flare[MAXPATH];
};

typedef struct Lumiere_Omni {
  Vecteur Point;
  byte Cache;
  byte RVB[3];
  byte Taille;
  byte OnOff;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  char Flare[MAXPATH];
};

typedef struct Lumiere_Area {
  Vecteur Point;
  byte Cache;
  byte RVB[3];
  byte Taille;
  DBL Rayon;
  byte OnOff;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  Vecteur Axis1;
  Vecteur Axis2;
  int Size1;
  int Size2;
  byte Jitter;
  DBL Adaptive;
  char Flare[MAXPATH];
};

typedef struct Type_Sujet {
  byte Type;
  int Num;
} CibleOeil;

typedef struct Tableau {
  Vecteur V;
  struct Tableau *Next;
} VERTEX;

typedef struct Brouillard {
  byte Ok;
  byte Type;
  DBL  Offset;
  DBL  Alt;
  DBL  TurbD;
  DBL  Turbulence;
  DBL  Distance;
  byte RGB[3];
} FOG;

typedef struct {
  byte Type;
  Vecteur P; // Points fictifs (buffers,temporaires,r‚els suivant objets)
  Vecteur S; // scale/ajuster
  Vecteur R; // rotate/pivoter
  Vecteur T; // translate/deplacer
  byte Couleur;
  byte Selection;
  byte Cache;
  char Matiere[LEN_TEXT_MAX];
  byte ScaleOM;
  int Reflexion;
  int Refraction;
  DBL Ior;
  int Diffusion;
  int Ambiance;
  int Crand;
  int Phong;
  int Specular;
  int PSize;
  int Caustics;
  int Fade_D;
  int Fade_P;
  int Rough;
  int Brilli;
  byte BitTexture[MAX_BIT_TEXTURE];
  struct {
    char Name[14];
    byte On;
    byte Type;
    byte Once;
    byte Interpolate;
    DBL  Amount;
    byte Alpha;
    DBL  Filter;
    int  Color;
  } Map[2];
  Vecteur MS; // scale texture
  Vecteur MR; // rotate texture
  Vecteur MT; // translate texture
  char Nom[13];
  int Buffer;
  int Poly;
  byte Rapide; // 0=normal, 1=rapide, 2=cube
  byte Smooth;
  char CheminRaw[MAXPATH];
  DBL HautHF;
  DBL WLevel;
  byte Ignore;
  byte Freeze;
  byte Operator;
  int CSG;
  byte Inverse;
  byte Ombre;
  byte Halo;
  struct {
    int Nb;
    byte Light;
  } LooksLike;
  struct {
    byte Type;
    int Nombre;
    VERTEX *Root;
    VERTEX *Pt;
  } Special;
} VOLUME;

typedef struct Noeud {
  DBL X,Y,Z;
  byte V;
  int C;
};

typedef struct {
  byte Buffer;
  int Nombre;
  byte Smooth;
  DBL T1[NB_TRIANGLE_MAX][3];
  DBL T2[NB_TRIANGLE_MAX][3];
  DBL T3[NB_TRIANGLE_MAX][3];
} POLYGONE;

// ---------------------------------------------------------- CAMERA.C

extern struct Oeil_Camera Camera[NB_CAMERA_MAX+1];
extern DBL Longitude;
extern DBL Latitude,Focale;
extern int NbCamera,NumCam;

void xyz_camera(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2);
void new_camera(void);
void affiche_camera(int N,byte Vue,byte Mode);
CibleOeil selection_camera(void );
void deplace_camera(void );
void max_min_camera(byte Vue);
void calcule_vision(int N);
void profondeur_champ(int N);
void modif_ouverture(int N);
void couleur_camera(int N);
int  incr_NbCamera(byte Val);
void cache_camera(byte Mode,byte D,byte F);
void supprime_camera(int NumCamera);
byte pas_camera(byte Son);
DBL  calcule_fov(int N);
void setup_camera(void);
DBL  distance_fog(DBL Distance);
void place_camera(void);
void roll_camera(int N);
void pan_rotate_camera(byte Pan);

// ---------------------------------------------------------- LUMIERE.C

#define MIN_TAILLE_LUMIERE 1
#define MAX_TAILLE_LUMIERE 52

int travail_lumiere(byte Travail);
byte pas_lumiere(byte Son);
void setup_lumiere(void);

// ---------------------------------------------------------- OMNI.C

extern struct Lumiere_Omni Omni[NB_OMNI_MAX+1];
extern byte NbOmni;

void new_omni(void);
void affiche_omni(byte Num,byte Vue,byte Mode);
CibleOeil selection_omni(void);
void deplace_omni(byte NumOmni);
void supprime_omni(byte NumOmni);
void max_min_omni (byte Vue);
byte incr_NbOmni(byte);
void cache_omni(byte Mode,byte D,byte F);
void test_limite_omni(byte N);
byte test_couleur_omni(byte N1,byte N2);
void couleur_omni(byte NumOmni);
void taille_omni(byte NumOmni);
void on_off_omni(byte N);
void ombre_omni(byte N);
void looks_like_omni(void);
byte si_omni_looks_like(int N);
void setup_omni(int N);
void affiche_omni_3d(int N,byte C);

// ---------------------------------------------------------- AREA.C

extern struct Lumiere_Area Area[NB_AREA_MAX+1];
extern byte NbArea;

void new_area(void);
void affiche_area(byte Num,byte Vue,byte Mode);
CibleOeil selection_area(void);
void deplace_area(byte NumArea);
void supprime_area(byte NumArea);
void max_min_area (byte Vue);
byte incr_NbArea(byte);
void cache_area(byte Mode,byte D,byte F);
void test_limite_area(byte N);
byte test_couleur_area(byte N1,byte N2);
void couleur_area(byte NumArea);
void taille_area(byte NumArea);
void rayon_area(byte NA);
void on_off_area(byte N);
void ombre_area(byte N);
void looks_like_area(void);
byte si_area_looks_like(int N);
void setup_area(int N);
void affiche_area_3d(int N,byte C);

// ---------------------------------------------------------- SPOT.C

extern struct Lumiere_Spot Spot[NB_SPOT_MAX+1];
extern byte NbSpot;

void xyz_spot(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2);
void new_spot(void);
void affiche_spot(int N,byte Vue,byte Mode,byte Cone);
CibleOeil selection_spot(void);
void deplace_spot(int Type,int Num);
void max_min_spot(byte Vue);
byte incr_NbSpot(byte Val);
byte test_couleur_spot(byte N1,byte N2);
void cache_spot(byte Mode,byte D,byte F);
void couleur_spot(int NumSpot);
void taille_spot(int NumSpot);
byte test_couleur_spot(byte N1,byte N2);
void supprime_spot(int NumSpot);
void angle_spot(int N,byte Travail);
void on_off_spot(byte N);
void ombre_spot(byte N);
void test_limite_spot(byte N);
void setup_spot(int N);
void affiche_spot_3d(int N,byte C);

// ---------------------------------------------------------- CYLLIGHT.C

extern struct Lumiere_CylLight CylLight[NB_CYLLIGHT_MAX+1];
extern byte NbCylLight;

void xyz_cyllight(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2);
void new_cyllight(void);
void affiche_cyllight(int N,byte Vue,byte Mode,byte Cone);
CibleOeil selection_cyllight(void);
void deplace_cyllight(int Type,int Num);
void max_min_cyllight(byte Vue);
byte incr_NbCylLight(byte Val);
byte test_couleur_cyllight(byte N1,byte N2);
void cache_cyllight(byte Mode,byte D,byte F);
void couleur_cyllight(int NumCylLight);
void taille_cyllight(int NumCylLight);
byte test_couleur_cyllight(byte N1,byte N2);
void supprime_cyllight(int NumCylLight);
void angle_cyllight(int N,byte Travail);
void on_off_cyllight(byte N);
void ombre_cyllight(byte N);
void test_limite_cyllight(byte N);
void setup_cyllight(int N);
void affiche_cyllight_3d(int N,byte C);

// ---------------------------------------------------------- FICHIER.C

#define ARGU_MAX 40

extern char CheminRAYTRACER[MAXPATH];
extern char FichierSCN[MAXFILE+MAXEXT];
extern char CheminSCN[MAXPATH];
extern char CheminLastSCN[MAXPATH];
extern char Argu[ARGU_MAX][128];
extern char ConfigRaytracer[40];
extern byte EXEC1;
extern byte OptSauve;

byte lecture_fichier(char *Nom,byte Selecteur,byte Ajoute,byte Config);
byte sauve_fichier(byte Ecrase,byte Partiel);
byte analyse_ligne (char *TempChar,byte Separateur);
void analyse_arguments(void);
void modif_cfg_raytracer(byte Num,byte Valeur);
int genere_script_raytracer(byte Niveau);
char *selection_fichier(int X,int Y,char *Titre,char *Spec[]);
int sauve_scene_en_quittant(void);
int test_si_nouveau_scn(void);
byte sauve_temporaire(void);
void efface_fichier_shareware(void);

// ---------------------------------------------------------- FONCTION.C

extern byte Fx4;

void trace_cercle(DBL X,DBL Y,DBL S,byte Mode);
int bouton_zone(byte Redraw);
int bouton_recentre(void);
void reset_min_max(void);
int bouton_nouveau(byte Question);
byte chercher_menu(byte Forme);
void appel_programme_externe(byte N);
int test_quitter(void);
int bouton_deplacement(void);
void bouton_couleur(void);
byte creation_tore (void);
byte creation_anneau_tube_disque(byte Lequel);
byte pas_objet(byte Son);
int plus_moins(byte Operation,byte Redraw);
DBL selecteur_cercle (DBL X,DBL Y,DBL Vmax,DBL Vini,byte Efface);
byte dialog_raytracer(void);
byte risque_erreur(void);
void show_logo(void);
void bouton_resolution(char *NewGraphic,byte Fenetre);
byte renomme_scene(char *Titre);
int bouton_fenetre(byte F);
byte couleur_interface (void);
void message_termine(int Sec,char *Commentaires,...);
void couleur_fond_image(void);
void surface_revolution(void);
void creation_couleur(byte *R,byte *V,byte *B,char *Titre);
void trace_rectangle(DBL XA,DBL YA,DBL XB,DBL YB,byte Croix,byte Ligne,byte C3D);
int bouton_rafraichit(void);
void statistique_scene(void);
void modifie_maillage(void);
void trace_croix (DBL XB,DBL YB,byte Valeur);
byte message_shareware(void);
void edit_cone(int N);
void edit_tore(int N);

// ---------------------------------------------------------- MENU.C

extern int DSM;

void efface_boutons(int Y);
void coche_bouton_menu(byte N);
void efface_bouton(int Y);
void choix_principal(void);
void modif_echelle(byte NF,DBL Valeur,byte Operation,byte Redraw);

// ---------------------------------------------------------- DISPLAY.C

extern byte NF;
extern byte VUE_CUBE;
extern byte GrilleType;
extern byte DrawNF;
extern DBL Snap;

byte trace_volume_3(int Num);
void trace_volume_0 (int Num);
void trace_volume_1 (int Num);
void trace_volume_2 (int Num);
void trace_boite(byte Fenetre1,byte Fenetre2);
void redessine_fenetre(byte Val,byte Efface);
byte type_couleur(byte Type);
byte trouve_fenetre(byte);
int trouve_volume(byte F1,byte F2,byte Freeze);
void trace_axes(byte Vue);
void trace_volume_all(int,int);
void trace_grille(byte Vue);
byte cherche_fenetre(void);
void affiche_objet(byte Type,int D,int F);
void zoom_rect(byte NF,byte PM);
void zoom_rect_fenetre(int X1,int Y1,int X2,int Y2);
void select_vue(byte Vue,byte Clip);
void options_fenetres(void);
void rescale_fenetres(void);
void trace_coord(int NF);
void valeur_snap(int NF,int N1,int N2,DBL *Pas,DBL *S,DBL Snap);
void drawing_aids(int NF);
void hide_show_objet(int N,byte Travail);

// ---------------------------------------------------------- OBJET.C

extern VOLUME *Objet[NB_OBJET_MAX+1];
extern struct Noeud Point[NB_TRIANGLE_MAX*9];
extern int NumObjet,NbObjet;

byte cherche_volume_0(int Num,byte Fonction);
byte cherche_volume_1(int Num,byte Fonction);
byte cherche_volume_2(int Num,byte Fonction);
byte cherche_volume_3(int Num,byte Fonction);
void rotationX (int Num,DBL AngleX);
void rotationY (int Num,DBL AngleY);
void rotationZ (int Num,DBL AngleZ);
void copie_objet(void);
void supprime_objet(void);
int  new_objet(byte Type,byte Affiche);
void charge_xyz(int Num,DBL X,DBL Y,DBL Z,byte Val,int Couleur);
void translation(byte Select,int NumObjet);
void rotation(byte Select,int NumObjet);
void deformation2D(byte Select,int NumObjet);
void deformation3D(byte Select,int NumObjet);
void modif_objet(int Num,DBL X,DBL Y,DBL Z,byte Modif);
void efface_objet(int N,byte Affiche);
void analyse_rotation(int i);
byte incr_NbObjet(byte);
void recentre_objet(void);
void reinitialisation_objet(void);
byte alloc_mem_objet(int Num);
byte free_mem_objet(int Num);
void couleur_objet(int N);
void cache_objet(byte Type);
byte creer_nom_objet(int N);
int nom_objet_existe(char *NomObjet,int NumeroObjet);
int renomme_objet (byte Lequel,int N);
void reinit_buffer_objet(void);
int choix_nom_objet(int XG,int YG);
void lisse_objet(void);
void alignement_objet(void);
void miroir_objet(byte Select,int NumObjet);
void auto_copy_objet(int N,byte NomAuto);

// ---------------------------------------------------------- DONNEES1.C

int data_objet(byte Type,int NumeroObjet,byte Vue);

// ---------------------------------------------------------- DONNEES2.C

int data_plan(byte Type,int NumeroObjet,byte Vue);

// ---------------------------------------------------------- DONNEES3.C

extern int MailleTore;

int calcule_tore(int N);
int calcule_quart_tore(int N);
int calcule_tube(int N);
int calcule_cone_tronque(int N);
int calcule_quart_tube(int N);
int calcule_anneau(int N);
int calcule_spline(int N,byte Vue);
int calcule_special(int N);
int calcule_extrude(int N);
int calcule_spot(int N);
int calcule_patch_bezier(int N);

// ---------------------------------------------------------- SOURIS.C

extern byte CURSEUR;
extern byte OptVSouris;
extern byte XHotSpot;
extern byte YHotSpot;
extern byte BitMouse;
extern byte MSMouse;
extern byte Sens;

void graphic_mouse(void);
void init_graphic_mouse (int X,int Y);
void place_mouse(int X,int Y);
void forme_mouse (byte Type);
int gmx_v(void);
int gmy_v(void);
int gmx_r(void);
int gmy_r(void);
void GMouseOn(void);
void GMouseOff(void);
void Sens_Souris(void);
void put_curseur(int X,int Y);
void get_curseur(int X,int Y);
void dessine_souris (byte Operation);
void reset_mouse(void);
int g_mousex_r (void);
int g_mousey_r (void);
void vitesse_souris(DBL Facteur);

// ---------------------------------------------------------- MATIERE.C

#define NB_MAX_MATIERE 1000

extern char CheminMTEX[MAXPATH];
extern char MatiereCourante[MAXPATH];
extern char MatiereCouranteGif[MAXPATH];
extern char *NomMat[NB_MAX_MATIERE];
extern char *NomGif[NB_MAX_MATIERE];
extern byte AnalyseTexture;

void lecture_matiere (void);
void give_me_your_matiere (void);
void assigne_matiere (void);
byte retourne_include(char* Include);
int analyse_matiere_scene(int Travail);
byte modif_matiere(byte Travail);
byte modif_parametre_texture(int N);
int genere_include_matiere(void);
void setup_texture(void);
void nom_gif_matiere_courante(void);

// ---------------------------------------------------------- PALETTE.C

extern byte Palette[256][3];

void fading_palette(byte Fade);
void get_palette(void);
void palette_noire(void);
void init_palette(void);
void put_palette(void);
void modif_entree_palette(byte Num,byte R,byte V,byte B);

// ---------------------------------------------------------- POVRAY.C

extern byte OptVideo;

void outl(FILE *Fichier,int T,char *String,...);
void sauve_config_raytracer(FILE *Fichier,byte Travail);
void lecture_config_raytracer(char *Buffer);
int bouton_render (void);
int bouton_selection (void);
int bouton_camera(void);
int bouton_lumiere(void);
int bouton_matiere(void);
int bouton_modifie (void);
int bouton_affiche (void);
int execute_barre(void);
byte init_barre(void);
int menu_csg(void);
byte dialog_povray(void);
int render_povray(byte Niveau,byte BDialog);
int genere_povray (byte Niveau);
void sauve_config_raytracer(FILE *Fichier,byte Travail);
void lecture_config_raytracer(char *Buffer);
int bouton_selection (void);
int bouton_render (void);
int bouton_camera(void);
int bouton_lumiere(void);
int bouton_matiere(void);
int bouton_modifie (void);
int bouton_affiche (void);
int execute_barre(void);
byte init_barre(void);
void options_video(void);
void set_indentation_output(void);

// ---------------------------------------------------------- TRIANGLE.C

extern POLYGONE *Poly[NB_POLY_MAX];
extern int NbPoly;

int data_triangle (int Num);
int new_volume_polygone(void);
void alloc_mem_triangle(DBL TX1,DBL TY1,DBL TZ1,
                        DBL TX2,DBL TY2,DBL TZ2,
                        DBL TX3,DBL TY3,DBL TZ3,
                        int Compteur);
void sauve_triangle(FILE *Fichier,int i);
int lecture_triangle_raw(char *NomFichierRaw,byte NouveauObj);
byte lecture_triangle_scn(FILE *Fichier,char *MarqueObjet);
void free_mem_poly(int Num);
void free_mem_poly_objet(int Num);
void adapte_polygone(int Num,int NbObjet,byte Force);
void reinit_buffer_poly(void);
byte lisse_triangle(int NumPoly,FILE *Fichier,byte Niveau,int N);

// ---------------------------------------------------------- LISSE.C

#define EPSILON 1.0e-5
#define HASHSIZE  1000          // Size of hash table for vertex lookup
#define DEGEN_TOL (1e-8)        // DBL comparison tolerance for checking
                                // for degenerate triangles
#ifndef MAXFLOAT
#define MAXFLOAT (1e37)
#endif

typedef struct {
  DBL red;
  DBL green;
  DBL blue;
} _Palette;

typedef char *Texture;

typedef struct {
  unsigned vert[3];
  unsigned text_index;
  char     text_type;
  char     flag;
} _Triangle;

// Linked list of triangles

typedef struct TList {
  _Triangle     *tri;
  struct TList *next;
} TriList;


// Double linked list of triangles

typedef struct TList2 {
  _Triangle      *tri;
  struct TList2 *prev;
  struct TList2 *next;
} TriList2;


// List of triangle vertices

typedef struct VList {
  unsigned     vert;
  struct VList *next;
} VertList;

// List of triangle groups

typedef struct GTree {
  TriList2     *index[3];    /* _Triangles indexed by x,y,and z coord */
  Vecteur       vmin;         /* min/max extents of triangle vertices */
  Vecteur       vmax;         /*    "       "     "     "        "     */
  DBL          area;         /* Total surface area of bounding region */
  unsigned     obj_cnt;      /* Total number of triangles in group */
  int          child_cnt;    /* Number of children */
  int          split_cnt;    /* Number of times this node has been split */
  struct GTree *parent;      /* Parent of this node */
  struct GTree *next;        /* Next node at this level */
  struct GTree *child;       /* First child of this ndoe */
} GroupTree;



int  opt_add_tri (DBL ax,DBL ay,DBL az,DBL bx,DBL by,DBL bz,DBL cx,DBL cy,DBL cz);
void opt_write_pov (FILE *out_file,byte Niveau,int N);
void opt_write_file (FILE *out_file,byte Niveau,int N);
void opt_get_limits (DBL *min_x,DBL *min_y,DBL *min_z,DBL *max_x,DBL *max_y,DBL *max_z);
void opt_get_glimits (DBL *min_x,DBL *min_y,DBL *min_z,DBL *max_x,DBL *max_y,DBL *max_z);
unsigned opt_get_vert_cnt();
unsigned opt_get_tri_cnt();
DBL opt_get_index();
unsigned opt_get_bounds();
void init_object(void);
void cleanup_object(void);
DBL calc_tpr (GroupTree *gnode);
GroupTree *create_group();
void delete_tree (GroupTree *gnode);
void optimize_tree (GroupTree *gnode);
void test_split (GroupTree *gnode,int axis,DBL *best_rtpr,TriList2 **best_loc);
void split_group(GroupTree *gnode,int axis,TriList2 *split_loc,GroupTree **group_a,GroupTree **group_b);
void write_file(FILE *f,byte Niveau,int N);
void write_pov20_tree(FILE *f,GroupTree *gnode,int level,byte Niveau,int N);
void write_pov20_transform (FILE *f,MATRIX matrix);
void write_pov20_triangle (FILE *f,_Triangle *tri,int one_texture,byte Niveau,int N);
void write_pov20_bound (FILE *f,GroupTree *gnode);
void update_node (GroupTree *gnode);
void quick_sort (TriList2 *start,TriList2 *end,int axis,int n);
void sort_indexes (GroupTree *gnode);
DBL surf_area (DBL a,DBL b,DBL c);
DBL max_vertex (_Triangle *tri,int axis);
DBL min_vertex (_Triangle *tri,int axis);
DBL avg_vertex (_Triangle *tri,int axis);
void build_tri_index();
void dump_tri_index();
void vert_normal (_Triangle *t,Vecteur *norm);
void tri_normal (_Triangle *t,Vecteur normal);
unsigned pal_lookup (DBL red,DBL green,DBL blue);
unsigned texture_lookup (char *texture_name);
unsigned vert_lookup (DBL x,DBL y,DBL z);
int degen_tri (DBL ax,DBL ay,DBL az,DBL bx,DBL by,DBL bz,DBL cx,DBL cy,DBL cz);
void abortmsg (char *msg,int exit_code);
DBL fmin (DBL a,DBL b);
DBL fmax (DBL a,DBL b);
void add_ext (char *fname,char *ext,int force);
void cleanup_name (char *name);

// ---------------------------------------------------------- PREFEREN.C

extern int  NbRendu;
extern byte OptBeep;
extern byte OptLabScn;
extern byte OptAxe;
extern byte OptAide;
extern byte OptGrille;
extern byte OptBtD;
extern byte MSMenu;
extern byte OptSnap;
extern byte OptCoord;
extern byte SuiviSelection;
extern byte PalRenduRapide;
extern byte ShowCtrlPt;
extern char CheminPOVSCN[MAXPATH];
extern char CheminIMAGE[MAXPATH];
extern char CheminVIEWERI[MAXPATH];
extern char CheminEDITEUR[MAXPATH];
extern char CheminVIEWERS[MAXPATH];
extern char POVExeName[MAXPATH];
extern byte POVForWindows;
extern char CheminUserINC[MAXPATH];
extern byte DEBUG;
extern byte GenerateHidden;

void lecture_config_interface(void);
void sauve_config_interface(byte Travail);
byte fenetre_preferences(void);
byte fenetre_systeme(void);
byte gestion_memoire(void);
void menu_souris(byte Bool);

// ---------------------------------------------------------- BARRE.C

#define NbMenu 7

typedef struct texte_barre {
  char Texte[15];
  int Debut;
  int Fin;
  int Nb;
  char Choix[25][20];
  byte Case[25][2];
  char Aide[25][50];
  byte Enable[25];
};

extern struct texte_barre Menu[NbMenu];

int barre_menu(void);
int test_barre_menu(byte Forme,byte Retour);
void affiche_barre_menu(void);
void forme_barre(byte MN,byte BN,char *Texte,byte Case,byte OnOff,byte Enable,char *Aide);

// ---------------------------------------------------------- GIF.C

extern byte PaletteGIF[256][3];

void doextension(FILE *fp);
byte unpackimage(FILE *fp,char *buffer,int bits,int flags);
byte unpackgif(FILE *fp,char *p,byte JusteXY);
byte decompresse_gif(int *X,int *Y,char *Fichier,char *p,byte JusteXY);
byte affiche_gif(int PosX,int PosY,int X,int Y,char *Image,byte Prepare,byte Display,byte Pas);

// ---------------------------------------------------------- INTERFA.C

extern int XMotif,YMotif;
extern char *ImageMotif;
extern byte MotifOk;
extern char CheminMotif[MAXPATH];
extern byte OptFade;
extern int XDebutAide;

extern int XMenuD,YMenuD;
extern int XMenuF,YMenuF;
extern int CentX,CentY;
extern int BarreD,BarreF;

void init_video_structure(void);
void a_propos_de(void);
void interface(byte ShowPal);
void affiche_donnees(void);
void affiche_bureau(void);
void init_vues(byte Draw);
void show_logo_logiciel(void);
void charge_motif(byte Travail);
byte choix_motif(void);
void affiche_boutons_principaux(void);
void affiche_boutons_graphique(void);
void efface_barre_bouton_bas(int N);

// ---------------------------------------------------------- LANGUE.C

extern char *RES_COPY[50];
extern char *RES_MENU[100];
extern char *RES_BOUT[50];
extern char *RES_AIDE[200];
extern char *RES_MESS[100];

// ---------------------------------------------------------- EXTRUDE.C

#define NB_POINT_MAX 500

typedef struct Chemin {
  DBL X[NB_POINT_MAX];
  DBL Y[NB_POINT_MAX];
};

extern struct Chemin Pt;
extern int NbVertex;

void extrusion(void);
void affiche_boutons_extrusion(void);
int creer_point(byte Spline);
void inserer_point(int i,DBL X,DBL Y,byte Spline);
void supprimer_point(byte Spline);
void ajouter_point(byte Spline);
void modifier_point(byte Spline);
void redessine_forme(void);
int test_point(void);
int test_segment(int BSpline);
void enregistre_prism_pov(FILE *Fichier,int N,int L);

// ---------------------------------------------------------- ICONE.C

void affiche_icone(int X,int Y,byte Mode,char *Icone);
extern char AppareilPhoto[225];
extern char Oeil[466];
extern char Disquette[404];
extern char PasDeTexture[10004];
extern char LogoChroma[2884];
extern char IconeRafraichit[260];
extern char POVLAB[1551];
extern char ScaleObjet[365];
extern char RotateObjet[365];
extern char TranslateObjet[328];
extern char IconeInterrogation[365];

// ---------------------------------------------------------- ICONE2.C

extern char Logo[35404];

// ---------------------------------------------------------- RENDER.C

extern byte DernierRendu;
extern byte Fond_RVB[3];
extern char LastImage[MAXPATH];
extern struct Brouillard Fog[5];

extern byte Global_Ambient[3];
extern DBL  Global_ADC;
extern DBL  Global_A_Gamma;
extern DBL  Global_Irid[3];
extern int  Global_MaxTrace;
extern int  Global_MaxInter;
extern int  Global_NbWave;

extern byte Atmos_OnOff;
extern byte Atmos_Type;
extern DBL  Atmos_Dist;
extern DBL  Atmos_Scatt;
extern DBL  Atmos_Eccent;
extern byte Atmos_Samples;
extern DBL  Atmos_Jitter;
extern DBL  Atmos_aa_t;
extern byte Atmos_aa_l;
extern byte Atmos_RGB[3];

extern DBL AntiAliasValue;
extern DBL JitterValue;
extern int AntiAliasDepth;

extern int ResolutionX;
extern int ResolutionY;

extern byte Rad_OnOff;
extern byte Rad_Type;
extern DBL  Rad_Brightness;
extern int  Rad_Count;
extern DBL  Rad_Dist_Max;
extern DBL  Rad_Err_Bound;
extern DBL  Rad_Gray_Threshold;
extern DBL  Rad_Low_Err_Fact;
extern DBL  Rad_Min_Reuse;
extern int  Rad_Near_Count;
extern int  Rad_Rec_Lim;

int lance_calcul(byte Niveau,byte BDialog);
byte dialog_raytracer(void);
byte lance_rendu_rapide(byte Bool);
void palette_rendu_rapide(void);
void setup_render(void);
void parametre_atmosphere(void);
void parametre_brouillard(void);
void init_fog(void);

// ---------------------------------------------------------- VECTEUR.C

void vect_init (Vecteur v,DBL x,DBL y,DBL z);
void vect_copy (Vecteur v1,Vecteur v2);
int  vect_equal (Vecteur v1,Vecteur v2);
void vect_add (Vecteur v1,Vecteur v2,Vecteur v3);
void vect_sub (Vecteur v1,Vecteur v2,Vecteur v3);
void vect_scale (Vecteur v1,Vecteur v2,DBL k);
DBL  vect_mag (Vecteur v);
void vect_normalize (Vecteur v);
DBL  vect_dot (Vecteur v1,Vecteur v2);
void vect_cross (Vecteur v1,Vecteur v2,Vecteur v3);
void vect_min (Vecteur v1,Vecteur v2,Vecteur v3);
void vect_max (Vecteur v1,Vecteur v2,Vecteur v3);
DBL  vect_angle (Vecteur v1,Vecteur v2);
void vect_print (FILE *f,Vecteur v,int dec,char sep);
void vect_rotate (Vecteur v1,Vecteur v2,int axis,DBL angle);
void vect_axis_rotate (Vecteur v1,Vecteur v2,Vecteur axis,DBL angle);
void vect_transform (Vecteur v1,Vecteur v2,MATRIX mat);

// ---------------------------------------------------------- BLOB.C

extern int GroupeEnCours;
extern DBL SeuilEnCours;
extern DBL ForceEnCours;

void erreur_2D_blob(void);
int quel_groupe(DBL S);
DBL quel_seuil(int G);
void force_seuil_groupe_blob(byte Cherche,int N);
int groupe_suivant(int N);

// ---------------------------------------------------------- ATTRIBUT.C

void attributs_objet(void);
int panel_srt(byte Type);
void change_attributs_selection(void);

// ---------------------------------------------------------- SELECT.C

extern int Selection;
extern byte OkSelect;

void selection_objet(byte Type);
int cube_selection(void);
void selection_modif_objet(byte Modif);
void couleur_selection(void);
byte selection_objet_par_matiere(byte Travail);
int selection_zone (void);
void utilise_selection(byte travail);
byte test_si_selection_et_coche(void);

// ---------------------------------------------------------- CSG.C

byte csg_objet(byte T);
void analyse_csg_partiel(int N);
void analyse_delete_csg_objet(int N);
void affiche_csg(byte T);
void inverse_csg(void);

// ---------------------------------------------------------- OUTIL.C

byte nom_objet_auto(void);
void duplique_rotation(void);
void alignement_objet(void);
void alignement(void);

// ---------------------------------------------------------- POLICE.C

extern char CheminTTF[MAXPATH];

void genere_police_3D(void);

// ---------------------------------------------------------- HFIELD.C

extern int MailleHF;

byte lecture_hfield(char *Nom,byte NewObj);
void water_level(int N);

// ---------------------------------------------------------- TARGA.C

typedef struct {
  unsigned short Red, Green, Blue, Filter;
} IMAGE_COLOUR;

typedef struct Image_Line {
  byte *red, *green, *blue;
} IMAGE_LINE;

typedef struct Image_Struct {
  DBL width, height;
  int iwidth, iheight;
  short Colour_Map_Size;
  IMAGE_COLOUR *Colour_Map;
  union {
    IMAGE_LINE *rgb_lines;
    byte **map_lines;
  } data;
} IMAGE;

byte lecture_targa(IMAGE *Image,char *Name);

// ---------------------------------------------------------- PLUGINS.C

byte execute_plugins(char *PLG);
byte choix_plugins(void);

// ---------------------------------------------------------- CLAVIER.C

void affiche_aide_clavier(void);
int test_entree_clavier(void);

// ---------------------------------------------------------- DITHER.C

extern byte Dither;

byte voir_image(byte Laquelle);
int get_pix(int x,int y);
int get_pix_fs(int x,int y);
int dither(int X,int Y);
void modifie_dithering(void);

// ---------------------------------------------------------- RGB&HSL.C

void RGB_to_HSL  (DBL r,DBL g,DBL b,DBL *h,DBL *s,DBL *l);
void HSL_to_RGB(DBL h,DBL sl,DBL l,DBL *r,DBL *g,DBL *b);

// ---------------------------------------------------------- VERSION.C

extern char LOGICIEL[];
extern char VERSION[];
extern char VERSION_MODELEUR[];

// ---------------------------------------------------------- SPLINE.C

#define NB_POINT_SPLINE_MAX 100

typedef struct  {
  byte Buffer;
  int Nombre;
  byte Type;
  byte Degree;
  byte Subdi;
  DBL rp[NB_POINT_SPLINE_MAX*6][4];
  DBL dots[NB_POINT_SPLINE_MAX][4];
} B_SPLINE;

extern B_SPLINE *Spline[NB_SPLINE_MAX];
extern int NbSpline;

int creation_spline(int N);
void free_mem_spline_objet(int Num);
void free_mem_spline(int Num);
void spline(int n,int m,int t,int NSP);
byte move_point_spline(void);
byte delete_point_spline(void);
byte add_point_spline(void);
void sauve_spline(FILE *Fichier,int i);
byte lecture_spline_scn(FILE *Fichier,char *MarqueObjet);
void reinit_buffer_spline(void);
void generate_output_spline(int N,FILE *Fichier);
void edit_vertex_spline(void);
void parametre_spline(void);
int new_volume_spline(void);
int copie_spline(int S,int C);
void smooth_spline(void);



// ---------------------------------------------------------- MAPPING.C

extern char Library[10][MAXPATH];

int set_mapping(int N);
void ecrit_mapping(FILE *Fichier,int L,int N);
void ecrit_bumping(FILE *Fichier,int L,int N);
void extract_xy_mapping(char *NomFichier,int *X,int *Y);
void library_mapping(void);
byte verifie_mapping(void);

// ---------------------------------------------------------- CLIP.C

int do_clip(int *XL1,int *YL1,int *XL2,int *YL2,int XR1,int YR1,int XR2,int YR2);

// ---------------------------------------------------------- BINFILE.C

byte sauve_fichier_bin(byte Ecrase,byte Partiel);

// ---------------------------------------------------------- OBJET2.C

void lecture_objet_externe(int Type);

// ---------------------------------------------------------- SPECIAL.C

VECTOR point_chaine(int N,int Nb);
byte lecture_special_scn(FILE *Fichier,char *MarqueObjet,int N);
void sauve_special(FILE *Fichier,int N);
void enregistre_lathe_pov(FILE *Fichier,int N,int L);
void import_lathe(void);
void free_mem_special(int N);
void change_point_chaine(int N,int Num,VECTOR V);

// ---------------------------------------------------------- AUTOMAP.C

void lecture_automap(void);

// ---------------------------------------------------------- BEZIER.C

typedef struct PatchStructRec {
  VECTOR Cp[17];                   // the control-points 1-16 (no [0] !!!)
  VECTOR R[101];                   // the resulting vertices for the poly-mesh
} B_PATCH;                         // also: no [0] !!!

void import_bezier(void);
void ComputePolyMesh(B_PATCH *Wk);
void ComputeBernstein(void);
void adapte_bezier(int N);
void enregistre_bezier_patch_pov(FILE *Fichier,int N,int L);

// ---------------------------------------------------------- FLARE.C

void write_lens_flare_code(FILE *File,DBL XL,DBL YL,DBL ZL,char *Flare);
void flare_light(char *Light,int Bouton);

#endif
#define GLOBAL_H_INCLUDED
#endif

