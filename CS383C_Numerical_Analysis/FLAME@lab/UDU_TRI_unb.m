function [ A_out ] = UDU_TRI_unb( A )
    
    % here we leave out the partion and repartion code in the notes

    %------------------------------------------------------------%
    
    alpha33 = alpha33 % = delta11 (no-op)
    l23 = alpha23 / alpha33; % scalar
    alpha22 = alpha22 - l23 * alpha23;
    alpha23 = l23;

    %------------------------------------------------------------%

    % here we leave out the continue code in the notes

return
