%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 3. Central Path for the large linear program
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function x = CP_large (c, A, b, t_init, alpha, marker)
    % acquired global optima from cvx command
    [M, N] = size(A);
    x_opt = CVX_solve_CP_large (c, A, b)

    % find feasible starting point 
    %x_F_init = feasible(b, A, c, ones(50, 1)');
    x_F_init = 10e-9 * ones(50, 1);
    % find analytical center
    x_F = Newton_Solver (b, A, x_F_init, 0, c')
    % generate a central path
    listK = [0];
    listLogNorm2 = [log(norm(x_F-x_opt, 2))];

    k = 1;
    t = t_init;
    x = x_F;
    while k < 50,
        x = Newton_Solver (b, A, x, t, c');

        logNorm2 = log(norm(x - x_opt, 2));
        listK = [listK k];
        listLogNorm2 = [listLogNorm2 logNorm2];

        if norm(x - x_opt, 2) < 1e-5,
            break
        end
        t = t * (1 + alpha);
        k = k + 1;
    end
    plot (listK, listLogNorm2, marker)
    xlabel('iterations: k', 'fontsize', 18)
    ylabel('log( || x - x* ||_2 )', 'fontsize', 18)
    title('Central Path for the large problem', 'fontsize', 18)
    set(gca, 'fontsize', 18)
end
