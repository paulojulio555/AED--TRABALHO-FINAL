#include <stdio.h>
#include <stdlib.h>

#define MAX_CHAVES 3

typedef struct No {
    int chaves[MAX_CHAVES];
    struct No* filhos[MAX_CHAVES + 1];
    int num_chaves;
    int eh_folha;
} No;

No* criar_no() {
    No* novo_no = (No*)malloc(sizeof(No));
    novo_no->num_chaves = 0;
    novo_no->eh_folha = 1;
    for (int i = 0; i < MAX_CHAVES + 1; i++) {
        novo_no->filhos[i] = NULL;
    }
    return novo_no;
}

void dividir_filho(No* pai, int indice) {
    No* filho = pai->filhos[indice];
    No* novo_no = criar_no();
    novo_no->eh_folha = filho->eh_folha;
    novo_no->num_chaves = MAX_CHAVES / 2;

    for (int i = 0; i < MAX_CHAVES / 2; i++) {
        novo_no->chaves[i] = filho->chaves[i + MAX_CHAVES / 2];
    }

    if (!filho->eh_folha) {
        for (int i = 0; i < MAX_CHAVES / 2 + 1; i++) {
            novo_no->filhos[i] = filho->filhos[i + MAX_CHAVES / 2];
        }
    }

    filho->num_chaves = MAX_CHAVES / 2;

    for (int i = pai->num_chaves; i > indice; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
    }

    pai->filhos[indice + 1] = novo_no;

    for (int i = pai->num_chaves - 1; i >= indice; i--) {
        pai->chaves[i + 1] = pai->chaves[i];
    }

    pai->chaves[indice] = filho->chaves[MAX_CHAVES / 2];
    pai->num_chaves++;
}

void inserir_nao_cheio(No* no, int chave) {
    int i = no->num_chaves - 1;

    if (no->eh_folha) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }

        no->chaves[i + 1] = chave;
        no->num_chaves++;
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            i--;
        }

        i++;

        if (no->filhos[i]->num_chaves == MAX_CHAVES) {
            dividir_filho(no, i);

            if (chave > no->chaves[i]) {
                i++;
            }
        }

        inserir_nao_cheio(no->filhos[i], chave);
    }
}

void inserir(No** raiz, int chave) {
    No* no = *raiz;

    if (no == NULL) {
        no = criar_no();
        no->chaves[0] = chave;
        no->num_chaves++;
        *raiz = no;
    } else {
        if (no->num_chaves == MAX_CHAVES) {
            No* novo_no = criar_no();
            novo_no->eh_folha = 0;
            novo_no->filhos[0] = no;
            *raiz = novo_no;
            dividir_filho(novo_no, 0);
            inserir_nao_cheio(novo_no, chave);
        } else {
            inserir_nao_cheio(no, chave);
        }
    }
}

void deletar_chave(No* no, int chave);

void mesclar_filhos(No* no, int indice) {
    No* filho = no->filhos[indice];
    No* irmao = no->filhos[indice + 1];

    filho->chaves[MAX_CHAVES] = no->chaves[indice];
    filho->num_chaves++;

    for (int i = 0; i < irmao->num_chaves; i++) {
        filho->chaves[filho->num_chaves] = irmao->chaves[i];
        filho->filhos[filho->num_chaves] = irmao->filhos[i];
        filho->num_chaves++;
    }

    filho->filhos[filho->num_chaves] = irmao->filhos[irmao->num_chaves];

    for (int i = indice; i < no->num_chaves - 1; i++) {
        no->chaves[i] = no->chaves[i + 1];
        no->filhos[i + 1] = no->filhos[i + 2];
    }

    no->num_chaves--;

    free(irmao);

    if (no->num_chaves == 0) {
        if (no->eh_folha) {
            free(no);
            no = NULL;
        } else {
            No* temp = no;
            no = no->filhos[0];
            free(temp);
        }
    }
}

void pegar_emprestado_do_irmao(No* no, int indice) {
    No* filho = no->filhos[indice];
    No* irmao_esquerdo = no->filhos[indice - 1];
    No* irmao_direito = no->filhos[indice + 1];

    if (indice > 0 && irmao_esquerdo->num_chaves > MAX_CHAVES / 2) {
        for (int i = filho->num_chaves; i > 0; i--) {
            filho->chaves[i] = filho->chaves[i - 1];
            filho->filhos[i + 1] = filho->filhos[i];
        }

        filho->filhos[1] = filho->filhos[0];
        filho->filhos[0] = irmao_esquerdo->filhos[irmao_esquerdo->num_chaves];
        filho->chaves[0] = no->chaves[indice - 1];
        filho->num_chaves++;

        no->chaves[indice - 1] = irmao_esquerdo->chaves[irmao_esquerdo->num_chaves - 1];

        irmao_esquerdo->num_chaves--;
    } else if (indice < no->num_chaves && irmao_direito->num_chaves > MAX_CHAVES / 2) {
        filho->chaves[filho->num_chaves] = no->chaves[indice];
        filho->filhos[filho->num_chaves + 1] = irmao_direito->filhos[0];
        filho->num_chaves++;

        no->chaves[indice] = irmao_direito->chaves[0];

        for (int i = 0; i < irmao_direito->num_chaves - 1; i++) {
            irmao_direito->chaves[i] = irmao_direito->chaves[i + 1];
            irmao_direito->filhos[i] = irmao_direito->filhos[i + 1];
        }

        irmao_direito->filhos[irmao_direito->num_chaves - 1] = irmao_direito->filhos[irmao_direito->num_chaves];
        irmao_direito->num_chaves--;
    }
}

void deletar_chave(No* no, int chave) {
    int i = 0;

    while (i < no->num_chaves && chave > no->chaves[i]) {
        i++;
    }

    if (no->eh_folha) {
        if (i < no->num_chaves && chave == no->chaves[i]) {
            for (int j = i; j < no->num_chaves - 1; j++) {
                no->chaves[j] = no->chaves[j + 1];
            }

            no->num_chaves--;
        }
    } else {
        if (i < no->num_chaves && chave == no->chaves[i]) {
            No* filho_esquerdo = no->filhos[i];
            No* filho_direito = no->filhos[i + 1];

            if (filho_esquerdo->num_chaves > MAX_CHAVES / 2) {
                int predecessor = filho_esquerdo->chaves[filho_esquerdo->num_chaves - 1];
                deletar_chave(filho_esquerdo, predecessor);
                no->chaves[i] = predecessor;
            } else if (filho_direito->num_chaves > MAX_CHAVES / 2) {
                int sucessor = filho_direito->chaves[0];
                deletar_chave(filho_direito, sucessor);
                no->chaves[i] = sucessor;
            } else {
                mesclar_filhos(no, i);
                deletar_chave(filho_esquerdo, chave);
            }
        } else {
            No* filho = no->filhos[i];

            if (filho->num_chaves == MAX_CHAVES / 2) {
                pegar_emprestado_do_irmao(no, i);
            }

            deletar_chave(filho, chave);
        }
    }
}

No* buscar(No* no, int chave) {
    int i = 0;

    while (i < no->num_chaves && chave > no->chaves[i]) {
        i++;
    }

    if (i < no->num_chaves && chave == no->chaves[i]) {
        return no;
    }

    if (no->eh_folha) {
        return NULL;
    }

    return buscar(no->filhos[i], chave);
}
void imprimir_arvore(No* no, int nivel) {
    if (no != NULL) {
        for (int i = 0; i < no->num_chaves; i++) {
            imprimir_arvore(no->filhos[i], nivel + 1);
            for (int j = 0; j < nivel; j++) {
                printf("  ");
            }
            printf("%d\n", no->chaves[i]);
        }
        imprimir_arvore(no->filhos[no->num_chaves], nivel + 1);
    }
}


int main() {
    No* raiz = NULL;

    inserir(&raiz, 10);
    inserir(&raiz, 20);
    inserir(&raiz, 30);
    inserir(&raiz, 40);
    inserir(&raiz, 50);
    inserir(&raiz, 60);
    inserir(&raiz, 70);
    inserir(&raiz, 80);
    inserir(&raiz, 90);
    inserir(&raiz, 100);

    printf("Árvore B:\n");
    imprimir_arvore(raiz, 0);

    int chave_para_deletar = 90;
    deletar_chave(raiz, chave_para_deletar);

    printf("\nÁrvore B após a exclusão da chave %d:\n", chave_para_deletar);
    imprimir_arvore(raiz, 0);

    int chave_para_buscar = 20;
    No* no_encontrado = buscar(raiz, chave_para_buscar);

    if (no_encontrado != NULL) {
        printf("\nChave %d encontrada na árvore B.\n", chave_para_buscar);
    } else {
        printf("\nChave %d não encontrada na árvore B.\n", chave_para_buscar);
    }

    return 0;
}
