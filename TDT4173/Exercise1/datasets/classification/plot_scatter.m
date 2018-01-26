function [ output_args ] = plot_scatter( x, y )
%PLOT_SCATTER Summary of this function goes here
%   Detailed explanation goes here

positive = x(logical(y), :);
negative = x(~logical(y), :);

scatter(positive(:, 2), positive(:, 3), '+')
hold on;
scatter(negative(:, 2), negative(:, 3), 'o')

end

