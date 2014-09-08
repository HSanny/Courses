%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Scripts invoking cvx least-square routines to
%%% solve LASSO problems using our three datasets.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

format short e
EPSILON = 10e-5;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% LASSO least-square for Small-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b1(size(X1,2))
    minimize( norm( X1*b1-y1 ) + norm(b1,1) )
cvx_end

RegressionError1 = norm( X1*b1-y1 )
TestingError1 = norm( X1test * b1 - y1test )
Support1 = sum(((b1 < EPSILON) + (b1 > -EPSILON)) < 2)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% LASSO least-square for Medium-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b2(size(X2,2))
    minimize( norm( X2*b2-y2 ) + norm(b2, 1))
cvx_end

RegressionError2 = norm( X2*b2-y2 )
TestingError2 = norm( X2test * b2 - y2test )
Support2 = sum(((b2 < EPSILON) + (b2 > -EPSILON)) < 2)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% LASSO least-square for Large-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b3(size(X3,2))
    minimize( norm( X3*b3-y3 ) + norm(b3, 1) )
cvx_end

RegressionError3 = norm( X3*b3-y3 )
TestingError3 = norm( X3test * b3 - y3test )
Support3 = sum(((b3 < EPSILON) + (b3 > -EPSILON)) < 2)
