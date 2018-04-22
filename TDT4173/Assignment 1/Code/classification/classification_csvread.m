function [ X, y ] = classification_csvread( file_name )
    data = csvread(file_name);
    n = size(data, 1);

    X = [ones(n, 1), data(:, 1:end-1)];
    y = data(:, end);

end

