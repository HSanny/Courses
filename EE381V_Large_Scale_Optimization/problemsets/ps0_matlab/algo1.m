%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Scripts invoking cvx least-square routines to
%%% solve problems using our three datasets.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% standard least-square for Small-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b1(size(X1,2))
    minimize( norm( X1*b1-y1 ) )
cvx_end

RegressionError1 = norm( X1*b1-y1 )
TestingError1 = norm( X1test*b1 - y1test )

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% standard least-square for Medium-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b2(size(X2,2))
    minimize( norm( X2*b2 - y2 ) )
cvx_end

RegressError2 = norm( X2*b2 - y2 )
TestError2 = norm( X2test*b2 - y2test )

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% standard least-square for Large-scale dataset
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cvx_begin
    variable b3(size(X3,2))
    minimize( norm( X3*b3-y3 ) )
cvx_end

RegressionError3 = norm( X3*b3 - y3 )
TestingError3 = norm( X3test*b3 - y3test )
