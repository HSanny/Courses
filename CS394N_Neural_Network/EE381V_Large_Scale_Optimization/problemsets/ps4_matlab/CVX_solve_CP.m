function x = CVX_solve_CP ()
zero_vec = zeros(4, 1);
cvx_begin
    variable x(4)
    minimize 2*x(1) + 4*x(2) + x(3) + x(4)
    subject to
        x(1) + 3*x(2) + x(4) <= 4;
        2*x(1) + x(2) <= 3;
        x(2) + 4*x(3) + x(4) <= 3;
        x >= zero_vec;
cvx_end
end
