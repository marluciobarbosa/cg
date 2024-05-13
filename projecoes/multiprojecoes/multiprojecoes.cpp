// include glfw
#include <GLFW/glfw3.h>

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#define WIDTH 800
#define HEIGHT 600

void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow*  createWindow() {
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Teapot Example", NULL, NULL);
    glfwMakeContextCurrent(window);

    return window;
}

void renderTeapot(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, GLfloat fov) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
    glRotatef(angle, 0, 1, 0);
    glutSolidTeapot(0.5);
}

void render(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Primeiro quadrante: Projeção perspectiva
    glViewport(0, HEIGHT/2, WIDTH/2, HEIGHT/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (GLfloat)WIDTH/(GLfloat)HEIGHT, 1, 100);
    // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    renderTeapot(0, 0, 0, 5, 45);

    // Segundo quadrante: Vista de topo
    glViewport(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2, 2, -2, 2);
    // glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    renderTeapot(90, 0, 5, 0, 0);

    // Terceiro quadrante: Vista de frente
    glViewport(0, 0, WIDTH/2, HEIGHT/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2, 2, -2, 2);
    renderTeapot(0, 0, 0, 0, 0);

    // Quarto quadrante: Vista do lado esquerdo
    glViewport(WIDTH/2, 0, WIDTH/2, HEIGHT/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2, 2, -2, 2);
    renderTeapot(-90, 0, 0, 5, 0);

    glfwSwapBuffers(window);
}

int main() {
    initGLFW();
    GLFWwindow*  window =createWindow();

    while (!glfwWindowShouldClose(glfwGetCurrentContext())) {
        render(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}