#include <GLUT/glut.h>
#include <iostream>

GLuint shaderProgram;
GLint attribute_coord3d;

const GLchar* vertex_shader =
  "#version 120\n"
  "attribute vec3 coord3d;\n"
  "void main(void) {\n"
  "  gl_Position = vec4(coord3d, 1.0);\n"
  "}\n";

const GLchar* fragment_shader =
  "#version 120\n"
  "uniform vec4 color;\n"
  "void main(void) {\n"
  "  gl_FragColor = color;\n"
  "}\n";

void onDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);
  glEnableVertexAttribArray(attribute_coord3d);
  
  // Define os vértices em 3D e as cores para cada triângulo
  GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f, // Triângulo 1 (abaixo)
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f,  0.0f,
    -1.0f, 1.0f,  0.0f, // Triângulo 2 (acima)
    1.0f, 1.0f,  0.0f,
    0.0f, -1.0f, 0.0f,
  };
  
  GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };   // Cor para o primeiro triângulo
  GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 }; // Cor para o segundo triângulo

  // Desenha o primeiro triângulo (abaixo) com a cor vermelha
  glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, red);
  glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Desenha o segundo triângulo (acima) com a cor verde
  glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, green);
  glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, vertices + 3);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(attribute_coord3d);
  glutSwapBuffers();
}

void free_resources() {
  glDeleteProgram(shaderProgram);
}

GLuint create_shader(const char* source, GLenum type) {
  GLuint res = glCreateShader(type);
  const GLchar* sources[] = {source};
  glShaderSource(res, 1, sources, NULL);
  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    fprintf(stderr, "Erro no sombreamento do vértice\n");
    return 0;
  }
  return res;
}

void init_resources() {
  GLuint vs = create_shader(vertex_shader, GL_VERTEX_SHADER);
  if (vs == 0)
    return;
  GLuint fs = create_shader(fragment_shader, GL_FRAGMENT_SHADER);
  if (fs == 0)
    return;

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);

  const char* attribute_name = "coord3d";
  attribute_coord3d = glGetAttribLocation(shaderProgram, attribute_name);
  if (attribute_coord3d == -1) {
    fprintf(stderr, "Não foi possível vincular o atributo %s\n", attribute_name);
    return;
  }
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
//   glutInitWindowSize(1024, 768);
  glutCreateWindow("Triângulos OpenGL");
  glEnable(GL_DEPTH_TEST); // Habilita teste de profundidade para visualização 3D
  init_resources();
  glutDisplayFunc(onDisplay);
  glutMainLoop();
  free_resources();
  return 0;
}
