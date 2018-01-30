% Make the X and y matrices from csv-files
function [ X, y ] = regression_csvread( file_name )
    training_data = csvread(file_name);

    n = size(training_data, 1);

    X = [ones(n, 1), training_data(:, 1:end-1)];
    y = training_data(:, end);

end