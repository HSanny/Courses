%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 3. Central Path
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function x = CP (t_init, alpha, marker)
    % acquired global optima from cvx command
    x_opt = zeros(4, 1);
    % create barrier system of equation
    A = [1 3 0 1; 2 1 0 0; 0 1 4 1]; 
    b = [4 3 3]';
    c = [2 4 1 1]';
    % find analytical center
    x_F_init = [0.2, 0.2, 0.2, 0.2]';
    t = 0;
    x_F = Newton_Solver (b, A, x_F_init, t, c);
    % generate a central path
    listK = [0];
    listLogNorm2 = [log(norm(x_F-x_opt, 2))];

    k = 1;
    t = t_init;
    x = x_F;
    while 1,
        x = Newton_Solver (b, A, x, t, c);

        logNorm2 = log(norm(x - x_opt, 2));
        listK = [listK k];
        listLogNorm2 = [listLogNorm2 logNorm2];

        if norm(x - x_opt, 2) < 1e-5,
            break
        end
        t = t * (1 + alpha);
        k = k + 1;
    end
    semilogx (listK, listLogNorm2, marker)
    xlabel('iterations: k', 'fontsize', 18)
    ylabel('log( || x - x* ||_2 )', 'fontsize', 18)
    title('Central Path for the large problem', 'fontsize', 18)
    set(gca, 'fontsize', 18)
end
