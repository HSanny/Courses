%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% gradient of function EQ. 9.20 in B & V
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Usage: 
%    gradient = func_grad (x)
% Parameter:
%    b: variable vector

function gradient = func_grad (x)
assert(all(size(x)==[2 1]))
x_1 = x(1);
x_2 = x(2);
grad_1 = exp(x_1+3*x_2-0.1) + exp(x_1-3*x_2-0.1) -1*exp(-1*x_1-0.1);
grad_2 = 3*exp(x_1+3*x_2-0.1) -3*exp(x_1-3*x_2-0.1);
gradient = [grad_1 grad_2]';
end
