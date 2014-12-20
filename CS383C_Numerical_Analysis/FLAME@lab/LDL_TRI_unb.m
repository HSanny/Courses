function [ A_out ] = LDL_TRI_unb( A )
    
    % here we leave out the partion and repartion code in the notes

    %------------------------------------------------------------%
    
    alpha11 = alpha11 % = delta11 (no-op)
    l21 = alpha21 / alpha11; % scalar
    alpha22 = alpha22 - l21 * alpha21;
    alpha21 =  l21;

    %------------------------------------------------------------%

    % here we leave out the continue code in the notes

return
