%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Invoke CVX least square regression after OMP
%%% feature selection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

SPARSITY = 5; % SPARSITY parameter for OMP

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Small-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic
Iset1 = omp(X1, y1, SPARSITY);
subX1 = X1(:, Iset1);
cvx_begin
    variable sub_b1(SPARSITY);
    minimize( norm(subX1 * sub_b1 - y1) )
cvx_end
toc

Iset1
RegressionError1 = norm(subX1*sub_b1 - y1)
TestingError1 = norm(X1test(:,Iset1)*sub_b1 - y1test)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Medium-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic
Iset2 = omp(X2, y2, SPARSITY);
subX2 = X2(:, Iset2);
cvx_begin
    variable sub_b2(SPARSITY);
    minimize( norm(subX2 * sub_b2 - y2) )
cvx_end
toc

Iset2
RegressionError2 = norm(subX2*sub_b2 - y2)
TestingError2 = norm(X2test(:,Iset2)*sub_b2 - y2test)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Large-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic
Iset3 = omp(X3, y3, SPARSITY);
subX3 = X3(:, Iset3);
cvx_begin
    variable sub_b3(SPARSITY);
    minimize( norm(subX3 * sub_b3 - y3) )
cvx_end
toc

Iset3
RegressionError3 = norm(subX3*sub_b3 - y3)
TestingError3 = norm(X3test(:,Iset3)*sub_b3 - y3test)
