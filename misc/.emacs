;; auto save to the same file not separate files
(auto-save-mode 1)

;; reload changed files automatically from disk
(global-auto-revert-mode 1)

;; remove the selected text when inserting new text
(delete-selection-mode 1)

;; auto complete package
(require 'package)
(add-to-list 'package-archives '("melpa" . "http://melpa.org/packages/") t)

(package-initialize)  ;load and activate packages, including auto-complete

(ac-config-default)

(global-auto-complete-mode t)


;; auto indent when yanking
(dolist (command '(yank yank-pop))
   (eval `(defadvice ,command (after indent-region activate)
            (and (not current-prefix-arg)
                 (member major-mode '(emacs-lisp-mode lisp-mode
                                                      clojure-mode    scheme-mode
                                                      haskell-mode    ruby-mode
                                                      rspec-mode      python-mode
                                                      c-mode          c++-mode
                                                      objc-mode       latex-mode
                                                      plain-tex-mode))
                 (let ((mark-even-if-inactive transient-mark-mode))
                   (indent-region (region-beginning) (region-end) nil))))))

;; highlight current line
(global-hl-line-mode 1)

;; show line numbers
(global-linum-mode t)
