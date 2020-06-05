//input residual and size of the matrix, output phi^tilda^corr_gridsize. By method of inverse matrix.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>
#define PI acos(-1)
#include "basic.h"

extern const float L;
extern const double dx;
extern const int N;

// m is the current size of matrix
double comatrix(double *matrix, int p, int q, int m){
    double *comx;
    comx = (double *)malloc((m-1)*(m-1)*sizeof(double));
    for (int i=0;i<m-1;i++){
        for (int j=0;j<m-1;j++){    
            if(i<p && j<q) comx[ind(i,j,m-1)]=matrix[ind(i,j,m)];
            else if(i<p && j>q) comx[ind(i,j-1,m-1)]=matrix[ind(i,j,m)];
            else if(i>p && j<q) comx[ind(i-1,j,m-1)]=matrix[ind(i,j,m)];
            else if(i>p && j>q) comx[ind(i-1,j-1,m-1)]=matrix[ind(i,j,m)];
        }
    }
        
    return comx;
    
}

double det(double *matrix, int m){
    int D=0;
    int sign=1;
    if (m==1) return matrix[ind(0,0,m)];
    for (int f=0;f<m;f++){
        D += sign*matrix[ind(0,f,m)]*det(comatrix(matrix,0,f,m),m-1);
        sign =-sign;
    }
    return D;
}

double inversematrix(double *matrix, int n){
    double *invmx;
    invmx = (double *)malloc(n*n*sizeof(double));
    int sign=1;
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            sign=pow(-1, i+j);
            invmx[ind(i,j,n)]=sign*det(comatrix(matrix,j,i,n),n-1)/det(matrix,n);         
        }
    }
    return invmx;
}

void exact_im(double *phi_corr, double *residual, int n) {
    //turn into Au=b where u and b are tempararily vectors of phi_corr and residual
    //construct A
    double *A;
    const int n_ext = pow(n-2,2); // abbr for n_extension
    A = (double *)malloc(n_ext*n_ext*sizeof(double));
    for(int i=0;i<n_ext;i++)
	for(int j=0;j<n_ext;j++){
        if (i==j) A[ind(i, j, n_ext)]=4;
        else if (i==j+1||i==j-1||i==j+3||i==j-3) A[ind(i, j, n_ext)]=-1;
        else A[ind(i, j, n_ext)]=0;
    }
    //construct b
    double *b;
    b = (double *)malloc(n_ext*sizeof(double));
    for(int i=0;i<n-2;i++) {
        for(int j=0;j<n-2;j++){
            b[ind(i ,j, n-2)]=pow((N/n)*dx,2)*residual[ind(j+1, i+1, n)];
            if     (i==0 && j==0) b[ind(i ,j, n)] += residual[ind(0, 1, n)]+residual[ind(1, 0, n)];
            else if(i==0 && j==1) b[ind(i ,j, n)] += residual[ind(2, 0, n)];
            else if(i==0 && j==2) b[ind(i ,j, n)] += residual[ind(4, 1, n)]+residual[ind(3, 0, n)];
            else if(i==1 && j==0) b[ind(i ,j, n)] += residual[ind(0, 2, n)];
            else if(i==1 && j==2) b[ind(i ,j, n)] += residual[ind(4, 2, n)];
            else if(i==2 && j==0) b[ind(i ,j, n)] += residual[ind(0, 3, n)]+residual[ind(1, 4, n)];
            else if(i==2 && j==1) b[ind(i ,j, n)] += residual[ind(2, 4, n)];
            else if(i==2 && j==2) b[ind(i ,j, n)] += residual[ind(4, 3, n)]+residual[ind(3, 4, n)];
        }
    }
    //calculate A^(-1)
    double *A_inv;
    A_inv = (double *)malloc(n_ext*n_ext*sizeof(double));
    A_inv = inversematrix(A,n_ext)

    //calculate u=A^(-1)b
    double *phi_corr_nb;
    phi_corr_nb = (double *)malloc(n_ext*sizeof(double));
    for(int i=0;i<n_ext;i++){
	    for(int j=0;j<n_ext;j++){
            phi_corr_nb[ind(0,i,n_ext)]+=A_inv[ind(i,j,n_ext)]*b[ind(0,j,n_ext)]
        }
    }
    //phi_corr is now a matrix phi_corr[ind(i,j,n-2)]
    //paste the boundary
    for(int i=0;i<n;i++){
	    for(int j=0;j<n;j++){
            if( i==0 || j==0 || i==(n-1) || j==(n-1)) 
                phi_corr[ind(i,j,n)]=0;
            else 
                phi_corr[ind(i,j,n)]=phi_corr_nb[ind(i-1,j-1,n)];
        }
    }
}
