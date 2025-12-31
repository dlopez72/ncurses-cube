#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

typedef struct {
    float x, y, z;
} vector3;

const float fov = 60;
int rows, cols;

vector3 vertices[8] = {
    {1, 1, 1}, {1, 1, -1}, {1, -1, 1}, {1, -1, -1},
    {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1}
};

int edges[12][2] = {
    {0, 1}, {0, 2}, {2, 3}, {1, 3},
    {4, 5}, {4, 6}, {6, 7}, {5, 7},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

float angle = 0;

void render_cube(float angle);
void draw_line(int x0, int y0, int x1, int y1);

int main(void) {
    initscr();
    getmaxyx(stdscr, rows, cols);
    raw();
    noecho();
    curs_set(0);
    timeout(50);

    while (getch() == ERR) {
        render_cube(angle);
        angle += 0.05;
    }

    endwin();
    return 0;
}

void render_cube(float angle) {
    clear();

    float rot_x, rot_z;
    float z_depth;
    float proj_x, proj_y;
    int proj_points[8][2];
    
    float scale = (rows < cols ? rows : cols) / 3.0f; 

    for (int i = 0; i < 8; i++) {
        rot_x = vertices[i].x * cosf(angle) - vertices[i].z * sinf(angle);
        rot_z = vertices[i].x * sinf(angle) + vertices[i].z * cosf(angle);

        z_depth = rot_z + 2.5;

        proj_x = rot_x / z_depth;
        proj_y = vertices[i].y / z_depth;

        proj_points[i][0] = (cols / 2) + (int)(proj_x * scale * 2); // *2 for aspect ratio (chars are skinny)
        proj_points[i][1] = (rows / 2) + (int)(proj_y * scale);
    }

    for (int i = 0; i < 12; i++) {
        int p1 = edges[i][0];
        int p2 = edges[i][1];
        draw_line(proj_points[p1][0], proj_points[p1][1],
                  proj_points[p2][0], proj_points[p2][1]);
    }
    refresh();
}

void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1) {
        if (x0 >= 0 && x0 < cols && y0 >= 0 && y0 < rows) {
            mvaddch(y0, x0, '@');
        }
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * error;
        
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }

    return;
}
