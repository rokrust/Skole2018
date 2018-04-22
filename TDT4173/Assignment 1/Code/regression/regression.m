%% Two dimensional data
%Find the design matrix and the output vector from the csv data
[ X_train, y_train ] =  regression_csvread('train_2d_reg_data.csv');
[ X_test, y_test ] =    regression_csvread('test_2d_reg_data.csv');

n_test =    size(y_test, 1);
n_train =   size(y_train, 1);

%Find regression weights (First part of the task)
w = pinv(X_train)*y_train;

%MSE for training and test set
test_err =      X_test * w - y_test;
train_err =     X_train * w - y_train;
E_mse_train =   train_err'*train_err / n_train;
E_mse_test =    test_err'*test_err / n_test;


%% One dimensional data
%Find the design matrix and the output vector from the csv data
[ X_train, y_train ] =  regression_csvread('train_1d_reg_data.csv');
[ X_test, y_test ] =    regression_csvread('test_1d_reg_data.csv');

n_test =    size(y_test, 1);
n_train =   size(y_train, 1);

%Find regression weights (First part of the task)
w = pinv(X_train)*y_train;

%MSE for training and test set
test_err =      X_test * w - y_test;
train_err =     X_train * w - y_train;
E_mse_train =   train_err'*train_err / n_train;
E_mse_test =    test_err'*test_err / n_test;

%Plot estimated function and training data points
plot(X_train(:, 2), y_train, 'r*')
hold on;
x = 0:0.01:1;
y = [ones(101, 1), x']*w;
plot(x, y);