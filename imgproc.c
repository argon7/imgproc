#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define cprompt_TOK_BUFSIZE 64
#define cprompt_TOK_DELIM " \t\r\n\a"
#define CHUNK 209715200
typedef struct node {
  struct node *next;
  char val[];
} node_t;
typedef struct blobs {
  char belongsto[cprompt_TOK_BUFSIZE];
  int xx, yy;
  int blobsize;
  float mR, mG, mB;
  float dR, dG, dB;
  int id;
  struct blobs *next;
} blob_t;
typedef struct imgs {
  char name[cprompt_TOK_BUFSIZE];
  int nlines;
  int ncols;
  int nchannels;
  int *datatable;
  int *blobdata;
  int numzonas;
} img_t;
typedef struct list_node_s {
  struct list_node_s *next;
  img_t payload;
} list_node_t;
extern list_node_t *the_head;
extern int R, G, B, D;
list_node_t *the_head = NULL;
extern blob_t *blob_head;
blob_t *blob_head = NULL;
int R = 0, G = 0, B = 0, D = 0;


int read_image_file(char *filename);
int list_insert_tail_node(list_node_t **input_head, char *name, int lines, int cols, int channels);
int list_destroy(list_node_t **input_head);
int printall(list_node_t *head);
int get_tail(list_node_t  *listHead, list_node_t **listtail);
int *copy_the_images_in_file(int number, char *filename, int r, int g, int b, int d);
int *copy_image(int image_number, char *filename);
int *blobinate(list_node_t *current, int r, int g, int b, int d);
int grass(void);
int grassfire(int i, list_node_t *current, int blob_number);
void pushblob(blob_t **head, char *name, int xx, int yy, int blobsize, int id, float mr, float mg, float mb, float dR, float dG, float dB);
int blob_destroy(blob_t **input_head);
int printblob(blob_t *head);
int xcentermass(list_node_t *current, int blob_number);
int ycentermass(list_node_t *current, int blob_number);
int sizeblob(list_node_t *current, int blob_number);
float MR(list_node_t *current, int r, int g, int b, int d, int blob_number);
float MG(list_node_t *current, int r, int g, int b, int d, int blob_number);
float MB(list_node_t *current, int r, int g, int b, int d, int blob_number);
void bubbleSort(blob_t *start);
void swapb(blob_t *a, blob_t *b);
int maxzonas(list_node_t *head);
float desvioR(list_node_t *current, int r, int g, int b, int d, int blob_number, float mR);
float desvioG(list_node_t *current, int r, int g, int b, int d, int blob_number, float mG);
float desvioB(list_node_t *current, int r, int g, int b, int d, int blob_number, float mB);
int min_desvio(blob_t *head, list_node_t *thead);
int printblobdesvio(blob_t *head);

int main(int argc, char *argv[])
{
    int r;
    if (argc != 7)
        fprintf(stderr, "ERROR: Incorrect number of arguments entered.\n\nHOW TO USE:\n\n./imgproc <filename> R G B D [MENUS|ALL|MEM]\n\nfilename-- file with images to analyze\nR-- Red color (0-255)\nG-- Green Color color (0-255)\nB-- Blue Color color (0-255)\nD-- Value of tolerance (0-255)\nMENUS-- enter menu mode \nALL-- runs all tests\nMEM-- tests memory\n\n");
    else if ((atoi(argv[2]) < 0 || atoi(argv[3]) < 0 || atoi(argv[4]) < 0 || atoi(argv[5]) < 0) || (atoi(argv[2]) > 255 || atoi(argv[3]) > 255 || atoi(argv[4]) > 255 || atoi(argv[5]) > 255))
    {
        {
            fprintf(stderr, "ERROR: Incorrect VALUES.\n\nHOW TO USE:\n\n./imgproc <filename> R G B D [MENUS|ALL|MEM]\n\nfilename-- file with images to analyze\nR-- Red color (0-255)\nG-- Green Color color (0-255)\nB-- Blue Color color (0-255)\nD-- Value of tolerance (0-255)\nMENUS-- enter menu mode \nALL-- runs all tests\nMEM-- tests memory\n\n");
        }
        exit(1);
    }
    else if ((strcmp("ALL", argv[6]) == 0))
    {
        R = atoi(argv[2]);
        G = atoi(argv[3]);
        B = atoi(argv[4]);
        D = atoi(argv[5]);
        printf("\n\n---1: Ler o ficheiro---\n\n");
        r = read_image_file(argv[1]);
        if(r == 1){return 0;}
        printf("\n\n---2: Calcular as Zonas (BLOB)---\n\n");
        copy_the_images_in_file(r, argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        //printblob(blob_head);
        printf("\n\n---3: Mostrar as Zonas Ordenadas pelo numero de pixéis (do menor para o maior) ---\n\n");
        bubbleSort(blob_head);
        printblob(blob_head);
        printf("\n\n---4: Para um dado ficheiro, determinar a imagem com mais zonas---\n\n");
        maxzonas(the_head);
        printf("\n\n---5: Determinar o desvio padrão para todas as zonas calculadas de cada imagem---\n\n");
        printblobdesvio(blob_head);
        printf("\n\n--- Por ordem simplificado... (do menor para o maior) ---\n\n");
        printblob(blob_head);
        printf("\n\n---6: Para todas as imagens de um ficheiro, determinar a zona com menor desvio padrão e a que imagem corresponde---\n\n");
        min_desvio(blob_head, the_head);
        //return 0;
        //help();
        //cprompt_loop();
        //DESTROY THINGS HERE
        list_destroy(&the_head);
        blob_destroy(&blob_head);
    }
    else if ((strcmp("MEM", argv[6]) == 0))
    {
        R = atoi(argv[2]);
        G = atoi(argv[3]);
        B = atoi(argv[4]);
        D = atoi(argv[5]);
        while (1)
        {
            printf("\n\n---1: Ler o ficheiro---\n\n");
            r = read_image_file(argv[1]);
            if(r == 1){return 0;}
            printf("\n\n---2: Calcular as Zonas (BLOB)---\n\n");
            copy_the_images_in_file(r, argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
            //printblob(blob_head);
            printf("\n\n---3: Mostrar as Zonas Ordenadas pelo numero de pixéis (do menor para o maior) ---\n\n");
            bubbleSort(blob_head);
            printblob(blob_head);
            printf("\n\n---4: Para um dado ficheiro, determinar a imagem com mais zonas---\n\n");
            maxzonas(the_head);
            printf("\n\n---5: Determinar o desvio padrão para todas as zonas calculadas de cada imagem---\n\n");
            printblobdesvio(blob_head);
            printf("\n\n--- Por ordem simplificado... (do menor para o maior) ---\n\n");
            printblob(blob_head);
            printf("\n\n---6: Para todas as imagens de um ficheiro, determinar a zona com menor desvio padrão e a que imagem corresponde---\n\n");
            min_desvio(blob_head, the_head);
            //DESTROY THINGS HERE
            list_destroy(&the_head);
            blob_destroy(&blob_head);
            //destroy 2
        }
    }
    else if ((strcmp("MENUS", argv[6]) == 0))
    {
        R = atoi(argv[2]);
        G = atoi(argv[3]);
        B = atoi(argv[4]);
        D = atoi(argv[5]);
        char choice;
        printf("\n\n---1: Ler o ficheiro---\n\n");
        r = read_image_file(argv[1]);
        if(r == 1){return 0;}
        printf("\n\n---2: Calcular as Zonas (BLOB)---\n\n");
        copy_the_images_in_file(r, argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        //printblob(blob_head);
        printf("Choose an option:\n\n3-- Zonas Ordenadas pelo numero de pixéis\n4-- determinar a imagem com mais zonas\n5-- Determinar o desvio padrão para todas as zonas\n6-- determinar a zona com menor desvio padrão e a que imagem corresponde\n\n");
        scanf("%c", &choice);
        switch(choice)
        {
            case '3':
            printf("\n\n---3: Mostrar as Zonas Ordenadas pelo numero de pixéis (do menor para o maior) ---\n\n");
            bubbleSort(blob_head);
            printblob(blob_head);
            //DESTROY THINGS HERE
            list_destroy(&the_head);
            blob_destroy(&blob_head);
                break;
            case '4':
            printf("\n\n---4: Para um dado ficheiro, determinar a imagem com mais zonas---\n\n");
            maxzonas(the_head);
            //DESTROY THINGS HERE
            list_destroy(&the_head);
            blob_destroy(&blob_head);
                break;
            case '5':
            printf("\n\n---5: Determinar o desvio padrão para todas as zonas calculadas de cada imagem---\n\n");
            printblobdesvio(blob_head);
            printf("\n\n--- Por ordem simplificado... (do menor para o maior) ---\n\n");
            printblob(blob_head);
            //DESTROY THINGS HERE
            list_destroy(&the_head);
            blob_destroy(&blob_head);
                break;
            case '6':
            printf("\n\n---6:Determinar a zona com menor desvio padrão de cada imagem---\n\n");
            min_desvio(blob_head, the_head);
            //DESTROY THINGS HERE
            list_destroy(&the_head);
            blob_destroy(&blob_head);
                break;
            default:
                printf(" \n\n INVALID CHOICE \n");
                //DESTROY THINGS HERE
                list_destroy(&the_head);
                blob_destroy(&blob_head);
        }

    }
    else    fprintf(stderr, "ERROR: Incorrect COMAND.\n\nHOW TO USE:\n\n./imgproc <filename> R G B D [MENUS|ALL|MEM]\n\nfilename-- file with images to analyze\nR-- Red color (0-255)\nG-- Green Color color (0-255)\nB-- Blue Color color (0-255)\nD-- Value of tolerance (0-255)\nMENUS-- enter menu mode \nALL-- runs all tests\nMEM-- tests memory\n\n");
    return 0;

}

int read_image_file(char *filename)
{
    int number_of_images = 0;
    char name[cprompt_TOK_BUFSIZE];
    int pos = -10;
    int length, i, nlines = 0, ncols = 0, nchannels;
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;
    pFile = fopen(filename, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "ERROR: Cant open file\n");
        return 1;
    }
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);
    if (lSize > CHUNK)
        lSize = CHUNK;
    buffer = (char *)malloc(sizeof(char) * (unsigned long)lSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "ERROR: memory allocatrion error\n");
        return 1;
    }
    memset(buffer, 0, sizeof(char) * (unsigned long)lSize + 1);
    result = fread(buffer, sizeof(char), (unsigned long)lSize, pFile);
    if ((unsigned)result != lSize)
    {
        fprintf(stderr, "ERROR: cant read file\n");
        return 1;
    }
    int bufsize = cprompt_TOK_BUFSIZE, position = 0;
    char **tokens = malloc((unsigned long)bufsize * sizeof(char*));
    char *token, **tokens_backup;
    if (!tokens)
    {
        fprintf(stderr, "ERROR: memory allocation error\n");
        return 1;
    }
    token = strtok(buffer, cprompt_TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= bufsize)
        {
            bufsize += cprompt_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, (unsigned long)bufsize * sizeof(char*));
            if (!tokens)
            {
                free(tokens_backup);
                fprintf(stderr, "ERROR: memory allocation error\n");
                return 1;
            }
        }
        length = (int) strlen(token);
        for (i = 0; i < length; i++)
        {
            if (!isdigit(token[i]))
            {
                strcpy(name, token);
                pos = position;
                break;
            }
        }
        if (position == pos + 1)
            nlines = atoi(token);
        else if (position == pos + 2)
            ncols = atoi(token);
        else if (position == pos + 3)
        {
            nchannels = atoi(token);
            list_insert_tail_node(&the_head, name, nlines, ncols, nchannels);
            number_of_images++;
        }
        token = strtok(NULL, cprompt_TOK_DELIM);
    }
    tokens[position] = NULL;
    printall(the_head);
    fclose(pFile);
    free(buffer);
    free(tokens);
    return number_of_images;
}

int list_insert_tail_node(list_node_t **input_head, char *name, int lines, int cols, int channels)
{
    list_node_t *tailNode;
    list_node_t *newNode = NULL;
    get_tail(*input_head, &tailNode);
    newNode = malloc(sizeof(*newNode));
    if (NULL == newNode)
    {
        fprintf(stderr, "ERROR: memory allocation error\n");
        return 1;
    }
    strcpy(newNode->payload.name, name);
    newNode->payload.nlines = lines;
    newNode->payload.ncols = cols;
    newNode->payload.nchannels = channels;
    newNode->next = NULL;
    if (tailNode)
        tailNode->next = newNode;
    else
        *input_head = newNode;
    return (0);
}

int list_destroy(list_node_t **input_head)
{
    while (*input_head)
    {
        list_node_t *node_of_death = *input_head;
        *input_head = (*input_head)->next;
        //clear leaks?
        free(node_of_death->payload.blobdata);
        free(node_of_death->payload.datatable);
        free(node_of_death);
    }
    return (0);
}

int printall(list_node_t *head)
{
    list_node_t *current = head;
    int i = 0;
    printf("\nImages Read:\n\n");
    while (current)
    {
        printf("name :%s\n", current->payload.name);
        printf("lines :%d\n", current->payload.nlines);
        printf("cols: %d\n", current->payload.ncols);
        printf("channels: %d\n\n", current->payload.nchannels);
        current = current->next;
        i++;
    }
    printf("Total images (nodes): %d \n", i);
    return (0);
}

int get_tail(list_node_t  *listHead, list_node_t **listtail)
{
    list_node_t *current_node = listHead;
    if (current_node)
    {
        while (current_node->next != NULL)
            current_node = current_node->next;
    }
    *listtail = current_node;
    return (0);
}

int *copy_the_images_in_file(int number, char *filename, int r, int g, int b, int d)
{
    list_node_t *current = the_head;
    for (int i = 1; i <= number; i++)
    {
        printf("Processing image %d", i);
        current->payload.datatable = copy_image(i, filename);
        current->payload.blobdata = blobinate(current, r, g, b, d);
        //free(current->payload.blobdata);//
        current = current->next;
        printf(" #done\n");
    }
    printf("\n\n");
    //printall3(the_head); //<--- Mostrar imagens no ecra...
    grass();
    return 0;
}

int *copy_image(int image_number, char *filename)
{
    int its_image = 0;
    list_node_t *current_node = the_head;
    int l = current_node->payload.nlines, c = current_node->payload.ncols, ch = current_node->payload.nchannels;
    int total = l * c * ch;
    int *data;
    data = malloc((unsigned long)total * sizeof(int));
    int pos = -10;
    int length, i;
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;
    pFile = fopen(filename, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "ERROR: Cant open file\n");
        exit(1);
    }
    fseek(pFile, 0, SEEK_END); // procura o fim do ficheiro
    lSize = ftell(pFile);  // obtem o ponteiro atual para o ficheiro
    rewind(pFile);  // volta para o inicio do ficheiro
    if (lSize > CHUNK)
        lSize = CHUNK;
    buffer = (char *)malloc(sizeof(char) * (unsigned long)lSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "ERROR: memory allocatrion error\n");
        exit(1);
    }
    memset(buffer, 0, sizeof(char) * (unsigned long)lSize + 1);
    result = fread(buffer, sizeof(char), (unsigned long)lSize, pFile);
    if ((unsigned)result != lSize)
    {
        fprintf(stderr, "ERROR: cant read file\n");
        exit(1);
    }
    unsigned long bufsize = cprompt_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;
    if (!tokens)
    {
        fprintf(stderr, "ERROR: memory allocation error\n");
        exit(1);
    }
    token = strtok(buffer, cprompt_TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= bufsize)
        {
            bufsize += cprompt_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens)
            {
                free(tokens_backup);
                fprintf(stderr, "ERROR: memory allocation error\n");
                exit(1);
            }
        }
        length = (int)strlen(token);
        for (i = 0; i < length; i++)
        {
            if (!isdigit(token[i]))
            {
                its_image++;
                pos = (int)position;
                break;
            }
            if (isdigit(token[i]))
                break;
        }
        if ((int)position >= pos + 4 && (its_image == image_number))
            data[position - ((unsigned long)pos + (unsigned long)4)] = atoi(token);
        token = strtok(NULL, cprompt_TOK_DELIM);
    }
    tokens[position] = NULL;
    fclose(pFile);
    free(tokens);
    free(buffer);
    return data;
}

int *blobinate(list_node_t *current, int r, int g, int b, int d)
{
    int *blob_data;
    blob_data = malloc((unsigned long)current->payload.nlines * (unsigned long)current->payload.ncols * sizeof(int));
    int i = 0;
    for (int f = 0; f < current->payload.nlines * current->payload.ncols * current->payload.nchannels; f++)
    {
        if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            blob_data[i] = 1;
        else
            blob_data[i] = 0;
        f += 2;
        i++;
    }
    return (blob_data);
}

int grass()
{
    int zonas = 0;
    float grassMR = 0, grassMG = 0, grassMB = 0;
    float DR = 0, DG = 0, DB = 0;
    int xx = 0, yy = 0, blobsize = 0;
    int i = 0;
    list_node_t *current = the_head;
    int number_of_blobs_st2 = 2;
    while (current)
    {
        //printf("image:%s\n",current->payload.name);
        while (i <= current->payload.nlines * current->payload.ncols -1 )
        {
            if (current->payload.blobdata[i] == 1)
            {
                number_of_blobs_st2 = grassfire(i, current, number_of_blobs_st2);
                xx = xcentermass(current, number_of_blobs_st2 - 1);
                yy = ycentermass(current, number_of_blobs_st2 - 1);
                blobsize = sizeblob(current, number_of_blobs_st2 - 1);
                grassMR = MR(current, R, G, B, D, number_of_blobs_st2 - 1);
                grassMG = MG(current, R, G, B, D, number_of_blobs_st2 - 1);
                grassMB = MB(current, R, G, B, D, number_of_blobs_st2 - 1);
                DR = desvioR(current, R, G, B, D, number_of_blobs_st2 - 1, grassMR);
                DG = desvioG(current, R, G, B, D, number_of_blobs_st2 - 1, grassMG);
                DB = desvioB(current, R, G, B, D, number_of_blobs_st2 - 1, grassMB);
                pushblob(&blob_head, current->payload.name, xx, yy, blobsize, number_of_blobs_st2 - 2, grassMR, grassMG, grassMB, DR, DG, DB);
                zonas++;
            }
            i++;
        }
        number_of_blobs_st2 = 2;
        i = 0;
        current->payload.numzonas = zonas;
        zonas = 0;
        current = current->next;
    }
    //printall4(the_head);
    //printblob(blob_head);
    return (0);
}

int grassfire(int i, list_node_t *current, int blob_number)
{
    current->payload.blobdata[i] = blob_number;
    //printf("current->payload.blobdata[i] == %d\n",current->payload.blobdata[i] );
    //direita
    if ((i + 1) % current->payload.ncols != 0/*nao mudou linha*/ && i + 1 <= current->payload.ncols * current->payload.nlines/*nao chegou ao fim*/)
    {
        if (current->payload.blobdata[i + 1] == 1)
        {
            current->payload.blobdata[i + 1] = blob_number;
            grassfire(i + 1, current, blob_number);
        }
    }
    //esquerda
    if ((i) % current->payload.ncols != 0/*nao mudou linha*/ && i - 1 >= 0/*nao vai para tras da posiçao inicial*/)
    {
        if (current->payload.blobdata[i - 1] == 1)
        {
            current->payload.blobdata[i - 1] = blob_number;
            grassfire(i - 1, current, blob_number);
        }
    }
    //baixo
    if (i + current->payload.ncols < current->payload.nlines * current->payload.ncols/*existe linha debaixo*/)
    {
        if (current->payload.blobdata[i + current->payload.ncols] == 1)
        {
            current->payload.blobdata[i + current->payload.ncols] = blob_number;
            grassfire(i + current->payload.ncols, current, blob_number);
        }
    }
    //cima
    if (i - current->payload.ncols >= 0/*existe linha de tras*/)
    {
        if (current->payload.blobdata[i - current->payload.ncols] == 1)
        {
            current->payload.blobdata[i - current->payload.ncols] = blob_number;
            grassfire(i - current->payload.ncols, current, blob_number);
        }
    }
    return blob_number + 1;
}

void pushblob(blob_t **head, char *name, int xx, int yy, int blobsize, int id, float mr, float mg, float mb, float dR, float dG, float dB)
{
    blob_t *temp = malloc(sizeof(blob_t));
    strcpy(temp->belongsto, name);
    temp->xx = xx;
    temp->yy = yy;
    temp->blobsize = blobsize;
    temp->mR = mr;
    temp->mG = mg;
    temp->mB = mb;
    temp->dR = dR;
    temp->dG = dG;
    temp->dB = dB;
    temp->id = id;
    temp->next = *head;
    *head = temp;
}

int blob_destroy(blob_t **input_head)
{
    while (*input_head)
    {
        blob_t *node_of_death = *input_head;
        *input_head = (*input_head)->next;
        free(node_of_death);
    }
    return (0);
}

int printblob(blob_t *head)
{
    blob_t *current = head;
    int i = 0;
    while (current)
    {
        printf("%s | id: %d | position(%d,%d) | size : %d | dRGB : (%f,%f,%f) \n", current->belongsto, current->id, current->xx, current->yy, current->blobsize,(double)current->dR,(double)current->dG,(double)current->dB);
        current = current->next;
        i++;
    }
    printf("Total blobs (nodes): %d \n", i);
    return (0);
}

int xcentermass(list_node_t *current, int blob_number)
{
    float result = 0;
    int f = 0;
    float fx = 0, xsum = 0, N = 0;
    while (f <= current->payload.nlines * current->payload.ncols - 1)
    {
        if (current->payload.blobdata[f] == blob_number)
        {
            fx = f;
            while (fx >= current->payload.ncols)
                fx = fx - current->payload.ncols;
            xsum = xsum + fx;
            N++;
            fx = 0;
        }
        f++;
    }
    result = (float)round((double)xsum / (double)N);
    int   vara = (int)result;
    return (vara);
}

int ycentermass(list_node_t *current, int blob_number)
{
    float result = 0;
    int f = 0;
    float fy = 0, ysum = 0, N = 0, ft = 0;
    while (f <= current->payload.nlines * current->payload.ncols - 1)
    {
        if (current->payload.blobdata[f] == blob_number)
        {
            ft = f;
            while (ft >= current->payload.ncols)
            {
                ft = ft - current->payload.ncols;
                fy++;
            }
            ysum = ysum + fy;
            N++;
            fy = 0;
            ft = 0;
        }
        f++;
    }
    result = (float)round((double)ysum / (double)N);
    int   vara = (int)result;
    return (vara);
}

int sizeblob(list_node_t *current, int blob_number)
{
    int size = 0;
    int f = 0;
    while (f <= current->payload.nlines * current->payload.ncols - 1)
    {
        if (current->payload.blobdata[f] == blob_number)
            size++;
        f++;
    }
    return size;
}

float MR(list_node_t *current, int r, int g, int b, int d, int blob_number)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + current->payload.datatable[f];
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = valR / N;
    return (result);
}

float MG(list_node_t *current, int r, int g, int b, int d, int blob_number)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + current->payload.datatable[f + 1];
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = valR / N;
    return (result);
}

float MB(list_node_t *current, int r, int g, int b, int d, int blob_number)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + current->payload.datatable[f + 2];
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = valR / N;
    return (result);
}

void bubbleSort(blob_t *start)
{
    int swapped;
    blob_t *ptr1;
    blob_t *lptr = NULL;
    if (start == NULL)
        return;
    do
    {
        swapped = 0;
        ptr1 = start;
        while (ptr1->next != lptr)
        {
            if (ptr1->blobsize > ptr1->next->blobsize)
            {
                swapb(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void swapb(blob_t *a, blob_t *b)
{
    char chmap[cprompt_TOK_BUFSIZE];
    strcpy(chmap, a->belongsto);
    strcpy(a->belongsto, b->belongsto);
    strcpy(b->belongsto, chmap);
    int temp = a->blobsize;
    a->blobsize = b->blobsize;
    b->blobsize = temp;
    temp = a->xx;
    a->xx = b->xx;
    b->xx = temp;
    temp = a->yy;
    a->yy = b->yy;
    b->yy = temp;
    temp = a->id;
    a->id = b->id;
    b->id = temp;
    float temp2 = a->mR;
    a->mR = b->mR;
    b->mR = temp2;
    temp2 = a->mG;
    a->mG = b->mG;
    b->mG = temp2;
    temp2 = a->mB;
    a->mB = b->mB;
    b->mB = temp2;
}

int maxzonas(list_node_t *head)
{
    int max = 0;
    char champ[cprompt_TOK_BUFSIZE] = {"NO IMAGE"};
    list_node_t *current = head;
    if (current == NULL)
    {
        fprintf(stderr, "THE HEAD IS NULL\n");
        exit(EXIT_FAILURE);
    }
    while (current)
    {
        if (current->payload.numzonas >= max)
        {
            max = current->payload.numzonas;
            strcpy(champ, current->payload.name);
        }
        current = current->next;
    }
    printf("\nImage With Most Zones: %s | Number of Zones: %d\n\n", champ, max);
    return (max);
}

float desvioR(list_node_t *current, int r, int g, int b, int d, int blob_number, float mR)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + (float)pow(current->payload.datatable[f] - (double)mR, 2);
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = (float)sqrt((double)valR / (double)N);
    return (result);
}

float desvioG(list_node_t *current, int r, int g, int b, int d, int blob_number, float mG)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + (float)pow(current->payload.datatable[f + 1] - (double)mG, 2);
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = (float)sqrt((double)valR / (double)N);
    return (result);
}

float desvioB(list_node_t *current, int r, int g, int b, int d, int blob_number, float mB)
{
    float result = 0, valR = 0, N = 0;
    int i = 0;
    for (int f = 0; f <= current->payload.nlines * current->payload.ncols * current->payload.nchannels - 1; f++)
    {
        if (current->payload.blobdata[i] == blob_number)
        {
            if (current->payload.datatable[f] >= r - d && current->payload.datatable[f] <= r + d && current->payload.datatable[f + 1] >= g - d && current->payload.datatable[f + 1] <= g + d && current->payload.datatable[f + 2] >= b - d && current->payload.datatable[f + 2] <= b + d)
            {
                valR = valR + (float)pow(current->payload.datatable[f + 2] - (double)mB, 2);
                N++;
            }
        }
        f += 2;
        i++;
    }
    result = (float) sqrt((double)valR / (double)N);
    return (result);
}

int min_desvio(blob_t *head, list_node_t *thead)
{
    int ran = 0;
    blob_t *current = head;
    list_node_t *crt = thead;
    if (current == NULL || crt == NULL)
    {
        fprintf(stderr, "THE HEAD IS NULL--- NO BLOBS?\n");
        exit(EXIT_FAILURE);
    }
    char chompare[cprompt_TOK_BUFSIZE];
    strcpy(chompare, crt->payload.name);
    double mindR = -999;
    int idR = -999;
    double mindG = -999;
    int idG;
    double mindB = -999;
    int idB;
    do
    {
        while (current)
        {
            if (strcmp(chompare, current->belongsto) == 0)
            {
                if (ran == 0)
                {
                    mindR = (double)current->dR;
                    mindG = (double)current->dG;
                    mindB = (double)current->dB;
                    idR = current->id;
                    idG = current->id;
                    idB = current->id;
                    ran = 1;
                }
                if ((double)current->dR < mindR)
                {
                    mindR = (double)current->dR;
                    idR = current->id;
                }
                if ((double)current->dG < mindG)
                {
                    mindG = (double)current->dG;
                    idG = current->id;
                }
                if ((double)current->dB < mindB)
                {
                    mindB = (double)current->dB;
                    idB = current->id;
                }
            }
            current = current->next;
        }
        ran = 0;
        current = head;
        //
        printf("\n IMAGE : %s \n", chompare);
        printf("\nDesvio minimo R : %f | BLOB : %d ", mindR, idR);
        printf("\nDesvio minimo G : %f | BLOB : %d ", mindG, idG);
        printf("\nDesvio minimo B : %f | BLOB : %d \n\n", mindB, idB);
        //
        crt = crt->next;
        if (crt != NULL)
            strcpy(chompare, crt->payload.name);
    }
    while (crt);
    return (0);
}

int printblobdesvio(blob_t *head)
{
    blob_t *current = head;
    int i = 0;
    while (current)
    {
        printf("Blob belongs to : %s\n", current->belongsto);
        printf("with id :%d\n", current->id);
        printf("xx :%d\n", current->xx);
        printf("yy :%d\n", current->yy);
        printf("blobsize :%d\n", current->blobsize);
        printf("mR :%f\n", (double)current->mR);
        printf("mG :%f\n", (double)current->mG);
        printf("mB :%f\n", (double)current->mB);
        printf("dR :%f\n", (double)current->dR);
        printf("dG :%f\n", (double)current->dG);
        printf("dB :%f\n", (double)current->dB);
        printf("\n\n");
        current = current->next;
        i++;
    }
    printf("Total blobs (nodes): %d \n", i);
    return (0);
}

