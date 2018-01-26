function [ is_sep ] = is_linearly_separable( x, w, y )
%IS_LINEARLY_SEPER Summary of this function goes here
%   Detailed explanation goes here
positive = x(logical(y), :);
negative = x(~logical(y), :);

is_sep = min(positive*w) >= 0 && max(negative*w) < 0;

end

