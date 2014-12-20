%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% HW2: Gradient Descent 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Usage:
%    [b, iter, all_costs] = gd_blts (b_init, f, fgrad, alpha, discount)
% Parameters:
%    b_init: inital value of variable
%    f: objective function 
%    fgrad: gradient of objective function
%    alpha: parameter for evaluating "OK" step size
%    discount: discounting factor for not-OK step size

function [b, iter, all_costs] = gd_blts (b_init, f, fgrad, alpha, discount)
assert (discount > 0 && discount < 1);
assert (alpha > 0 && alpha < 0.5);
eps = 10e-5; % optimality tolerance
iter = 1; % iteration count
gamma = 1.0;  % step size
b = b_init;  % variable vector
last_cost = func(b);  % value of objective function in most recent iteration
all_costs = []; % all values of objective function, for plotting
while true,
    %% compute essential numerics and do gradient descent
    gradient = fgrad(b);
    delta_b = -1.0 * gradient / norm(gradient);
    %% do backtrack line search
    while f(b+gamma*delta_b) > f(b) + alpha*gamma*gradient'*delta_b,
        gamma = discount * gamma;
        disp(sprintf('BTLS: new gamma is %f', gamma));
    end
    b = b + gamma * delta_b;
    cost = func(b);
    rate = (cost / last_cost);
    all_costs = [all_costs cost];
    %% output numeric information of this iteration
    disp (sprintf('Iter: %d, Cost: %e, Conv_Rate: %f',iter,cost,rate));
    %% quadratic optimization converges to zero
    if abs((cost - last_cost) / last_cost) < eps, 
        disp('Convergence reached!')
        break
    end
    %% prepare for next iteration
    last_cost = cost; 
    iter = iter + 1;
end
%% uncomment following code for plotting individual gradient descent run
% plot f(b^(n)) with regard to n
plot (1:iter, all_costs)
title (sprintf ('Standard Gradient Descent with BTLS'));
xlabel ('iteration: n');
ylabel ('cost of function: f(b^n)');
end
