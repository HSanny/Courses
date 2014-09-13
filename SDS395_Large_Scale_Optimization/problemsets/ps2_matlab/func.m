%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% objective function for optimization
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Input vector should be column vector

function y = func (x)
assert (size(x, 1) == 2);
assert (size(x, 2) == 1);
x_1 = x(1);
x_2 = x(2);
term1 =    x_1 +3*x_2 -0.1;
term2 =    x_1 -3*x_2 -0.1;
term3 = -1*x_1        -0.1;
y = exp(term1) + exp(term2) + exp(term3);
end
