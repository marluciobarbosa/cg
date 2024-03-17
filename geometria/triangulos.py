from OpenGL.GL import *
from OpenGL.GLUT import *

vertex_shader = """
#version 120
attribute vec3 coord3d;
void main(void) {
  gl_Position = vec4(coord3d, 1.0);
}
"""

fragment_shader = """
#version 120
uniform vec4 color;
void main(void) {
  gl_FragColor = color;
}
"""

shaderProgram = None
attribute_coord3d = None
window = None  # Definindo a variável global window

def onDisplay():
    global window  # Adicionando a declaração global
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glUseProgram(shaderProgram)
    glEnableVertexAttribArray(attribute_coord3d)
    
    # Define os vértices em 3D e as cores para cada triângulo
    vertices = [
        -1.0, -1.0, 0.0,  # Triângulo 1 (abaixo)
         1.0, -1.0, 0.0,
         0.0,  1.0, 0.0,
        -1.0,  1.0, 0.0,  # Triângulo 2 (acima)
         1.0,  1.0, 0.0,
         0.0, -1.0, 0.0,
    ]
    
    red = [1.0, 0.0, 0.0, 1.0]    # Cor para o primeiro triângulo
    green = [0.0, 1.0, 0.0, 1.0]  # Cor para o segundo triângulo

    # Desenha o primeiro triângulo (abaixo) com a cor vermelha
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, red)
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 
                          (GLfloat * len(vertices))(*vertices))
    glDrawArrays(GL_TRIANGLES, 0, 3)

    # Desenha o segundo triângulo (acima) com a cor verde
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, green)
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 
                          (GLfloat * len(vertices[3:]))(*vertices[3:]))

    glDrawArrays(GL_TRIANGLES, 0, 3)

    glDisableVertexAttribArray(attribute_coord3d)
    glutSwapBuffers() #Trocar buffers frontal e traseiro

def free_resources():
    global shaderProgram
    glDeleteProgram(shaderProgram)

def create_shader(source, shader_type):
    shader = glCreateShader(shader_type)
    glShaderSource(shader, source)
    glCompileShader(shader)
    if glGetShaderiv(shader, GL_COMPILE_STATUS) != GL_TRUE:
        print(glGetShaderInfoLog(shader))
        return None
    return shader

def init_resources():
    global shaderProgram, attribute_coord3d
    vs = create_shader(vertex_shader, GL_VERTEX_SHADER)
    fs = create_shader(fragment_shader, GL_FRAGMENT_SHADER)
    shaderProgram = glCreateProgram()
    glAttachShader(shaderProgram, vs)
    glAttachShader(shaderProgram, fs)
    glLinkProgram(shaderProgram)
    
    attribute_name = "coord3d"
    attribute_coord3d = glGetAttribLocation(shaderProgram, attribute_name)
    if attribute_coord3d == -1:
        print(f"Não foi possível vincular o atributo {attribute_name}")
        return

def main():
    glutInit()
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH)
    window = glutCreateWindow("Triângulos OpenGL")
    glEnable(GL_DEPTH_TEST)  # Habilita teste de profundidade para visualização 3D
    init_resources()
    glutDisplayFunc(onDisplay)
    glutMainLoop()
    free_resources()

if __name__ == "__main__":
    main()
