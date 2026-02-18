((nil . ((eval . (progn (add-to-list 'compilation-error-regexp-alist 'super-tux-logger)
                        (add-to-list 'compilation-error-regexp-alist-alist
                                     '(super-tux-logger
                                       ".*(\\([^:]*\\):\\(.*\\))$"
                                       1
                                       2
                                       )))))))
