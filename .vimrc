set tabstop=4
set softtabstop=4
set shiftwidth=4
set expandtab
set colorcolumn=110
set guifont=Monospace\ 20
highlight ColorColumn ctermbg=darkgrey
augroup project
    autocmd!
    autocmd BufRead,BufNewFile *h,*c set filetype=c.doxygen
augroup END
let &path.="./src"
