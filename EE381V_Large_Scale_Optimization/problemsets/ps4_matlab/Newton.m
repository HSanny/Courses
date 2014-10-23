%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 2. Newton Method
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function Newton(m)
    EPSILON = 10e-10; % how close solution do we need
    x_opt = zeros(5, 1); % initial point
    t = 1; % step size fixed at 1
    x_0 = [100 -80 10 150 60]';

    listK = [];
    listlogMdiff = [];

    k = 0;
    x = x_0;
    while 1,
        diff = x - x_opt;
        logMdiff = log(diff' * diff);

        fprintf ('iteration: %d, log(||x - x*||_M) = %f \n', k, logMdiff)
        listK = [listK k];
        listlogMdiff = [listlogMdiff logMdiff];
        if k > 20, 
            break
        end
        
        grad = grad_func(x, m);
        hess = hess_func(x, m);
        x = x - t * inv(hess) * grad;
        
        x'
        k = k + 1;
    end
    plot (listK, listlogMdiff)
    title('Conjugate Gradient Solver for Ax = b')
    xlabel('iterations: k')
    ylabel('log of M-norm: log(||x - x*||_M)')
end

function val = func(x, m)
    normx = norm(x, 2);
    val = normx^3 + 0.5 * m * normx^2;
end

function val = grad_func(x, m)
    val = 3 * norm(x, 2) * x + m * x;
end

function val = hess_func(x, m)
    len = size(x,1);
    normx = norm(x, 2);
    val = (1.5 / normx) * x * x' + ( 1.5* normx + m) * eye(len, len);
end
