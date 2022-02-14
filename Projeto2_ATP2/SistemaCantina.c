//Codigo feito por Julia Amadio, Bacharelado de Ciências da Computação/Integral, 2o semestre. RA 211151041.
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX 1000 //limite do cadastro de produtos

//structs
typedef struct{
    char nome[50]; //chave de busca
    int index; //posicao dos detalhes do produto
}CHAVE;

typedef struct {
    int deletado; 
    float v_compra; //valor da compra
    float v_venda; //valor da venda
    int qtd_estoque; //qtd em estoque do produto
    int qtd_vendido; //qtd vendida do produto
}INFS;

//declaracao de funcoes
void inicio(CHAVE* key, INFS* info, int* cont, int* tam_infos);
void ler_arquivo(INFS* info, FILE* fInfos, CHAVE* key, FILE* fChave, int *cont, int* tam_infos);
void venda_produto(CHAVE* key, int cont, INFS* info);
void cadastro_novo_produto (CHAVE* key, INFS* info, int* cont, int* tam_infos);
void consulta_estoque(CHAVE* key, INFS* info, int cont);
void consulta_lucro(CHAVE* key, INFS* info, int cont);
void repor_estoque(CHAVE* key, INFS* info, int cont);
void remover_prod_estoque(CHAVE* key, int* cont, INFS* info, int* prod_all, float* lucro);
int busca_binaria(char *prod, CHAVE* key, int low, int high);
void partition(int esquerda, int direita, int *i, int *j, CHAVE *v);
void quicksort(CHAVE *v, int n);
void sort(int esquerda, int direita, CHAVE *v);
void finalizacao(CHAVE* key, int cont, INFS* info, int prod_all, float lucro);


int main(){
    int opc;
    int cont = 0;
    int tam_infos = 0;
    int prod_all = 0;
    float lucro = 0;
    FILE *fChave;
    FILE *fInfos;
    CHAVE *key;
    INFS *info;

    key = (CHAVE*) malloc (MAX * sizeof(CHAVE));
    info = (INFS*) malloc (MAX * sizeof(INFS*));
    fChave = fopen("chave.bin", "r+b");

    if(fChave == NULL){
        fChave = fopen("chave.bin", "w+b");
        fInfos = fopen("infs.bin", "w+b");
        inicio(key, info, &cont, &tam_infos);
        quicksort(key, cont);
    }else{
        fInfos = fopen("infs.bin", "r+b");
        ler_arquivo(info, fInfos, key, fChave, &cont, &tam_infos);
    }

    while (1){
        
        printf("\n\t---------- MENU ----------\n\n");
        printf("[1] - Vender\n");
        printf("[2] - Cadastrar novo produto\n");
        printf("[3] - Remover produto do estoque\n");
        printf("[4] - Consultar estoque\n");
        printf("[5] - Consultar lucro\n");
        printf("[6] - Repor estoque\n");
        printf("[0] - Sair\n"); //altera 
        printf("\nDigite a opcao desejada: ");
        scanf("%d", &opc);

        switch (opc){
            case 1:{
                venda_produto(key, cont, info);
                break;
            }

            case 2:{
                cadastro_novo_produto(key, info, &cont, &tam_infos);
                quicksort(key, cont);
                break;
            }

            case 3:{
                remover_prod_estoque(key, &cont, info, &prod_all, &lucro);
                break;
            }

            case 4:{
                consulta_estoque(key, info, cont);
                break;
            }

            case 5:{
                consulta_lucro(key, info, cont);
                break;
            }

            case 6:{
                repor_estoque(key, info, cont);
                break;
            }

            case 0:{
                finalizacao(key, cont, info, prod_all, lucro);
                fChave = fopen("chave.bin", "w+b");
                fInfos = fopen("infs.bin", "w+b");
                for (int i = 0; i < tam_infos; i++){
                    fwrite(&info[i].deletado, sizeof(int), 1, fInfos);
                    fwrite(&info[i].v_compra, sizeof(float), 1, fInfos);
                    fwrite(&info[i].v_venda, sizeof(float), 1, fInfos);
                    fwrite(&info[i].qtd_estoque, sizeof(int), 1, fInfos);
                    fwrite(&info[i].qtd_vendido, sizeof(int), 1, fInfos);
                }

                fclose(fInfos);

                for (int i = 0; i < cont; i++){
                    fwrite(key[i].nome, sizeof(char), 50, fChave);
                    fwrite(&key[i].index, sizeof(int), 1, fChave);
                }

                fclose(fChave);
                printf("\nFinalizando programa...\n");
                return 0;
            }
            default:{
                printf("\nERRO: Opcao invalida!\n");
                break;
            }   
        }
    }   
}

void inicio(CHAVE* key, INFS* info, int* cont, int* tam_infos){
    char continuar = 'S';
    int i=0;

    printf("PRIMEIRA EXECUCAO\n");
    
    while(1){
        printf("\nInsira o nome do produto: ");
        scanf("%s", key[i].nome);

        for (int l = 0; l < strlen(key[i].nome); l++){
            key[i].nome[l]=toupper(key[i].nome[l]);
        }

        key[i].index = i * (sizeof(INFS));
        info[i].deletado = -1;

        printf("\nInsira o valor de compra do produto: ");
        scanf("%f", &info[i].v_compra);

        printf("\nInsira o valor de venda do produto: ");
        scanf("%f", &info[i].v_venda);

        printf("\nInsira a quantidade do produto no estoque: ");
        scanf("%d", &info[i].qtd_estoque);

        printf("\nInsira a quantidade ja vendida do produto: ");
        scanf("%d", &info[i].qtd_vendido);

        (*cont)++;
        (*tam_infos)++;

        while (1){
            printf("\nDeseja continuar registrando? (S/N)\n");
            scanf(" %c", &continuar);
            if (continuar == 'S' || continuar == 's') break;
            else if (continuar == 'N' || continuar == 'n') return;
            else printf("\nOpcao invalida, tente novamente.");
        }
        i++;
    }
}

void ler_arquivo(INFS* info, FILE* fInfos, CHAVE* key, FILE* fChave, int *cont, int* tam_infos){
    while(fread(&info[*tam_infos].deletado, sizeof(int), 1, fInfos) != 0){
        fread(&info[*tam_infos].v_compra, sizeof(float), 1, fInfos);
        fread(&info[*tam_infos].v_venda, sizeof(float), 1, fInfos);
        fread(&info[*tam_infos].qtd_estoque, sizeof(int), 1, fInfos);
        fread(&info[*tam_infos].qtd_vendido, sizeof(int), 1, fInfos);
        (*tam_infos)++;
    }

    fclose(fInfos);

    while((fread(key[*cont].nome, sizeof(char), 50, fChave)) != '\0'){
        fread(&key[*cont].index, sizeof(int), 1, fChave);
        (*cont)++;
    }

    fclose(fChave);
    return;
}

void venda_produto(CHAVE* key, int cont, INFS* info){ 
    char *prod;
    int aux;

    prod = (char*) malloc (50*sizeof(char));

    printf("\nInsira o nome do produto: ");
    scanf("%s", prod);

    for (int i = 0; i < strlen(prod); i++){
        prod[i] = toupper(prod[i]);
    }
    
    int aux1 = busca_binaria(prod, key, 0, cont);
    if (aux1 < 0){
        printf("\nERRO: Produto nao encontrado! Voltando ao menu.");
        return;
    }

    int aux2 = key[aux1].index/sizeof(INFS);

    printf("\nInsira a quantidade: ");
    int qtd;
    scanf("%d", &qtd);

    if (info[aux2].qtd_estoque < qtd) {
        printf("\nNao ha estoque disponivel deste produto para realizar a venda.\n");
        return;
    }

    if (qtd <= 0){
        printf("\nQuantidade insuficiente ou incorreta.");
        return;
    }
    
    float total = qtd * info[aux2].v_venda;
    printf("\nValor total da venda: %.2f\n", total);
    info[aux2].qtd_estoque-=qtd;
    info[aux2].qtd_vendido+=qtd;
    return;
}

void cadastro_novo_produto(CHAVE* key, INFS* info, int* cont, int* tam_infos){ 
    char continuar = 'S';
    char *prod;
    int i = *cont, j = 0;

    prod = (char*) malloc (50 * sizeof(char));

    while(1){
        printf("\nInsira o nome do produto: ");
        scanf("%s", prod);

        for (int l = 0; l < strlen(prod); l++){
            prod[l]=toupper(prod[l]);
        }
        int n = *cont;
        int aux1 = busca_binaria(prod, key, 0, n);

        if (aux1 < 0){
            strcpy(key[i].nome, prod);
        }else{    
            printf("\nProduto ja existente!");
            return;
        }
            
        while (1){
            if (info[j].deletado == -1) j++;
            else{
                if(info[j].deletado == 0){
                    (*tam_infos)++;
                    break;
                }
                break;
            }            
        }

        key[i].index = j * (sizeof(INFS));
        info[j].deletado = -1;

        printf("\nInsira o valor de compra do produto: ");
        scanf("%f", &info[j].v_compra);

        printf("\nInsira o valor de venda do produto: ");
        scanf("%f", &info[j].v_venda);

        printf("\nInsira a quantidade do produto no estoque: ");
        scanf("%d", &info[j].qtd_estoque);

        printf("\nInsira a quantidade ja vendida do produto: ");
        scanf("%d", &info[j].qtd_vendido);

        (*cont)++;
        
        while (1){
            printf("\nDeseja continuar registrando? (S/N)\n");
            scanf(" %c", &continuar);
            if (continuar == 'S' || continuar == 's') break;
            else if (continuar == 'N' || continuar == 'n') return;
            else printf("\nOpcao invalida, tente novamente.");
        }
        i = *cont;
        j=0;
    }
}

void partition(int esquerda, int direita, int *i, int *j, CHAVE *v){
    CHAVE x, w;
    *i = esquerda; *j = direita;
    x = v[(*i + *j)/2];

    do{           
        while(strcmp(x.nome, v[*i].nome) > 0)
            (*i)++;
        while(strcmp(x.nome, v[*j].nome) < 0)
            (*j)--;

        if (*i <= *j){
            w = v[*i];
            v[*i] = v[*j];
            v[*j] = w;
            (*i)++;
            (*j)--;
        }
    }while(*i <= *j);
}

void sort(int esquerda, int direita, CHAVE *v){
    int i,j;

    partition(esquerda, direita, &i, &j, v);
    if (esquerda < j) sort(esquerda, j, v);
    if (i < direita) sort(i, direita, v);
}

void quicksort(CHAVE *v, int n){
    sort(0, n-1, v);
}

void remover_prod_estoque(CHAVE* key, int* cont, INFS* info, int* prod_all, float* lucro){ 
    char *prod;

    prod = (char*) malloc (50*sizeof(char));

    printf("\nInsira o nome do produto a ser removido: ");
    scanf("%s", prod);

    for (int i = 0; i < strlen(prod); i++){
        prod[i] = toupper(prod[i]);

    }
    int m = *cont;
    int aux1 = busca_binaria(prod, key, 0, m);

    if (aux1 < 0){
        printf("\nERRO: Produto nao encontrado!");
        return;
    }

    int aux2 = key[aux1].index/sizeof(INFS);

    (*cont)--;

    for (int i = aux1; i < (*cont); i++){
        strcpy(key[i].nome, key[i+1].nome);
        key[i].index = key[i+1].index;
    }
    
    info[aux2].deletado = 1;
    (*prod_all) += info[aux2].qtd_vendido;
    (*lucro) += info[aux2].qtd_vendido * info[aux1].v_venda;

    int n = *cont;
    quicksort(key, n);
    return;
}

void consulta_estoque(CHAVE* key, INFS* info, int cont){
    float aux = 0;

    for (int i = 0; i < cont; i++){
        printf("\nProduto: %s", key[i].nome);
        int j = key[i].index/sizeof(INFS);
        float k = info[j].qtd_estoque * info[j].v_venda;
        printf("\nValor total em estoque: R$ %.2f\n", k);
        aux += k;
    }

    printf("\n\nValor total do estoque: R$ %.2f\n", aux);
    return;
}

void consulta_lucro(CHAVE* key, INFS* info, int cont){ 
    for (int i = 0; i < cont; i++){
        printf("\nProduto: %s", key[i].nome);
        int j = key[i].index/sizeof(INFS);
        float k = info[j].qtd_vendido * info[j].v_venda;
        printf("\nLucro: R$ %.2f\n", k);
    }
    
    return;
}

void repor_estoque(CHAVE* key, INFS* info, int cont){
    char *prod;
    int aux1, aux2;

    prod = (char*) malloc (50 * sizeof(char));

    printf("\nInsira o produto que deseja repor o estoque: ");
    scanf("%s", prod);
    for (int i = 0; i < strlen(prod); i++){
        prod[i] = toupper(prod[i]);
    }
    
    aux1 = busca_binaria(prod, key, 0, cont);

    if (aux1 < 0){
        printf("\nERRO: Produto nao encontrado!");
        return;
    }

    aux2 = key[aux1].index/sizeof(INFS);
    
    int qtd;
    printf("\nInsira a quantidade a ser reposta no estoque: ");
    scanf("%d", &qtd);

    if (qtd <= 0){
        printf("\nQuantidade insuficiente ou incorreta.\n");
        return;
    }
    
    info[aux2].qtd_estoque += qtd;
    printf("\nO novo estoque de %s e: %d\n", key[aux1].nome, info[aux2].qtd_estoque);
    return;
}

int busca_binaria(char *prod, CHAVE *key, int low, int high){  
    int indice, x;

    if (low > high) return -1;
    indice = (low + high) / 2;
    x = strcmp( prod, key[indice].nome);
    if (x == 0 ) 
        return indice;
    if (x < 0) 
        return busca_binaria( prod, key, low, indice - 1); 
    else 
        return busca_binaria( prod, key, indice + 1, high); 
}

void finalizacao(CHAVE* key, int cont, INFS* info, int prod_all, float lucro){
    int aux = 0;
    float lucro2 = 0;

    for (int i = 0; i < cont; i++){
        int j = key[i].index/sizeof(INFS);
        aux += info[j].qtd_vendido;
    }

    printf("\nA cantina vendeu um total de %d produtos.", aux);
    int aux2 = aux + prod_all;
    printf("\nA cantina vendeu um total de %d produtos contando os itens excluidos nesta execucao.", aux2); 

    for (int i = 0; i < cont; i++){
        printf("\n\nNome do produto: %s", key[i].nome);
        int j = key[i].index/sizeof(INFS);
        printf("\nQuantidade vendida: %d", info[j].qtd_vendido);
        printf("\nEstoque restante: %d", info[j].qtd_estoque);
        printf("\nValor de venda: %.2f", info[j].v_venda);
        float k = info[j].qtd_estoque * info[j].v_venda;
        printf("\nValor total em estoque: R$ %.2f\n", k);
        lucro2 += info[j].v_venda * info[j].qtd_vendido;
    }

    float lucro3 = lucro2 + lucro;
    printf("\nLucro total do dia: R$ %.2f\n", lucro3); //este considera os itens excluidos
    return;
}