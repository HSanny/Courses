function x = feasible(b, A, c, x_0)
[M, N] = size(A);
cvx_begin 
   variables s x(N)
   minimize s 
   subject to 
       A * x - b <= s * ones(M, 1)
       x >= zeros(N,1)
cvx_end
end