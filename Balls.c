#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 400
#define TITLE "Balls and their admirers"
#define BALL_COUNT 100
#define FPS 60
#define VEL_MAX 5
#define RADIUS_MAX 20
#define RADIUS_MIN 5

// Farvepalet
static Color COLORS[] = {
    LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
    RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
    DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN,
};
#define COLORS_COUNT (int)(sizeof(COLORS)/sizeof(COLORS[0]))

// Definition af Ball
typedef struct Ball {
    int posx, posy;
    int velx, vely;
    int radius;
    Color color;
    struct Ball* follows; // peger på den bold, denne bold følger
} Ball;

static Ball balls[BALL_COUNT];

// Hjælp: tilfældig hastighed i [-VEL_MAX, VEL_MAX], undgå 0 hvis muligt
static int RandVel(void) {
    int v = GetRandomValue(-VEL_MAX, VEL_MAX);
    if (v == 0) v = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    return v;
}

// Initialisér én bold (uden at sætte 'follows' endnu)
static Ball* init_ball_random_basic(Ball* p) {
    p->posx = GetRandomValue(0, WIDTH - 1);
    p->posy = GetRandomValue(0, HEIGHT - 1);
    p->velx = RandVel();
    p->vely = RandVel();
    p->radius = GetRandomValue(RADIUS_MIN, RADIUS_MAX);
    p->color = COLORS[GetRandomValue(0, COLORS_COUNT - 1)];
    p->follows = NULL;
    return p;
}

// Initialiser alle bolde og tildel derefter en leder til hver
static void init_balls_random(void) {
    // Grunddata
    for (int i = 0; i < BALL_COUNT; ++i) {
        init_ball_random_basic(&balls[i]);
    }

    // Tildel 'follows' (sørg for ikke at pege på sig selv)
    for (int i = 0; i < BALL_COUNT; ++i) {
        int j = i;
        while (j == i) {
            j = GetRandomValue(0, BALL_COUNT - 1);
        }
        balls[i].follows = &balls[j];
    }
}

static Ball* draw_ball(Ball* p) {
    DrawCircle(p->posx, p->posy, (float)p->radius, p->color);
    return p;
}

// Opdater position med wrap-around
static Ball* update_pos(Ball* p) {
    p->posx = (WIDTH + p->posx + p->velx) % WIDTH;
    p->posy = (HEIGHT + p->posy + p->vely) % HEIGHT;
    return p;
}

// Retning mod lederen (enkel “følg”-adfærd)
static Ball* update_vel_for_following(Ball* p) {
    if (!p->follows) return p;
    int errx = p->follows->posx - p->posx;
    int erry = p->follows->posy - p->posy;
    p->velx = (errx == 0) ? 0 : (errx > 0 ? 1 : -1);
    p->vely = (erry == 0) ? 0 : (erry > 0 ? 1 : -1);
    return p;
}

static void update_and_draw_all(void) {
    // Opdater først alle hastigheder og positioner
    for (int i = 0; i < BALL_COUNT; ++i) {
        update_vel_for_following(&balls[i]);
    }
    for (int i = 0; i < BALL_COUNT; ++i) {
        update_pos(&balls[i]);
    }
    // Tegn
    for (int i = 0; i < BALL_COUNT; ++i) {
        draw_ball(&balls[i]);
    }
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    // (Valgfrit) seed – raylib har egen RNG; dette er kun hvis du bruger rand()
    // SetRandomSeed(GetTime()); // findes i nyere raylib; ellers udelad

    init_balls_random();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);      // Ryd baggrunden FØR tegning
        update_and_draw_all();          // Opdater + tegn
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
