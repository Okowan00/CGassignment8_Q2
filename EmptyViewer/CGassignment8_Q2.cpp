#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "load_mesh.cpp"
#include "frame_timer.cpp"

float eye[3] = { 0, 0, 0 };
int nx = 1280, ny = 1280;

GLuint vao, vbo_pos, vbo_nor, ebo;

void set_viewport() {
    glViewport(0, 0, nx, ny);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void set_light() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    float global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glShadeModel(GL_SMOOTH);

    float light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float light_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float light_position[] = { -1.0f, -1.0f, -1.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setup_vertex_array() {
    std::vector<float> positions, normals;
    std::vector<unsigned int> indices;

    for (auto& v : gPositions) {
        positions.push_back(v.x);
        positions.push_back(v.y);
        positions.push_back(v.z);
    }

    for (auto& n : gNormals) {
        float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        float x = -n.x / len, y = -n.y / len, z = -n.z / len;
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
    }

    for (auto& t : gTriangles) {
        indices.push_back(t.indices[0]);
        indices.push_back(t.indices[1]);
        indices.push_back(t.indices[2]);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glGenBuffers(1, &vbo_nor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_nor);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void draw_bunny_vertex_array() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, gTriangles.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void my_display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], 0, 0, -1, 0, 1, 0);
    glTranslatef(0.1f, -1.0f, -1.5f);
    glScalef(10.0f, 10.0f, 10.0f);

    start_timing();
    draw_bunny_vertex_array();
    float timeElapsed = stop_timing();

    gTotalFrames++;
    gTotalTimeElapsed += timeElapsed;
    float fps = gTotalFrames / gTotalTimeElapsed;
    char string[1024] = { 0 };
    sprintf(string, "OpenGL Bunny: %0.2f FPS", fps);
    glutSetWindowTitle(string);

    glutSwapBuffers();
    glutPostRedisplay();
}

void initialize() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    set_viewport();
    set_light();
    load_mesh("bunny.obj");
    setup_vertex_array();
    init_timer();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(nx, ny);
    glutCreateWindow("OpenGL Bunny");
    glewInit();
    initialize();
    glutDisplayFunc(my_display);
    glutMainLoop();
    return 0;
}
