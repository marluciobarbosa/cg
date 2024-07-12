// Inclusão das bibliotecas necessárias
#include <GL/glew.h>  // Biblioteca para carregar extensões OpenGL
#include <GLFW/glfw3.h>  // Biblioteca para criar janelas e contextos OpenGL
#include <glm/glm.hpp>  // Biblioteca GLM para operações matemáticas
#include <glm/gtc/matrix_transform.hpp>  // Para glm::perspective
#include <glm/gtc/type_ptr.hpp>  // Para glm::value_ptr
#include <cmath>  // Para funções matemáticas como sin, cos, M_PI
#include <iostream>  // Para saída de console (std::cout)
#include <cstring>  // Para função strcmp

// Função para inicializar o GLFW e criar uma janela
GLFWwindow* inicializar_glfw() {
    if (!glfwInit()) {  // Tenta inicializar o GLFW
        return nullptr;  // Se falhar, retorna nullptr
    }
    // Cria uma janela de 800x800 pixels com o título "Modelo de iluminação Phong"
    GLFWwindow* window = glfwCreateWindow(800, 800, "Modelo de iluminação Phong", nullptr, nullptr);
    if (!window) {  // Se falhar ao criar a janela
        glfwTerminate();  // Finaliza o GLFW
        return nullptr;  // Retorna nullptr
    }
    glfwMakeContextCurrent(window);  // Torna a janela o contexto OpenGL atual
    return window;  // Retorna o ponteiro para a janela criada
}

// Função para configurar as propriedades de iluminação
void configurar_iluminacao() {
    glEnable(GL_LIGHTING);  // Habilita a iluminação
    glEnable(GL_LIGHT0);  // Habilita a luz 0 (ambiente)
    glEnable(GL_LIGHT1);  // Habilita a luz 1 (difusa)
    glEnable(GL_LIGHT2);  // Habilita a luz 2 (especular)

    // Configura a luz ambiente
    float luz_ambiente[] = {0.2f, 0.2f, 0.2f, 1.0f};  // Cor da luz ambiente (cinza claro)
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);  // Define a cor da luz ambiente
    float luz_zero[] = {0.0f, 0.0f, 0.0f, 1.0f};  // Luz "nula" para desativar componentes
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_zero);  // Sem componente difusa
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_zero);  // Sem componente especular

    // Configura a luz difusa
    float luz_difusa[] = {0.8f, 0.8f, 0.8f, 1.0f};  // Cor da luz difusa (branco)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz_difusa);  // Define a cor da luz difusa
    glLightfv(GL_LIGHT1, GL_AMBIENT, luz_zero);  // Sem componente ambiente
    glLightfv(GL_LIGHT1, GL_SPECULAR, luz_zero);  // Sem componente especular
    float posicao_luz[] = {1.0f, 0.0f, 0.0f, 0.0f};  // Posição da luz (luz direcional)
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz);  // Define a posição da luz difusa

    // Configura a luz especular
    float luz_especular[] = {1.0f, 1.0f, 1.0f, 1.0f};  // Cor da luz especular (branco brilhante)
    glLightfv(GL_LIGHT2, GL_SPECULAR, luz_especular);  // Define a cor da luz especular
    glLightfv(GL_LIGHT2, GL_AMBIENT, luz_zero);  // Sem componente ambiente
    glLightfv(GL_LIGHT2, GL_DIFFUSE, luz_zero);  // Sem componente difusa
    glLightfv(GL_LIGHT2, GL_POSITION, posicao_luz);  // Define a posição da luz especular
}

// Função para desenhar uma esfera
void desenhar_esfera(float raio, int slices, int stacks) {
    // Este loop cria as fatias verticais da esfera
    for (int i = 0; i < slices; ++i) {
        // Calcula as coordenadas para duas fatias adjacentes
        float lat0 = M_PI * (-0.5 + float(i) / slices);
        float z0 = raio * sin(lat0);
        float zr0 = raio * cos(lat0);

        float lat1 = M_PI * (-0.5 + float(i + 1) / slices);
        float z1 = raio * sin(lat1);
        float zr1 = raio * cos(lat1);

        // Começa a desenhar uma faixa de quadriláteros
        glBegin(GL_QUAD_STRIP);
        // Este loop cria os segmentos horizontais da esfera
        for (int j = 0; j <= stacks; ++j) {
            float lng = 2 * M_PI * float(j) / stacks;
            float x = cos(lng);
            float y = sin(lng);
            // Define a normal e o vértice para dois pontos adjacentes
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();  // Termina de desenhar a faixa de quadriláteros
    }
}

// Função para desenhar uma esfera em uma posição específica
void desenhar_esfera_na_posicao(float x, float y, const char* modo_de_iluminacao) {
    glPushMatrix();  // Salva o estado atual da matriz
    glTranslatef(x, y, -5);  // Move a esfera para a posição desejada

    // Define as propriedades do material da esfera
    float cor_verde[] = {0.0f, 1.0f, 0.0f, 1.0f};  // Cor verde
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cor_verde);  // Define cor ambiente e difusa
    glMaterialfv(GL_FRONT, GL_SPECULAR, cor_verde);  // Define cor especular
    glMaterialf(GL_FRONT, GL_SHININESS, 1.0f);  // Define o brilho do material

    // Ativa diferentes luzes baseado no modo de iluminação
    if (strcmp(modo_de_iluminacao, "ambiente") == 0) {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
    } else if (strcmp(modo_de_iluminacao, "difusa") == 0) {
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
    } else if (strcmp(modo_de_iluminacao, "especular") == 0) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
    } else if (strcmp(modo_de_iluminacao, "phong") == 0) {
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
    }
    
    desenhar_esfera(0.5f, 250, 250);  // Desenha a esfera
    glPopMatrix();  // Restaura o estado anterior da matriz
}

// Função para renderizar a cena
void renderizar() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Limpa os buffers de cor e profundidade
    glMatrixMode(GL_PROJECTION);  // Muda para a matriz de projeção
    glLoadIdentity();  // Reseta a matriz de projeção
    
    // Substituindo gluPerspective(45, 1, 1, 10) com glm::perspective
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);  // Muda para a matriz de modelagem
    glLoadIdentity();  // Reseta a matriz de modelagem

    // Desenha as esferas nos quatro quadrantes com diferentes modos de iluminação
    desenhar_esfera_na_posicao(-1.5f, 1.5f, "ambiente");
    desenhar_esfera_na_posicao(1.5f, 1.5f, "difusa");
    desenhar_esfera_na_posicao(-1.5f, -1.5f, "especular");
    desenhar_esfera_na_posicao(1.5f, -1.5f, "phong");
}

// Função principal do programa
int main() {
    GLFWwindow* window = inicializar_glfw();  // Inicializa o GLFW e cria a janela
    if (!window) {  // Se falhar ao criar a janela
        return -1;  // Encerra o programa
    }

    if (glewInit() != GLEW_OK) {  // Inicializa o GLEW
        std::cout << "Falha ao inicializar GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  // Habilita o teste de profundidade
    configurar_iluminacao();  // Configura a iluminação

    // Loop principal do programa
    while (!glfwWindowShouldClose(window)) {
        renderizar();  // Renderiza a cena
        glfwSwapBuffers(window);  // Troca os buffers de exibição
        glfwPollEvents();  // Processa eventos da janela
    }

    glfwTerminate();  // Finaliza o GLFW quando o loop termina
    return 0;
}