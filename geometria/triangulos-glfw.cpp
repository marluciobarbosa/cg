#include <GL/glew.h>                           // Inclui a biblioteca GLEW para lidar com extensões OpenGL de forma portável
#include <GLFW/glfw3.h>                        // Inclui a biblioteca GLFW para criação de janelas e contexto OpenGL
#include <iostream>                            // Inclui a biblioteca para entrada e saída de dados em console

const char* vertex_shader = R"(                // Define uma string contendo o código do shader de vértices
    #version 330 core                         // Declara a versão do OpenGL utilizada
    in vec3 coord3d;                          // Define o atributo de entrada para as coordenadas 3D dos vértices
    void main(void) {                         // Função principal do shader de vértices
        gl_Position = vec4(coord3d, 1.0);     // Define a posição do vértice na cena
    }
)";

const char* fragment_shader = R"(              // Define uma string contendo o código do shader de fragmentos
    #version 330 core                         // Declara a versão do OpenGL utilizada
    uniform vec4 color;                       // Define uma variável uniforme para a cor do fragmento
    out vec4 frag_color;                      // Define a cor de saída do fragmento
    void main(void) {                         // Função principal do shader de fragmentos
        frag_color = color;                   // Define a cor do fragmento como a cor uniforme especificada
    }
)";

GLuint shaderProgram;                         // Variável para armazenar o programa de shader compilado
GLint attribute_coord3d;                      // Localização do atributo de coordenadas 3D nos shaders
GLFWwindow* window;                           // Variável para a janela GLFW
GLuint vao, vbo;                              // Variáveis para os objetos de array de vértices e buffer de vértices

void onDisplay() {                            // Função chamada para renderizar a cena na janela OpenGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Limpa os buffers de cor e profundidade
    glUseProgram(shaderProgram);              // Usa o programa de shader especificado

    glBindVertexArray(vao);                   // Vincula o array de vértices
    glEnableVertexAttribArray(attribute_coord3d);  // Habilita o atributo de coordenadas 3D

    GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Define a cor vermelha
    GLfloat green[] = {0.0f, 1.0f, 0.0f, 1.0f};   // Define a cor verde

    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, red); // Define a cor do primeiro triângulo como vermelho
    glDrawArrays(GL_TRIANGLES, 0, 3);        // Desenha o primeiro triângulo

    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, green); // Define a cor do segundo triângulo como verde
    glDrawArrays(GL_TRIANGLES, 3, 3);        // Desenha o segundo triângulo

    glDisableVertexAttribArray(attribute_coord3d);  // Desabilita o atributo de coordenadas 3D
    glfwSwapBuffers(window);                   // Troca os buffers da janela OpenGL
}

void free_resources() {                       // Função para liberar os recursos alocados na GPU
    glDeleteProgram(shaderProgram);           // Deleta o programa de shader
    glDeleteVertexArrays(1, &vao);            // Deleta o array de vértices
    glDeleteBuffers(1, &vbo);                 // Deleta o buffer de vértices
}

void init_resources() {                       // Função para inicializar os recursos na GPU
    glGenVertexArrays(1, &vao);               // Gera o array de vértices
    glBindVertexArray(vao);                   // Vincula o array de vértices

    GLfloat vertices[] = {                    // Define as coordenadas dos vértices
        -1.0f, -1.0f, 0.0f,                   // Triângulo inferior esquerdo
         1.0f, -1.0f, 0.0f,                   // Triângulo inferior direito
         0.0f,  1.0f, 0.0f,                   // Triângulo superior

        -1.0f,  1.0f, 0.0f,                   // Segundo triângulo inferior esquerdo
         1.0f,  1.0f, 0.0f,                   // Segundo triângulo superior direito
         0.0f, -1.0f, 0.0f,                   // Segundo triângulo superior esquerdo
    };

    glGenBuffers(1, &vbo);                    // Gera o buffer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);       // Vincula o buffer de vértices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Preenche o buffer de vértices com os dados

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);  // Cria um shader de vértices
    glShaderSource(vs, 1, &vertex_shader, NULL);   // Associa o código fonte ao shader de vértices
    glCompileShader(vs);                          // Compila o shader de vértices

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); // Cria um shader de fragmentos
    glShaderSource(fs, 1, &fragment_shader, NULL);  // Associa o código fonte ao shader de fragmentos
    glCompileShader(fs);                           // Compila o shader de fragmentos

    shaderProgram = glCreateProgram();          // Cria um programa de shader
    glAttachShader(shaderProgram, vs);          // Anexa o shader de vértices ao programa
    glAttachShader(shaderProgram, fs);          // Anexa o shader de fragmentos ao programa
    glLinkProgram(shaderProgram);               // Linka o programa de shader
    glDeleteShader(vs);                         // Deleta o shader de vértices
    glDeleteShader(fs);                         // Deleta o shader de fragmentos

    attribute_coord3d = glGetAttribLocation(shaderProgram, "coord3d"); // Obtém a localização do atributo de coordenadas 3D
    if (attribute_coord3d == -1) {            // Verifica se o atributo foi vinculado corretamente
        std::cerr << "Não foi possível vincular o atributo coord3d" << std::endl; // Exibe uma mensagem de erro
        return;                                // Retorna caso não seja possível vincular o atributo
    }

    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0); // Define os atributos dos vértices
    glEnableVertexAttribArray(attribute_coord3d); // Habilita o atributo de coordenadas 3D
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) { // Função de callback para eventos de teclado
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { // Verifica se a tecla pressionada foi a tecla Esc
        glfwSetWindowShouldClose(window, GLFW_TRUE); // Fecha a janela GLFW
    }
}

int main() {                                   // Função principal
    if (!glfwInit()) {                         // Inicializa a biblioteca GLFW
        return -1;                             // Retorna -1 em caso de falha na inicialização
    }

    glfwWindowHint(GLFW_SAMPLES, 4);           // Configura o número de amostras para anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Configura a versão principal do contexto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Configura a versão secundária do contexto OpenGL
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Habilita o modo de compatibilidade avançado OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Configura o perfil de contexto OpenGL para núcleo

    window = glfwCreateWindow(640, 480, "Triângulos OpenGL", NULL, NULL); // Cria uma janela GLFW
    if (!window) {                             // Verifica se a janela foi criada corretamente
        glfwTerminate();                       // Encerra a biblioteca GLFW
        return -1;                             // Retorna -1 em caso de falha na criação da janela
    }

    glfwMakeContextCurrent(window);            // Define a janela como o contexto atual
    glewExperimental = GL_TRUE;                // Habilita o modo experimental GLEW para suporte a modernas extensões OpenGL
    if (glewInit() != GLEW_OK) {               // Inicializa a biblioteca GLEW
        glfwTerminate();                       // Encerra a biblioteca GLFW
        return -1;                             // Retorna -1 em caso de falha na inicialização do GLEW
    }

    glEnable(GL_DEPTH_TEST);                   // Habilita o teste de profundidade para renderização 3D
    init_resources();                          // Inicializa os recursos na GPU
    glfwSetKeyCallback(window, key_callback);  // Configura a função de callback para eventos de teclado

    while (!glfwWindowShouldClose(window)) {   // Loop principal de renderização
        onDisplay();                           // Renderiza a cena na janela
        glfwPollEvents();                      // Verifica e processa eventos da janela
    }

    free_resources();                          // Libera os recursos alocados na GPU
    glfwTerminate();                           // Encerra a biblioteca GLFW

    return 0;                                  // Retorna 0 indicando execução bem-sucedida
}
