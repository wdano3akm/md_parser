# md_parser
Translates (*some*) markdown to html, somewhat of a constant work in progress 

## Build Instruction
`make` will automatically compile gparser using gcc

## Run instruction
`./gparser $1 $2`
where `$1` is input file (markdown) and `$2` is output file (html), if you don't specify an output file the tool will print to `stdout`

## Config
Modify the field `prefix_path` in `config.h` to prepend a string to all media related paths
