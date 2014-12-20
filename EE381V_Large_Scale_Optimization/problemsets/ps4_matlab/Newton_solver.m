%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 3. General Newton's method solver for Central Path problem
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function x_opt = Newton_Solver (b, A, x_0, t, c)
    eta = 0.1;
    k = 0;
    x = x_0;
    x_opt = x;
    beta = 0.8;
    while k < 100,
        grad = grad_func(b, A, x, t, c);
        hess = hess_func(b, A, x, t, c);
        delta_x = - inv(hess) * grad;
        x = x + eta * delta_x;
        if norm(x-x_opt, 2)/norm(x, 2) <= 1e-3 , 
            break;
        else 
            k = k + 1;
            x_opt = x;
        end
    end
    x_opt = x;
end
function val = func (b, A, x, t, c)
    N = size(x, 1); % number of variables
    C = size(A, 1); % number of constraint
    val = 0.0;
    for i = 1:N,
        val = val - log(x(i));
    end
    for i = 1:C,
        val = val - log(b(i) - A(i,:)*x);
    end
    val = val + t* c' * x;
end
function val = grad_func (b, A, x, t, c)
    N = size(x, 1);
    C = size(A, 1); % number of constraint
    val = zeros(N, 1);
    val = val - 1 ./ x;
    for i = 1:C,
        val = val - (1 / (b(i) - A(i,:)*x)) * (-A(i,:)');
    end 
    val = val + t * c;
end
function val = hess_func (b, A, x, t, c)
    N = size(x, 1);
    C = size(A, 1); % number of constraint
    val = zeros(N, N);
    for i = 1:N,
        val(i, i) = val(i, i)+ (1/x(i))^2;
    end
    for i = 1:C,
        val = val + (1 / (b(i) - A(i,:)*x))^2 * (A(i,:)*A(i,:)');
    end 
end
