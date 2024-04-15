#include <GLFW/glfw3.h>  // Inclui a biblioteca GLFW
#include <OpenGL/gl.h>    // Inclui a biblioteca OpenGL

int main() {  // Função principal do programa
    // Inicializa o GLFW
    glfwInit();

    // Cria uma janela com as dimensões desejadas
    int larguraJanela = 800;
    int alturaJanela = 600;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Define a versão major do contexto OpenGL para 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // Define a versão minor do contexto OpenGL para 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Define o perfil do OpenGL como core profile
    GLFWwindow* janela = glfwCreateWindow(larguraJanela, alturaJanela, "Configuração da Viewport", NULL, NULL);  // Cria a janela
    glfwMakeContextCurrent(janela);  // Define a janela atual como contexto OpenGL

    // Define as dimensões da viewport para coincidir com as dimensões da janela inicialmente
    glViewport(0, 0, larguraJanela, alturaJanela);

    // Dimensões da cena (supondo que a cena está carregada ou calculada)
    int larguraCena = 1280;
    int alturaCena = 720;

    // Calcula a proporção de aspecto da cena
    float proporcaoAspectoCena = (float)larguraCena / (float)alturaCena;

    // Loop principal
    while (!glfwWindowShouldClose(janela)) {  // Enquanto a janela não deve ser fechada
        // Verifica eventos
        glfwPollEvents();

        // Ajusta as dimensões da viewport com base na proporção de aspecto da cena
        int larguraViewportAjustada, alturaViewportAjustada;
        if (proporcaoAspectoCena > 1.0f) {  // Se a proporção de aspecto da cena for maior que 1.0
            larguraViewportAjustada = larguraJanela;
            alturaViewportAjustada = larguraJanela / proporcaoAspectoCena;
        } else {
            larguraViewportAjustada = alturaJanela * proporcaoAspectoCena;
            alturaViewportAjustada = alturaJanela;
        }

        // Define as dimensões atualizadas da viewport
        glViewport(0, 0, larguraViewportAjustada, alturaViewportAjustada);  // (x, y, larguraViewportAjustada, alturaViewportAjustada)

        // Limpa a tela com um fundo preto
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderiza a geometria da cena aqui

        // Troca os buffers e desenha
        glfwSwapBuffers(janela);
    }

    // Finaliza o GLFW
    glfwTerminate();

    return 0;
}
