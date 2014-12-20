%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 2. Newton Method
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function Newton(m, marker)
    x_opt = zeros(5, 1); % initial point
    t = 1; % step size fixed at 1
    x_0 = [0.5 0.5 0.5 0.5 0.5]';

    listK = [];
    listlogMdiff = [];

    k = 0;
    x = x_0;
    while 1,
        diff = x - x_opt;
        logMdiff = log(norm(diff,2)^2);

        fprintf ('iteration: %d, log(||x - x*||_2^2) = %f \n', k, logMdiff)
        listK = [listK k];
        listlogMdiff = [listlogMdiff logMdiff];

        grad = grad_func(x, m);
        hess = hess_func(x, m);
        x = x - t * inv(hess) * grad;
        
        k = k + 1;
        %if grad' * hess * grad <= eps, 
        if logMdiff < -40,
            break
        end
    end
    plot (listK, listlogMdiff, marker)
    title('Newton Method for f_m', 'fontsize', 18)
    xlabel('iterations: k', 'fontsize', 18)
    ylabel('log of M-norm: log(||x - x*||_M)', 'fontsize', 18)
    set(gca, 'fontsize', 18)
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
    val = (3 / normx) * x * x' + ( 3* normx + m) * eye(len, len);
end
