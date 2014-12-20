clear all; close all;

n = 100; A = randn(n,n); [U,R] = qr(A);

%% Matrix 1
X1 = U * diag(ones(1,n)) * U'; b1 = ones(n,1);

%% Matrix 2
X2 = U * diag([ones(1,n/2) 1e-2*ones(1,n/2)]) * U'; b2 = ones(n,1);

%% Matrix 3
X3 = U * diag([1e2*ones(1,10) ones(1,n-10)]) * U'; b3 = ones(n,1);

