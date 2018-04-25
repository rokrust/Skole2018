function E = l_simple(w)
    E = (logsig(w(1, :))-1).^2 + logsig(w(2, :)).^2 + (logsig(sum(w, 2))-1).^2;
end