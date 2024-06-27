#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>

// Dimensões da imagem
const int largura = 256;
const int altura = 256;

// Função para criar uma matriz 2D com fundo branco (valor máximo em RGB: 255)
std::vector<std::vector<std::array<int, 3>>> criarImagem() {
    std::vector<std::vector<std::array<int, 3>>> imagem(altura, std::vector<std::array<int, 3>>(largura, {255, 255, 255}));
    return imagem;
}

// Função para desenhar uma linha
void desenharLinha(std::vector<std::vector<std::array<int, 3>>>& imagem, int x0, int y0, int x1, int y1, std::array<int, 3> cor) {
    bool íngreme = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (íngreme) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int erro = dx / 2;
    int ypasso = (y0 < y1) ? 1 : -1;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (íngreme) {
            if (0 <= x && x < altura && 0 <= y && y < largura) {
                imagem[x][y] = cor;
            }
        } else {
            if (0 <= x && x < largura && 0 <= y && y < altura) {
                imagem[y][x] = cor;
            }
        }
        erro -= dy;
        if (erro < 0) {
            y += ypasso;
            erro += dx;
        }
    }
}

int main() {
    // Criar a imagem com fundo branco
    auto imagem = criarImagem();

    // Coordenadas do centro da flor
    int centro_x = largura / 2;
    int centro_y = altura / 2;
    int raio_pétala = 100;
    int num_pétalas = 16;
    std::array<int, 3> cor_amarela = {255, 255, 0};
    std::array<int, 3> cor_laranja = {255, 165, 0};

    // Desenhar as pétalas
    for (int i = 0; i < num_pétalas; i++) {
        double angulo = i * (360.0 / num_pétalas);
        int fim_x = static_cast<int>(centro_x + raio_pétala * std::cos(angulo * M_PI / 180.0));
        int fim_y = static_cast<int>(centro_y + raio_pétala * std::sin(angulo * M_PI / 180.0));
        desenharLinha(imagem, centro_x, centro_y, fim_x, fim_y, cor_amarela);
    }

    // Desenhar o centro da flor
    int raio_centro = 50;
    for (int x = centro_x - raio_centro; x <= centro_x + raio_centro; x++) {
        for (int y = centro_y - raio_centro; y <= centro_y + raio_centro; y++) {
            if ((x - centro_x) * (x - centro_x) + (y - centro_y) * (y - centro_y) <= raio_centro * raio_centro) {
                if (0 <= x && x < largura && 0 <= y && y < altura) {
                    imagem[y][x] = cor_laranja;
                }
            }
        }
    }

    // Salvar a imagem em formato PPM (P3 para imagens coloridas em ASCII)
    std::ofstream arquivo("girassol.ppm");
    arquivo << "P3\n" << largura << " " << altura << "\n255\n";  // Cabeçalho com valor máximo de RGB
    for (const auto& linha : imagem) {
        for (const auto& pixel : linha) {
            arquivo << pixel[0] << " " << pixel[1] << " " << pixel[2] << " ";  // Escrever cada valor de pixel RGB seguido por um espaço
        }
        arquivo << "\n";  // Nova linha após cada linha
    }

    std::cout << "Imagem PPM criada com sucesso." << std::endl;
    return 0;
}
