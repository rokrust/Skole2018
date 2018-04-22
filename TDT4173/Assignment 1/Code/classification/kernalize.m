% Employ a kernel function that yields the second data set linearly
% separable
function [ x_kernel ] = kernalize( x )
    x_kernel = x(:, 2:3) - 0.5;
    x_kernel = sqrt(sum(x_kernel .^ 2, 2));
    x_kernel = [x(:, 1) x_kernel];
    
end

