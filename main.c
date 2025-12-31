#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

/*
!!! POSITIVE X IS RIGHT, NEGATIVE X IS LEFT
!!! POSITIVE Y IS DOWN, NEGATIVE Y IS UP
!!! POSITIVE Z IS INTO THE SCREEN, NEGATIVE Z IS OUT FROM THE SCREEN
!!! This is right hand rule !!!
 */

typedef struct {
    float x, y, z;
} vector3;

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

int triangles[12][3] = {
    {0, 1, 2}, {1, 2, 3}, {0, 1, 5}, {0, 4, 5},
    {1, 3, 7}, {1, 5, 7}, {2, 3, 7}, {2, 6, 7},
    {0, 2, 6}, {0, 4, 6}, {4, 5, 6}, {5, 6, 7}
};

float y_angle = 0;
float x_angle = 0;
float z_angle = 0;

int draw_wireframe = 0;

void render_cube(float y_angle, float x_angle, float z_angle);
void draw_line(int x0, int y0, int x1, int y1, char c);
void draw_triangle(vector3 triangle[3], char c);
float edge_function(vector3 p0, vector3 p1, vector3 p2);

int main(int argc, char *argv[]) {
    initscr();
    getmaxyx(stdscr, rows, cols);
    raw();
    noecho();
    curs_set(0);

    if ((argc > 1) && (strcmp(argv[1], "-l") == 0)) {
        draw_wireframe = 1;
    }

    timeout(50);

    while (getch() != 'q') {
        render_cube(y_angle, x_angle, z_angle);
        y_angle += 0.05;
        x_angle += 0.05;
    }

    endwin();
    return 0;
}

void render_cube(float y_angle, float x_angle, float z_angle) {
    clear();

    float rot_x, rot_z, rot_y;
    float z_depth;
    float proj_x, proj_y;
    vector3 proj_points[8];
    
    float scale = (rows < cols ? rows : cols) / 3.0f; 

    for (int i = 0; i < 8; i++) {
        // rotated along y axis
        rot_x = vertices[i].x * cosf(y_angle) - vertices[i].z * sinf(y_angle);
        rot_z = vertices[i].x * sinf(y_angle) + vertices[i].z * cosf(y_angle);

        // rotating along x axis
        rot_y = (vertices[i].y * cosf(x_angle)) + (rot_z * sinf(x_angle));
        rot_z = (0 - (vertices[i].y * sinf(x_angle)) + (rot_z * cosf(x_angle)));

        // rotating along z axis
        rot_x = (rot_x * cosf(z_angle)) - (rot_y * sinf(z_angle));
        rot_y = (rot_x * sinf(z_angle)) - (rot_y * cosf(z_angle));

        z_depth = rot_z + 2.5;

        proj_x = rot_x / z_depth;
        proj_y = rot_y / z_depth;

        // proj_points[i][0] = (cols / 2) + (int)(proj_x * scale * 2); // *2 for aspect ratio (chars are skinny)
        // proj_points[i][1] = (rows / 2) + (int)(proj_y * scale);
        proj_points[i].x = ((float)cols / 2) + (proj_x * scale * 2); // *2 for aspect ratio (chars are skinny)
        proj_points[i].y = ((float)rows / 2) + (proj_y * scale);
        proj_points[i].z = z_depth;
    }

    // if (draw_wireframe) {
    //     for (int i = 0; i < 12; i++) {
    //         int p1 = edges[i][0];
    //         int p2 = edges[i][1];
    //         draw_line(proj_points[p1][0], proj_points[p1][1],
    //                   proj_points[p2][0], proj_points[p2][1], (35 + i));
    //     }
    // }
    // else {
    //     for (int i = 0; i < 12; i++) {
    //         int triangle[3][2] = {
    //             {proj_points[triangles[i][0]][0], proj_points[triangles[i][0]][1]},
    //             {proj_points[triangles[i][1]][0], proj_points[triangles[i][1]][1]},
    //             {proj_points[triangles[i][2]][0], proj_points[triangles[i][2]][1]}
    //         };
    //         draw_triangle(triangle, '#');
    //     }
    // }
    for (int i = 0; i < 12; i++) {
        vector3 triangle[3] = {
            proj_points[triangles[i][0]],
            proj_points[triangles[i][1]],
            proj_points[triangles[i][2]],
        };
        draw_triangle(triangle, '#');
    }
    refresh();
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm for reference
void draw_line(int x0, int y0, int x1, int y1, char c) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1) {
        if (x0 >= 0 && x0 < cols && y0 >= 0 && y0 < rows) {
            mvaddch(y0, x0, c);
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

// https://jtsorlinis.github.io/rendering-tutorial/ for reference
void draw_triangle(vector3 triangle[3], char c) {
    float abp, bcp, cap;

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            vector3 point = {i, j, 0};
            abp = edge_function(triangle[0], triangle[1], point);
            bcp = edge_function(triangle[1], triangle[2], point);
            cap = edge_function(triangle[2], triangle[0], point);
            if (edge_function(triangle[0], triangle[1], triangle[2]) >= 0) {
                if (abp > 0 && bcp > 0 && cap > 0) {
                    mvaddch(j, i, c);
                }
            }
            else {
                if (abp < 0 && bcp < 0 && cap < 0) {
                    mvaddch(j, i, c);
                }
            }
        }
    }
    return;
}

float edge_function(vector3 p0, vector3 p1, vector3 p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
}
