%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 5. Gradient descent and Newton on Rosenbrock function
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function Rosenbrock ()
    EPSILON = 10e-10; % how close solution do we need
    alpha = 0.1; 
    beta = 0.95;

    x_0 = [-1.2, 1]'; % initial point
    x_opt = [1 1]';

    [listK_GD, listError_GD] = gradient_descent (@func, @grad_func, ...
                                                 x_0, x_opt, alpha, beta);
    [listK_NM, listError_NM] = newton_method (@func, @grad_func, @hess_func, ...
                                                 x_0, x_opt, alpha, beta);

    plot (listK_GD, listError_GD, 'k-', listK_NM, listError_NM, 'k--')
    legend ('Gradient Descent', 'Newton Method')
    title('Gradient Descent and Newton Method for Rosenbrock function')
    xlabel('iterations: k')
    ylabel('log of Error: log(||x - x*||_M)')
end

%% gradient descent with BTLS
function [listK, listError] = gradient_descent (f, grad_f, x_0, x_opt, alpha, beta)
    listK = [];
    listError = [];
    x = x_0;  % initial point
    k = 0;
    while 1,
        Error = log(norm(x_opt-x, 2));

        fprintf ('iteration: %d, Error = %f \n', k, Error)
        listK = [listK k];
        listError = [listError Error];
        if k > 500,
            break
        end

        gradient = grad_f(x);
        delta_x = -1.0 * gradient;
        t = 1.0;
        while f(x+t*delta_x) > f(x) + alpha*t*gradient'*delta_x,
           t = beta * t;
        end
        x = x + t * delta_x;
        k = k + 1;
    end
end
%% newton method with BTLS
function [listK, listError] = newton_method (f, grad_f, hess_f, x_0, x_opt, alpha, beta)
    listK = [];
    listError = [];
    x = x_0;  % initial point
    k = 0;
    while 1,
        % Error = f(x);
        Error = log(norm(x_opt-x, 2));

        fprintf ('iteration: %d, Error = %f \n', k, Error)
        listK = [listK k];
        listError = [listError Error];
        if k > 50,
            break
        end

        gradient = grad_f(x);
        hessian = hess_f(x);
        delta_x = -1.0 * inv(hessian) * gradient;
        t = 1.0;
        while f(x+t*delta_x) > f(x) + alpha*t*gradient'*delta_x,
           t = beta * t;
        end
        x = x + t * delta_x;
        k = k + 1;
    end   
end

function [x_1, x_2] = parse (x)
    [m, n] = size(x);
    assert (m == 2 && n == 1);
    x_1 = x(1);
    x_2 = x(2);
end
function y = func (x)
    [x_1, x_2] = parse(x);
    y = 100 * (x_2 - x_1^2)^2 + (1-x_1)^2;
end
function y = grad_func (x)
    [x_1, x_2] = parse(x);
    y = zeros(2,1);
    y(1) = -400 * x_1 * (x_2-x_1^2) - 2 * (1-x_1);
    y(2) = 200 * (x_2 - x_1^2);
end
function y = hess_func (x)
    [x_1, x_2] = parse(x);
    y = zeros(2,2);
    y(1,1) = -400 * (x_2 - 3*x_1^2 ) + 2;
    y(1,2) = -400 * x_1;
    y(2,1) = y (1,2);
    y(2,2) = 200;
end
