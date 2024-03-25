#include <GL/glew.h> // Inclui o cabeçalho GLEW (The OpenGL Extension Wrangler Library) para carregar as funções OpenGL.
#include <GLFW/glfw3.h> // Inclui o cabeçalho GLFW (Graphics Library Framework) para criar janelas e gerenciar eventos.
#include <iostream> // Inclui o cabeçalho para operações de entrada/saída (I/O) em C++.
#include <vector> // Inclui o cabeçalho do container std::vector da Biblioteca Padrão C++.
#include <glm/glm.hpp> // Inclui o cabeçalho da biblioteca GLM (OpenGL Mathematics) para operações matemáticas.
#include <glm/gtc/type_ptr.hpp> // Inclui o cabeçalho da biblioteca GLM para funções de conversão de tipos.
#include <glm/gtc/matrix_transform.hpp> // Inclui o cabeçalho da biblioteca GLM para operações de transformação de matrizes.

// Código fonte dos shaders vertex e fragment (são programas executados na GPU)
const char* vertex_shader_code = R"(
    #version 330 core // Especifica a versão do GLSL (OpenGL Shading Language) como 3.30 core
    layout(location = 0) in vec3 vertexPosition; // Declara um atributo de entrada para as posições dos vértices (vec3 = vetor de 3 elementos) com layout location 0
    layout(location = 1) in vec3 vertexColor; // Declara um atributo de entrada para as cores dos vértices com layout location 1

    uniform mat4 mvp; // Declara uma variável uniforme (constante durante a execução do shader) para a matriz de projeção
    uniform mat4 trans; // Declara uma variável uniforme para a matriz de transformação

    out vec3 fragmentColor; // Declara uma variável de saída para a cor do fragmento

    void main() { // Função principal do vertex shader
        gl_Position = mvp * trans * vec4(vertexPosition, 1.0); // Calcula a posição final do vértice
        fragmentColor = vertexColor; // Passa a cor do vértice para o fragment shader
    }
)";

const char* fragment_shader_code = R"(
    #version 330 core // Especifica a versão do GLSL como 3.30 core
    in vec3 fragmentColor; // Declara uma variável de entrada para a cor do fragmento
    out vec3 color; // Declara uma variável de saída para a cor final do fragmento

    void main() { // Função principal do fragment shader
        color = fragmentColor; // Define a cor final do fragmento
    }
)";

// Variáveis globais para o deslocamento da casa
GLfloat delta = 0.0f; // Inicializa o deslocamento como zero
GLuint VertexArrayID, programID, vertexbuffer, colorbuffer; // Declara identificadores para o Vertex Array Object (VAO), programa de shader, buffer de vértices e buffer de cores

// Protótipos de funções
GLuint compileShader(const char* source, GLenum shader_type) { // Função para compilar um shader
    GLuint shader = glCreateShader(shader_type); // Cria um shader do tipo especificado
    glShaderSource(shader, 1, &source, NULL); // Associa o código fonte ao shader
    glCompileShader(shader); // Compila o shader

    GLint success; // Variável para armazenar o status de compilação
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Obtém o status de compilação do shader
    if (!success) { // Se a compilação falhou
        GLchar infoLog[512]; // Buffer para armazenar o log de erros
        glGetShaderInfoLog(shader, 512, NULL, infoLog); // Obtém o log de erros do shader
        std::cerr << "Shader compilation failed: " << infoLog << std::endl; // Imprime o log de erros
        return 0; // Retorna 0 para indicar falha
    }
    return shader; // Retorna o identificador do shader compilado
}

void transferDataToGPUMemory() { // Função para transferir dados para a memória da GPU
    // Compila e vincula os shaders
    programID = glCreateProgram(); // Cria um novo programa de shader
    GLuint vertex_shader = compileShader(vertex_shader_code, GL_VERTEX_SHADER); // Compila o vertex shader
    GLuint fragment_shader = compileShader(fragment_shader_code, GL_FRAGMENT_SHADER); // Compila o fragment shader
    glAttachShader(programID, vertex_shader); // Anexa o vertex shader ao programa
    glAttachShader(programID, fragment_shader); // Anexa o fragment shader ao programa
    glLinkProgram(programID); // Vincula os shaders ao programa

    // Verifica erros na vinculação dos shaders
    GLint success; // Variável para armazenar o status de vinculação
    glGetProgramiv(programID, GL_LINK_STATUS, &success); // Obtém o status de vinculação do programa
    if (!success) { // Se a vinculação falhou
        GLchar infoLog[512]; // Buffer para armazenar o log de erros
        glGetProgramInfoLog(programID, 512, NULL, infoLog); // Obtém o log de erros do programa
        std::cerr << "Shader linking failed: " << infoLog << std::endl; // Imprime o log de erros
        return; // Retorna sem fazer nada
    }

    // Vértices e cores para os componentes da casa
    std::vector<GLfloat> g_vertex_buffer_data = { // Vetor contendo os dados dos vértices
        // Telhado (triângulo)
        -10.0f, 20.0f, 0.0f, // Vértice 1 do telhado
        10.0f, 20.0f, 0.0f, // Vértice 2 do telhado
        0.0f, 30.0f, 0.0f, // Vértice 3 do telhado
        // Paredes (retângulo)
        -10.0f, 10.0f, 0.0f, // Vértice 1 da parede esquerda
        -10.0f, 20.0f, 0.0f, // Vértice 2 da parede esquerda
        10.0f, 20.0f, 0.0f, // Vértice 3 da parede direita
        -10.0f, 10.0f, 0.0f, // Vértice 1 da parede esquerda (novamente)
        10.0f, 20.0f, 0.0f, // Vértice 2 da parede direita (novamente)
        10.0f, 10.0f, 0.0f // Vértice 3 da parede direita
    };

    glGenVertexArrays(1, &VertexArrayID); // Gera um novo Vertex Array Object (VAO)
    glBindVertexArray(VertexArrayID); // Vincula o VAO para uso

    glGenBuffers(1, &vertexbuffer); // Gera um novo buffer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Vincula o buffer de vértices para uso
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), g_vertex_buffer_data.data(), GL_STATIC_DRAW); // Envia os dados dos vértices para o buffer

    std::vector<GLfloat> g_color_buffer_data = { // Vetor contendo os dados das cores
        // Telhado (cor vermelha)
        1.0f, 0.0f, 0.0f, // Cor vermelha para o vértice 1 do telhado
        1.0f, 0.0f, 0.0f, // Cor vermelha para o vértice 2 do telhado
        1.0f, 0.0f, 0.0f, // Cor vermelha para o vértice 3 do telhado
        // Paredes (cor azul)
        0.0f, 0.0f, 1.0f, // Cor azul para o vértice 1 da parede esquerda
        0.0f, 0.0f, 1.0f, // Cor azul para o vértice 2 da parede esquerda
        0.0f, 0.0f, 1.0f, // Cor azul para o vértice 3 da parede direita
        0.0f, 0.0f, 1.0f, // Cor azul para o vértice 1 da parede esquerda (novamente)
        0.0f, 0.0f, 1.0f, // Cor azul para o vértice 2 da parede direita (novamente)
        0.0f, 0.0f, 1.0f // Cor azul para o vértice 3 da parede direita
    };

    glGenBuffers(1, &colorbuffer); // Gera um novo buffer de cores
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); // Vincula o buffer de cores para uso
    glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size() * sizeof(GLfloat), g_color_buffer_data.data(), GL_STATIC_DRAW); // Envia os dados das cores para o buffer
}

void cleanupDataFromGPU() { // Função para limpar os dados da GPU
    glDeleteBuffers(1, &vertexbuffer); // Exclui o buffer de vértices
    glDeleteBuffers(1, &colorbuffer); // Exclui o buffer de cores
    glDeleteVertexArrays(1, &VertexArrayID); // Exclui o Vertex Array Object (VAO)
    glDeleteProgram(programID); // Exclui o programa de shader
}

void draw(GLFWwindow* window) { // Função para desenhar a cena
    // Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor

    // Usa o shader
    glUseProgram(programID); // Ativa o programa de shader

    // Cria o domínio da cena
    glm::mat4 mvp = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f); // Cria a matriz de projeção ortogonal

    // Obtém as localizações das variáveis uniformes
    GLint matrix = glGetUniformLocation(programID, "mvp"); // Obtém a localização da variável uniforme "mvp"
    glUniformMatrix4fv(matrix, 1, GL_FALSE, glm::value_ptr(mvp)); // Envia a matriz de projeção para o shader

    glm::mat4 trans = glm::mat4(1.0f); // Cria uma matriz de transformação identidade
    trans = glm::translate(trans, glm::vec3(delta, delta, 0.0f)); // Aplica uma translação à matriz de transformação
    GLint m = glGetUniformLocation(programID, "trans"); // Obtém a localização da variável uniforme "trans"
    glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(trans)); // Envia a matriz de transformação para o shader

    // 1º buffer de atributos: vértices
    glEnableVertexAttribArray(0); // Habilita o atributo de vértices
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Vincula o buffer de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Configura o ponteiro de atributo para os vértices

    // 2º buffer de atributos: cores
    glEnableVertexAttribArray(1); // Habilita o atributo de cores
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); // Vincula o buffer de cores
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Configura o ponteiro de atributo para as cores

    // Desenha os componentes da casa
    glDrawArrays(GL_TRIANGLES, 0, 3); // Desenha o telhado (triângulo)
    glDrawArrays(GL_TRIANGLES, 3, 6); // Desenha as paredes (retângulo)

    // Desabilita os arrays de atributos para vértices
    glDisableVertexAttribArray(0); // Desabilita o atributo de vértices
    glDisableVertexAttribArray(1); // Desabilita o atributo de cores
}

int main() { // Função principal
    // Inicializa o GLFW
    if (!glfwInit()) { // Se a inicialização do GLFW falhar
        std::cerr << "Falha ao inicializar o GLFW" << std::endl; // Imprime uma mensagem de erro
        return -1; // Retorna um código de erro
    }

    // Configuração da versão do OpenGL e afins
    glfwWindowHint(GLFW_SAMPLES, 4); // Define a amostragem de anti-aliasing para 4 amostras
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Define a versão principal do contexto OpenGL para 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Define a versão secundária do contexto OpenGL para 3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Habilita a compatibilidade futura do OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Define o perfil do OpenGL para core

    // Abre uma janela
    GLFWwindow* window = glfwCreateWindow(800, 800, "Movimentação de casa em 2D", nullptr, nullptr); // Cria uma janela de 800x800 pixels com o título "Moving House in 2D"
    if (!window) { // Se a criação da janela falhar
        std::cerr << "Falha ao abrir a janela GLFW" << std::endl; // Imprime uma mensagem de erro
        glfwTerminate(); // Termina o GLFW
        return -1; // Retorna um código de erro
    }

    // Cria o contexto da janela
    glfwMakeContextCurrent(window); // Define o contexto OpenGL atual para a janela criada

    // Inicializa o GLEW
    glewExperimental = true; // Indica que o GLEW deve usar funções modernas
    if (glewInit() != GLEW_OK) { // Se a inicialização do GLEW falhar
        std::cerr << "Falha ao inicializar o GLEW" << std::endl; // Imprime uma mensagem de erro
        return -1; // Retorna um código de erro
    }

    // Garante que possamos capturar a tecla Escape pressionada abaixo
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Define o modo de entrada para capturar teclas pressionadas mesmo após serem soltas

    // Cor de fundo branca
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Define a cor de fundo como branca

    // Transfere os dados (vértices, cores e shaders) para a GPU
    transferDataToGPUMemory();

    // Renderiza a cena para cada quadro
    delta = 0.0f; // Inicializa o deslocamento como zero
    while (!glfwWindowShouldClose(window)) { // Loop enquanto a janela não for fechada
        draw(window); // Chama a função draw() para desenhar a cena
        glfwSwapBuffers(window); // Troca os buffers de renderização (double buffering)
        glfwPollEvents(); // Processa eventos de entrada (teclado, mouse, etc.)
        if (delta < 10.0f) // Se o deslocamento for menor que 10
            delta += 0.05f; // Incrementa o deslocamento
    }

    // Limpa o VAO, VBOs e shaders da GPU
    cleanupDataFromGPU();

    // Fecha a janela OpenGL e termina o GLFW
    glfwTerminate();

    return 0; // Retorna 0 indicando sucesso
}