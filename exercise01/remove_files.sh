#!/bin/bash

maxdepth='-maxdepth 1'
delete='-delete'
dir='.'

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
            echo -e 'SYNOPSIS'
            echo -e '\tremove_files [OPTIONS] [DIRECTORY]'
            echo -e 'OPTIONS'
            echo -e '\t-h, --help'
            echo -e '\t\t Shows help'
            echo -e '\t-r, --recursive'
            echo -e '\t\t Removes files in subdirectories'
            echo -e '\t-t, --test'
            echo -e '\t\t Shows only files to remove, without deletion'
            exit
            ;;
        -r|--recursive)
            maxdepth=''
            ;;
        -t|--test)
            delete=''
            ;;
        *)
            dir=$key
            ;;
    esac
    shift
done

find $dir $maxdepth -regextype posix-extended -regex '.*/([_~\-].*)?(.*\.tmp)?' $delete