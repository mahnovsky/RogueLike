colorscheme codedark
set encoding=utf-8
set fileencoding=utf-8
set guifont=Courier\ New:h18
set tabstop=4
set softtabstop=4
set shiftwidth=4
set expandtab
set colorcolumn=110
highlight ColorColumn ctermbg=darkgrey
augroup project
    autocmd!
    autocmd BufRead,BufNewFile *h,*c set filetype=c.doxygen
augroup END
let &path.="./src"