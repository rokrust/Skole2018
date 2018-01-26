%Read data
training_data1 = csvread('cl_train_1.csv');
test_data1 = csvread('cl_test_1.csv');

eta = 0.01;      %Learning rate

[n_train, d] = size(training_data1(:, 1:end-1));
x_train = [ ones(n_train, 1), training_data1(:, 1:end-1) ];
y_train = training_data1(:, end);

[n_test, d] = size(test_data1(:, 1:end-1));
x_test = [ ones(n_test, 1), test_data1(:, 1:end-1) ];
y_test = test_data1(:, end);

w = zeros(d+1, 1);
E_train = zeros(1000, 1);
E_test = zeros(1000, 1);

for k = 1:1000
    gradient = (sigmoid(x_train * w) - y_train)' * x_train;
    w = w - eta * gradient';
    
    E_train(k) = cross_entropy(y_train, x_train * w);
    E_test(k) = cross_entropy(y_test, x_test * w);
end

%% See if the samples have been correctly classified
%{
%Training
classes = sigmoid(x_train * w) > 0.5;
any(~(classes == y_train))

%Test
classes = sigmoid(x_test * w) > 0.5;
any(~(classes == y_test))
%}
%% Plot cross entropy
plot(E_train)
hold on;
plot(E_test)
