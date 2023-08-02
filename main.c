#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

//шаблоны:
int glider[9]={0,1,0, 0,0,1, 1,1,1};
int blinker[9]={0,1,0, 0,1,0, 0,1,0};
int toad[16]={0,0,1,0, 1,0,0,1, 1,0,0,1, 0,1,0,0};


void print_menu(){
    printf("Choose structure:\n");
    printf("1. Glider\n");
    printf("2. Blinker\n");
    printf("3.Toad\n");
}

typedef struct{
    int** grid;
    int rows;
    int cols;
} GameField;


GameField *CreateGameField(int rows, int cols){     //создаем поле и выделяем память
    GameField *field=malloc(sizeof(GameField));
    field->rows = rows;
    field->cols = cols;

    field->grid=malloc(sizeof(int*) * rows);
    for(size_t i=0;i<rows;i++){
        field->grid[i]=calloc(cols, sizeof(int));
    }
    return field;
}

void free_game_field(GameField *field){            //освобождение памяти
    for(int i=0;i<field->rows;i++){
        free (field->grid[i]);
    }
    free (field->grid);
}

void fill_game_field(GameField *field){        //заполняем случайными значениями
    for(int i=0;i<field->rows;i++){
        for(int j=0;j<field->cols;j++){
            field->grid[i][j]=rand()%2;
        }
    }
}

void print_field(GameField *field){               //печать игрового поля
    for(int i=0;i<field->rows;i++){
        for(int j=0;j<field->cols;j++){
            printf("%c",field->grid[i][j] ? '#' : ' ');
        }
        printf("\n");
    }
    fflush(stdout);
}

void apply_rules(GameField *field){
    int **new_grid=malloc(field->rows * sizeof(int*));
    for(int i=0;i<field->rows;i++){
        new_grid[i]=malloc(field->cols * sizeof(int));
        for(int j=0;j<field->cols;j++){
            int alive_neighbours=0;
            for(int di=-1;di<=1;di++){
                for(int dj=-1;dj<=1;dj++){
                    if(di==0 && dj==0) continue;
                    int ni=i+di;
                    int nj=j+dj;
                    if(ni>=0 && ni<field->rows && nj>=0 && nj<field->cols){
                        alive_neighbours +=field->grid[ni][nj];         
                    }
                }
            }
            if(field->grid[i][j] && (alive_neighbours==2 || alive_neighbours==3))
                new_grid[i][j]=1;
            else if(!field->grid[i][j] && alive_neighbours==3)
                new_grid[i][j]=1;
            else
                new_grid[i][j]=0;
        }    
    }    
    for(int i = 0;i<field->rows;i++){
        free(field->grid[i]);
        field->grid[i]=new_grid[i];
    }
    free(new_grid);  
}

void user_initialize(GameField *field){
    int row,col;
    char response;

    while(1){
        printf("Enter coordinates of a live cell (row and column) or 'q' to quit: ");
        if(scanf("%d %d",&row,&col)==2){
            if(row >=0 && row<field->rows && col>=0 && col<field->cols){
                field->grid[row][col]=1;
            }
            else{
                printf("Invalid coordinates. Please try again.\n");
            }
        }
        else{
            response=getchar();
            while((getchar()) != '\n' && response!=EOF)
                ;
            if(response=='q' || response=='Q') break;
            printf("Invalid output. Please enter two integers for row and column.");
        }
    }
}

void initialize_with_pattern(GameField *field, int* pattern, int rows, int cols) {
    // копирование шаблонов в центр поля
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            field->grid[field->rows/2 + i - rows/2][field->cols/2 + j - cols/2] = pattern[i*cols + j];
        }
    }
}


void initialize_game_field(GameField *field){
    char UserChoice;

    printf("Do you want to fill the field manually? (y/n): ");
    scanf(" %c", &UserChoice);

    if(UserChoice == 'y' || UserChoice == 'Y'){
        printf("Do you want to choose template? (y/n): ");
        scanf(" %c", &UserChoice);
        if(UserChoice == 'y' || UserChoice == 'Y'){
            print_menu();
            int choice;
            scanf("%d",&choice);

            switch(choice){
                case 1:
                    initialize_with_pattern(field,glider,3,3);
                    break;
                case 2:
                    initialize_with_pattern(field,blinker,3,3);
                    break;
                case 3:
                    initialize_with_pattern(field,toad,4,4);
                    break;
            }
        }
        else
            user_initialize(field);
    }
    else
        fill_game_field(field);
}

void scan_rows_cols(int* rows,int* cols){
    do{
        printf("Enter the number of rows: ");
        scanf("%d",rows);
        printf("Enter the number of columns: ");
        scanf("%d",cols);

        if(*rows<=5 || *cols<=5){
            printf("Field must be at least 5x5\n");
        }
    }while(*rows<5 || *cols<5);
}

bool has_alive_cells(GameField *field){
    for(int i=0;i<field->rows;i++){
        for(int j=0;j<field->cols;j++){
            if(field->grid[i][j]==1){
                return true;
            }
        }
    }
    return false;
}

void run_field(GameField* field){
    for(int i=1;i<=20;i++){
        system("clear");
        printf("Generation: %d\n",i);
        fflush(stdout);
        print_field(field);
        apply_rules(field); 
        if(!has_alive_cells(field)){
            printf("All cells are dead. Game over.\n");
            break;
        }  
        sleep(1);    
    }    
}

int main(){
    int rows;
    int cols;
    scan_rows_cols(&rows,&cols);
    GameField* field = CreateGameField(rows,cols);
    initialize_game_field(field);
    run_field(field);
    free_game_field(field);
    return 0;
}