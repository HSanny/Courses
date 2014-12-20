%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 1. Conjugate Gradient Algorithm
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function CGS(M, b, x_opt)
    [R, C] = size(M);
    assert (R == C, 'M should be square matrix.');
    assert (R == size(b,1), 'Dim of M and b should be consistent.');

    EPSILON = 10e-10; % how close solution do we need
    x_0 = zeros(size(x_opt)); % initial point

    listK = [];
    listlogMdiff = [];

    k = 0;
    x = x_0;
    r = b - M * x; % residual
    p = r;
    while 1,
        diff = x - x_opt;
        logMdiff = log(diff' * M * diff);

        fprintf ('iteration: %d, log(||x - x*||_M) = %f \n', k, logMdiff)
        listK = [listK k];
        listlogMdiff = [listlogMdiff logMdiff];
        
        alpha = (r' * r) / (p' * M * p);
        x = x + alpha * p;
        r_new = r - alpha * M * p;
        if r_new < EPSILON,
            break
        end
        beta = (r_new' * r_new) / (r' * r);
        p = r_new + beta * p;
        r = r_new;
        k = k + 1;
    end
    plot (listK, listlogMdiff)
    title('Conjugate Gradient Solver for Ax = b')
    xlabel('iterations: k')
    ylabel('log of M-norm: log(||x - x*||_M)')
end
