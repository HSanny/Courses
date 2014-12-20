%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Orthogonal matching Pursuit
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function Iset = omp (X, y, SPARSITY)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% INITIALIZATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[target_feat_dot_prod, target_feat_idx] = max(X' * y);
Iset = [target_feat_idx];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% AUGMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
residual = y;
for iter = 1:(SPARSITY-1),
    % perpendicular complement of y to X_i
    phi = X(:, Iset);
    P = phi * inv(phi'*phi) * phi';
    I = eye(size(P));
    residual = (I - P) * residual; 
    % elect new atom and add to selected atom set
    [target_feat_dot_prod, target_feat_idx] = max(X' * residual);
    % NOTE that new feature(atom) will not pre-exist in Iset
    % This is theoreotically guaranteed by orthogonal projection
    Iset = [Iset, target_feat_idx];
end
end
