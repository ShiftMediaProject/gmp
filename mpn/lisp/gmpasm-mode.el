;;; gmpasm-mode.el -- GNU MP asm and m4 editing mode.


;; Copyright (C) 1999-2000 Free Software Foundation, Inc.
;;
;; This file is part of the GNU MP Library.
;;
;; The GNU MP Library is free software; you can redistribute it and/or modify
;; it under the terms of the GNU Library General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or (at your
;; option) any later version.
;;
;; The GNU MP Library is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
;; License for more details.
;;
;; You should have received a copy of the GNU Library General Public License
;; along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
;; the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


;;; Commentary:
;;
;; gmpasm-mode selects m4-mode and then makes some improvements and
;; alterations.  See the `gmpasm-mode' docstring and the code for details.
;;
;; m4-mode-hook is run when gmpasm-mode selects m4-mode, and then
;; gmpasm-mode-hook is run too.
;;
;;
;; Install
;; -------
;;
;; To make M-x gmpasm-mode available, put gmpasm-mode.el somewhere in your
;; load-path and the following in your .emacs
;;
;;	(autoload 'gmpasm-mode "gmpasm-mode" nil t)
;;
;; To use gmpasm-mode automatically on all .asm and .m4 files, put the
;; following in your .emacs
;;
;;	(add-to-list 'auto-mode-alist '("\\.asm\\'" . gmpasm-mode))
;;	(add-to-list 'auto-mode-alist '("\\.m4\\'" . gmpasm-mode))
;;
;; To have gmpasm-mode only on gmp files, try instead something like the
;; following, which uses it only in a directory starting with "gmp", or a
;; sub-directory of such.
;;
;;	(add-to-list 'auto-mode-alist
;;	             '("/gmp.*/.*\\.\\(asm\\|m4\\)\\'" . gmpasm-mode))
;;
;; Byte compiling will slightly speed up loading.  If you want a docstring
;; in the autoload you can use M-x update-file-autoloads if you set it up
;; right.
;;
;;
;; Emacsen
;; -------
;;
;; FSF Emacs 20.x - gmpasm-mode is designed for this.
;; FSF Emacs 19.34 - works if you get a copy of m4-mode.el and chop the
;;                   20.x-isms out of it.
;; XEmacs 20.4 - seems to work.


;;; History:
;;
;; Jan 2000 - Initial version.


;;; Code:

(require 'm4-mode)

(defvar gmpasm-mode-hook nil
  "*Hook to run after entering `gmpasm-mode'.")


(defun gmpasm-add-to-list-second (list-var element)
  "(gmpasm-add-to-list-second LIST-VAR ELEMENT)

Add ELEMENT to LIST-VAR as the second element in the list, if it isn't
already in the list.  If LIST-VAR is nil, then ELEMENT is just added as the
sole element in the list.

This is like `add-to-list', but it puts the new value second in the list.

The first cons cell is copied rather than changed in-place, so references to
the list elsewhere won't be affected."

  (if (member element (symbol-value list-var))
      (symbol-value list-var)
    (set list-var
	 (if (symbol-value list-var)
	     (cons (car (symbol-value list-var))
		   (cons element
			 (cdr (symbol-value list-var))))
	   (list element)))))


(defun gmpasm-delete-from-list (list-var element)
  "(gmpasm-delete-from-list LIST-VAR ELEMENT)

Delete ELEMENT from LIST-VAR, using `delete'.
This is like `add-to-list', but the element is deleted from the list.
The list is copied rather than changed in-place, so references to it elsewhere
won't be affected."

  (set list-var (delete element (copy-sequence (symbol-value list-var)))))


(defvar gmpasm-mode-map
  (let ((map (make-sparse-keymap)))
    (set-keymap-parent map m4-mode-map)

    ;; C-c C-k is a way to kill an M-x compile, since it's not hard to make
    ;; m4 go into an infinite loop.
    (define-key map "\C-c\C-k" 'kill-compilation)

    ;; C-c C-d is dnl commenting, as opposed to C-c C-c which is assembler
    ;; commenting (# ; ! or @).
    (define-key map "\C-c\C-d" 'gmpasm-comment-region-dnl)

    map)
  "Keymap for `gmpasm-mode'.  `m4-mode-map' is inherited.")


(defvar gmpasm-mode-syntax-table
  (let ((table (copy-syntax-table m4-mode-syntax-table)))

    ;; Don't want # treated as a comment in the syntax table because it can
    ;; foul up sexp motion and bracket and quote matching across # (both
    ;; with $# parameters in m4 defines, or assembler # comments inside
    ;; ifdef()s or forloop()s).  m4 interprets # as a comment only when
    ;; outside quotes, and the syntax table can't express that.
    (modify-syntax-entry ?#  "w" table)
    (modify-syntax-entry ?\n " " table)

    ;; Prefer underscore as a symbol char, like C mode.
    (modify-syntax-entry ?_  "_" table)

    table)
  "Syntax table used in `gmpasm-mode'.
Initialized from `m4-mode-syntax-table', but modified.")

    
(defvar gmpasm-font-lock-keywords
  (let ((keywords (copy-sequence m4-font-lock-keywords)))

    ;; Replace the comment regexp with one that doesn't have #, because the
    ;; asm comment starter might not be a # and will be dynamically
    ;; determined in the code below.
    (gmpasm-delete-from-list 'keywords
			     '("\\(\\b\\(m4_\\)?dnl\\b\\|^\\#\\).*$"
			       . font-lock-comment-face))
    (add-to-list 'keywords
		 '("\\b\\(m4_\\)?dnl\\b.*$" . font-lock-comment-face))

    ;; Some extra keywords for macros defined in asm-incl.m4.
    ;; L and LF don't look good fontified, so they're deliberately omitted.
    (add-to-list 'keywords
		 '("\\b\\(defframe\\|defframe_pushl\\|deflit\\|defreg\\|EPILOGUE\\|PROLOGUE\\)\\b"
		   . font-lock-keyword-face))
    
    ;; This is for $N parameters in m4 definitions, but it doesn't look good
    ;; on $N immediates or registers, so delete it.
    (gmpasm-delete-from-list 'keywords
			     '("\\$[*#@0-9]" . font-lock-variable-name-face))

    ;; With $N macro parameters not highlighted, it doesn't make much sense
    ;; to highlight $@ and $*, so delete them.
    (gmpasm-delete-from-list 'keywords
			     '("\\$\\@" . font-lock-variable-name-face))
    (gmpasm-delete-from-list 'keywords
			     '("\\$\\*" . font-lock-variable-name-face))

    keywords)
  "Default font-lock-keywords for `gmpasm-mode'.
Initialized from `m4-font-lock-keywords', but modified.
The right assembler comment regexp is added dynamically, buffer-local.")
 

;; Initialized if gmpasm-mode finds filladapt loaded.
(defvar gmpasm-filladapt-token-table nil
  "Filladapt token table used in `gmpasm-mode'.")
(defvar gmpasm-filladapt-token-match-table nil
  "Filladapt token match table used in `gmpasm-mode'.")
(defvar gmpasm-filladapt-token-conversion-table nil
  "Filladapt token conversion table used in `gmpasm-mode'.")


;;;###autoload
(defun gmpasm-mode ()
  "A major mode for editing GNU MP asm and m4 files.

This is `m4-mode' with some alterations and improvements.

Filladapt gets @ ! and dnl added buffer-local as fill prefixes (# and ; are
there already), but only if filladapt.el is already loaded.

`comment-start' and `comment-end' are set buffer-local for assembler style
commenting.  The first line found starting with # ; ! or @ determines
`comment-start'.  If there's no comments in the buffer, # is used.  If #
isn't what you want then type in a comment and do \\[gmpasm-mode] to
reinitialize.

`m4-mode-hook' is run when `gmpasm-mode' selects `m4-mode', and
`gmpasm-mode-hook' is run after `gmpasm-mode' has made its alterations.

\\{gmpasm-mode-map}"

  (interactive)

  (m4-mode)
  (setq major-mode 'gmpasm-mode
        mode-name  "gmpasm")

  (use-local-map gmpasm-mode-map)
  (set-syntax-table gmpasm-mode-syntax-table)
  (setq fill-column 76)

  ;; Don't want to find out the hard way whether some dumb assembler doesn't
  ;; like a missing final newline.
  (set (make-local-variable 'require-final-newline) t)

  ;; Paragraphs are separated by blank lines, or lines with only # ; ! @ or
  ;; dnl.
  (set (make-local-variable 'paragraph-separate)
       "[ \t\f]*\\(\\([#;!@]\\|dnl\\)[ \t]*\\)*$")
  (set (make-local-variable 'paragraph-start)
       (concat "\f\\|" paragraph-separate))
 
  ;; ! @ and dnl are comment style prefixes for adaptive fill, added to the
  ;; standard regexp (which has # and ; already).
  (set (make-local-variable 'adaptive-fill-regexp)
       "[ \t]*\\(\\([-|#;!@>*]+\\|dnl\\|(?[0-9]+[.)]\\)[ \t]*\\)*")
  (set (make-local-variable 'adaptive-fill-first-line-regexp)
       "\\`\\([ \t]*dnl\\)?[ \t]*\\'")

  (if (fboundp 'filladapt-mode)
      (progn
	(if (not gmpasm-filladapt-token-table)
	    (progn
	      (setq gmpasm-filladapt-token-table
		    filladapt-token-table)
	      (setq gmpasm-filladapt-token-match-table
		    filladapt-token-match-table)
	      (setq gmpasm-filladapt-token-conversion-table
		    filladapt-token-conversion-table)

	      ;; Add dnl, ! and @ as fill prefixes (# and ; are there
	      ;; already).
	      ;; Comments in filladapt.el say filladapt-token-table must
	      ;; begin with ("^" beginning-of-line), so put our addition
	      ;; second.
	      (gmpasm-add-to-list-second 'gmpasm-filladapt-token-table
					 '("dnl[ \t]\\|[!@]+" gmpasm-comment))
	      (add-to-list 'gmpasm-filladapt-token-match-table
			   '(gmpasm-comment gmpasm-comment))
	      (add-to-list 'gmpasm-filladapt-token-conversion-table
			   '(gmpasm-comment . exact))

	      ;; Numbered bullet points like "2.1" get matched at the start
	      ;; of a line when you're really trying to write something like
	      ;; "2.1 cycles/limb", so delete this from the list.  The
	      ;; regexp for "1.", "2." etc is left though.
	      (gmpasm-delete-from-list 'gmpasm-filladapt-token-table
				       '("[0-9]+\\(\\.[0-9]+\\)+[ \t]"
					 bullet))))

        (set (make-local-variable 'filladapt-token-table)
	     gmpasm-filladapt-token-table)
        (set (make-local-variable 'filladapt-token-match-table)
	     gmpasm-filladapt-token-match-table)
        (set (make-local-variable 'filladapt-token-conversion-table)
	     gmpasm-filladapt-token-conversion-table)))
 
  ;; The comment regexps above always have all of # ; ! and @, in case a
  ;; file has some weird combination of styles, but the first of these found
  ;; at the start of a line is used for comment-start.
  (set (make-local-variable 'comment-start)
       (save-excursion
	 (goto-char (point-min))
	 (if (re-search-forward "^[#;!@]" nil t)
	     (buffer-substring (1- (point)) (point))
	   "#")))
  (set (make-local-variable 'comment-end) "")

  ;; Comment fontification is added based on the comment-start determined.
  ;; The regexp requires whitespace before a comment-start so that m4 $#
  ;; doesn't match when comment-start is "#".
  (add-to-list (make-local-variable 'gmpasm-font-lock-keywords)
	       (cons (concat "\\(\\s-\\|^\\)"
			     (regexp-quote comment-start)
			     ".*$")
		     'font-lock-comment-face))

  (set (make-local-variable 'font-lock-defaults)
       '(gmpasm-font-lock-keywords nil))

  (run-hooks 'gmpasm-mode-hook))


(defun gmpasm-comment-region-dnl (beg end &optional arg)
  "(gmpasm-comment-region BEG END &option ARG)

Comment or uncomment each line in the region using `dnl'.
With \\[universal-argument] prefix arg, uncomment each line in region.
This is `comment-region', but using `dnl'."

  (interactive "r\nP")
  (let ((comment-start "dnl")
	(comment-end ""))
    (comment-region beg end arg)))


(provide 'gmpasm-mode)

;;; gmpasm-mode.el ends here
