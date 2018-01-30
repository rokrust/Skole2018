% Scatter plot of positive and negative samples
function plot_scatter( x, y )
    positive = x(logical(y), :);
    negative = x(~logical(y), :);

    scatter(positive(:, end-1), positive(:, end), '+')
    hold on;
    scatter(negative(:, end-1), negative(:, end), 'o')

end

