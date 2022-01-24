//Codigo feito por Julia Amadio.
//Este foi feito com o intuito de que, a partir da imagem venus2.pgm, possam ser gerados arquivos de limiarizacao, negativo e histograma.
#include <stdio.h>
#include <stdlib.h>
#define LIMIAR 150

//-------declaracao
typedef struct{
    char tipo_arquivo[3];  //tipo do arquivo definido por código de 2 caracteres. exemplo: P2
    int n_colunas;
    int n_linhas; //tamanho da matriz da imagem determinado pelo num. de colunas e linhas
    int max_tons_cinza; //num. maximo de tons de cinza. contem info sobre a quantizacao da imagem
    int *matriz_imagem; //valores dos pixels, do superior esquerdo ao inferior direito
}IMAGEM;

void thresholding(IMAGEM img);
void neg(IMAGEM img);
void histogram(IMAGEM img);

//-------main e funcoes
int main(){
    FILE *f;
    IMAGEM img; //pegando informacoes da imagem
    int i, j;

    f = fopen("venus2.pgm", "r");
    if(f == NULL){
        printf("ERRO: O arquivo venus2.pgm nao foi encontrado no diretorio!\n");
        return 0;
    }

    fscanf(f, "%s\n", img.tipo_arquivo);
    fscanf(f, "%d ", &img.n_colunas);
    fscanf(f, "%d\n", &img.n_linhas);
    fscanf(f, "%d\n", &img.max_tons_cinza);
    img.matriz_imagem = (int*) malloc (img.n_colunas * img.n_linhas * sizeof(int));

    for (i = 0; i < img.n_linhas; i++){
        for (j = 0; j < img.n_colunas; j++){            
            fscanf(f, " %d", (img.matriz_imagem + ((i * img.n_colunas) + j)));
        } 
    }

    thresholding(img);
    neg(img);
    histogram(img);

    free(img.matriz_imagem);
    return 0;
}

void thresholding(IMAGEM img){
    int i, j, aux;
    FILE *file_tsd;

    file_tsd = fopen("thresholding.pgm", "w");
    fprintf(file_tsd, "%s\n", img.tipo_arquivo);
    fprintf(file_tsd, "%d ", img.n_colunas);
    fprintf(file_tsd, "%d\n", img.n_linhas);
    fprintf(file_tsd, "%d\n", img.max_tons_cinza);

    for(i = 0; i < img.n_linhas; i++){
        for (j = 0; j < img.n_colunas; j++){
            aux = *(img.matriz_imagem + (i * img.n_colunas) + j);
            if (aux <= LIMIAR){
                fprintf(file_tsd, "%d ", 0);
            }
            if (aux > LIMIAR){
                fprintf(file_tsd, "%d ", 255);
            }            
        }       
        fprintf(file_tsd, "\n");
    }
    
    printf("Limiarizacao thresholding.pgm criado com sucesso.\n");
    fclose(file_tsd);
}

void neg(IMAGEM img){
    FILE *file_neg;
    int i, j, v;

    file_neg = fopen("negative.pgm", "w");
    fprintf(file_neg, "%s\n", img.tipo_arquivo);
    fprintf(file_neg, "%d ", img.n_colunas);
    fprintf(file_neg, "%d\n", img.n_linhas);
    fprintf(file_neg, "%d\n", img.max_tons_cinza);

    for (i = 0; i < img.n_linhas; i++){
        for (j = 0; j < img.n_colunas; j++) {
            v = *(img.matriz_imagem + (i * img.n_colunas) + j);
            v = 255 - v;
            fprintf(file_neg, "%d ", v);
        }
        fprintf(file_neg, "\n");
    }
    
    printf("Negativo negative.pgm criado com sucesso.\n");
    fclose(file_neg);
}

void histogram(IMAGEM img){
    FILE *file_hist;
    int i, j, k;
    int array[256] = {0}; 

    file_hist = fopen("histogram.txt", "w");
    fprintf(file_hist, "%s\n", img.tipo_arquivo);
    fprintf(file_hist, "%d ", img.n_colunas);
    fprintf(file_hist, "%d\n", img.n_linhas);
    fprintf(file_hist, "%d\n", img.max_tons_cinza);

    for (i = 0; i < img.n_linhas; i++){
        for (j = 0; j < img.n_colunas; j++){
            k = *(img.matriz_imagem + i * img.n_colunas + j);      
            array[k]++;
        }
    }

    for (i = 0; i < 256; i++) {
        fprintf(file_hist, "Tonalidade %d: %d\n", i, array[i]);
    }
    
    printf("Histograma histogram.txt criado com sucesso.");
    free(array);
    fclose(file_hist);
}