function x = CVX_solve_CP_large (c, A, b)
[m, n] = size(A);
zero_vec = zeros(n, 1);
cvx_begin
    variable x(n)
    minimize c * x
    subject to
        A * x <= b;
        x >= zero_vec;
cvx_end
end
