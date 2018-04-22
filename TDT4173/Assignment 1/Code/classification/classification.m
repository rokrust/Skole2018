%% First data set
%Read data
[ x_train, y_train ] = classification_csvread('cl_train_1.csv');
[ x_test, y_test ] = classification_csvread('cl_test_1.csv');
dim = size(x_train, 2);

eta = 0.1;      %Learning rate
w = zeros(dim, 1);
E_train = zeros(1000, 1);
E_test = zeros(1000, 1);

for k = 1:1000
    gradient = (sigmoid(x_train * w) - y_train)' * x_train;
    w = w - eta * gradient';
    
    E_train(k) = cross_entropy(y_train, x_train * w);
    E_test(k) = cross_entropy(y_test, x_test * w);
end


%% Second data set
%Read data
[ x_train, y_train ] = classification_csvread('cl_train_2.csv');
[ x_test, y_test ] = classification_csvread('cl_test_2.csv');

x_train = kernalize(x_train);
x_test = kernalize(x_test);
dim = size(x_train, 2);

eta = 0.1;      %Learning rate
w = zeros(dim, 1);
E_train = zeros(1000, 1);
E_test = zeros(1000, 1);

for k = 1:1000
    gradient = (sigmoid(x_train * w) - y_train)' * x_train;
    w = w - eta * gradient';
    
    E_train(k) = cross_entropy(y_train, x_train * w);
    E_test(k) = cross_entropy(y_test, x_test * w);
end


%% Plot data points
figure(1)
plot_scatter(x_train, y_train);
plot_scatter(x_test, y_test);

%Plot decision boundary
%x = -w(1)/w(2):0.01:1;
%y = (-x.*w(2)-w(1))./w(3);
%plot(x, y);

x = 0:0.1:2;
n = size(x, 2);
sep = -w(1)/w(2)*ones(n, 1);
plot(x, sep);


%% Plot cross entropy
figure(2)
plot(E_train)
hold on;
plot(E_test)
