#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "functions.h"

#define kRows 6
#define kColumns 6

void create_matrix(double *** m, int rows, int columns) {
    *m = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; ++i) {
        (*m)[i] = (double *)malloc(columns * sizeof(double));
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            (*m)[i][j] = 0.0;
        }
    }
}

void delete_matrix(double *** m, int rows, int columns) {
    for (int i = 0; i < rows; ++i) {
        free((*m)[i]);
    }
    free(*m);
    m = NULL;
}

double ** matrix_summation(double ** m1, int rows1, int columns1, double ** m2, int rows2, int columns2) {
    if (rows1 != rows2 || columns1 != columns2) {
        return NULL;
    }
    double ** new_matrix;
    create_matrix(&new_matrix, rows1, columns1);

    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < columns1; ++j) {
            new_matrix[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return new_matrix;
}

void mult_matrix_by_number(double *** m, int rows, int columns, double value) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            (*m)[i][j] = (*m)[i][j] * value;
        }
    }
}

double ** matrix_mult(double *** m1, int rows1, int columns1, double *** m2, int rows2, int columns2) {
    if (columns1 != rows2) {
        return NULL;
    }
    double ** new_matrix;
    create_matrix(&new_matrix, rows1, columns2);

    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < columns1; ++j) {
            new_matrix[i][j] = (*m1)[i][j] * (*m2)[j][i];
        }
    }

    return new_matrix;
}

double matrix_determinant(double *** m, int rows, int columns) {
    int degree = 1; // (-1) ^ n
    if (rows != columns) {
        return 0;
    }
    double result = 0.0;

    if (rows == 2) {
        result = (*m)[0][0] * (*m)[1][1] - ((*m)[1][0] * (*m)[0][1]);
        return result;
    }

    for (int i = 0; i < rows; ++i) {
        result = result + degree * (*m)[i][0] * matrix_determinant(m, rows, columns);
        degree = -degree;
    }

    return result;
}

void input_matrix(double *** m, int rows, int columns) {
    for (int i = 0; i < rows; ++i) {
        printf("Enter numbers of the %d row:\n", i+1);
        for (int j = 0; j < columns; ++j) {
            double tmp_val;
            scanf("%le",&tmp_val);
//            (*m)[i][j]=tmp_val;
        }
    }
}

void print_matrix(double *** m, int rows, int columns) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf("%f",(*m)[i][j]);
        }
        printf("\n");
    }
}

int main() {
    double **B;
    double **A;
    input_matrix(&A, 3, 3);
    input_matrix(&B, 3, 3);
    double **C = matrix_mult(&A, 3, 3,&B, 3, 3);
    print_matrix(&C, 3, 3);
    delete_matrix(&B, 3, 3);
    
    return 0;
}
