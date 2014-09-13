%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Greedy Coordinate Descent Algorithm with Backtrack Line Search
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Usage:
%    [b, iter, all_costs] = gcd_btls (b_init, f, fgrad, alpha, discount)
% Parameters:
%    b_init: starting search point of variable
%    f: objective function
%    fgrad: gradient of objective function
%    P: matrix that defines norm of steepest descent
%    alpha: parameter for evaluating "OK" step size
%    discount(beta): discounting factor for not-OK step size
% Note:
%    a) the minimal distinguisable value eps = 10e-16

function [b, iter, all_costs] = gcd_btls (b_init, f, fgrad, alpha, discount)
assert (discount > 0 && discount < 1);
assert (alpha > 0 && alpha < 0.5);
iter = 1; % iteration count
b = b_init;  % variable vector
ndim = size(b, 1);
last_cost = func(b);  % value of objective function in most recent iteration
all_costs = []; % all values of objective function, for plotting
while true,
    gradient = fgrad(b);
    waitlist = zeros(ndim, 1);
    gammalist = zeros(ndim, 1);
    for d = 1:ndim,
        %% compute essential numerics and do gradient descent
        delta_b = zeros (d, 1);
        delta_b(d) = -1.0 * gradient(d);
        %% do backtrack line search
        gamma = 1.0;
        while f(b+gamma*delta_b) > f(b) + alpha*gamma*gradient'*delta_b,
            gamma = discount * gamma;
        end
        waitlist(d) = f(b+gamma*delta_b);
        gammalist(d) = gamma;
    end
    [min_value, min_index] = min(waitlist);
    delta_b = zeros (d, 1);
    delta_b(min_index) = -1.0 * gradient(min_index);
    b = b + gammalist(min_index) * delta_b;
    cost = func(b);
    rate = (cost / last_cost);
    all_costs = [all_costs cost];
    %% output numeric information of this iteration
    disp (sprintf('Iter: %d, Cost: %e, Conv_Rate: %f, gamma: %f',iter,cost,rate,gamma));
    %% quadratic optimization converges to zero
    if abs((cost - last_cost) / last_cost) < eps, 
        disp('Convergence reached!')
        break
    end
    %% prepare for next iteration
    iter = iter + 1;
    last_cost = cost; 
end
%% uncomment following code for plotting individual gradient descent run
% plot f(b^(n)) with regard to n
plot (1:iter, all_costs)
title (sprintf ('Greedy Steepest Descent with BTLS (alpha: %f, beta: %f)',alpha,discount));
xlabel ('iteration: n');
ylabel ('cost of function: f(b^n)');
end
