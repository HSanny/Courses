%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Scripts invoking cvx least-square routines to
%%% solve problems using our three datasets.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% solve least-square problem for first dataset
cvx_begin
    variable b1(size(X1,2))
    minimize( norm( X1*b1-y1 ) )
cvx_end
%% apply learned model on testing data
pred1 = X1test * b1;

RegressionError1 = norm( X1*b1-y1 )
TestingError1 = norm(pred1 - y1test)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% solve least-square problem for second dataset
cvx_begin
    variable b2(size(X2,2))
    minimize( norm( X2*b2-y2 ) )
cvx_end
%% apply learned model on testing data
pred2 = X2test * b2;

RegressError2 = norm( X2*b2-y2 )
TestError2 = norm(pred2 - y2test)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% solve least-square problem for second dataset
cvx_begin
    variable b3(size(X3,2))
    minimize( norm( X3*b3-y3 ) )
cvx_end
%% apply learned model on testing data
pred3 = X3test * b3;

RegressionError3 = norm( X3*b3-y3 )
TestingError3 = norm(pred3 - y3test)
