#!/bin/bash

current_dir=$(pwd)
dir_show_option=''
current_command=''
arg1=''
arg2=''

bold=$(tput bold)
normal=$(tput sgr0)
green=$(tput setaf 2)
red=$(tput setaf 1)

if [ $# -gt 0 ]
then
    current_dir=$1
fi

print_current_dir()
{
    echo ${normal}${bold}$(pwd)
    echo ${normal}$(ls $dir_show_option $current_dir)
}

print_menu()
{
    print_current_dir
    show_mode=$([[ $dir_show_option = '' ]] && echo 'Short' || echo 'Full')
    echo ${bold}1${normal} - Show mode: ${green}$show_mode${normal}
    echo ${bold}2${normal} - Change directry
    echo ${bold}3${normal} - Copy file
    echo ${bold}4${normal} - Move/rename file
    echo ${bold}5${normal} - Delete file
    echo ${bold}6${normal} - Exit
}

get_command()
{
    arg1=''
    arg2=''
    read -p '$' current_command arg1 arg2
}

while [ "$current_command" != "6" ]; do
    print_menu
    get_command
    case $current_command in
        1)
            dir_show_option=$([[ $dir_show_option = '' ]] && echo '-a' || echo '')
            ;;
        2)
            if [[ -z $arg1 ]]
            then
                echo ${red}'Please provide directory'
            else
                current_dir=$arg1
            fi
            ;;
        3)
            if [[ -z $arg1 || -z $arg2 ]]
            then
                echo ${red}'Please provide source and destination filename'
            else
                cp $current_dir/$arg1 $current_dir/$arg2
            fi
            ;;
        4)
            if [[ -z $arg1 || -z $arg2 ]]
            then
                echo ${red}'Please provide source and destination filename'
            else
                mv $current_dir/$arg1 $current_dir/$arg2
            fi
            ;;
        5)
            if [ -z $arg1 ]
            then
                echo ${red}'Please provide filename'
            else
                rm $current_dir/$arg1
            fi
            ;;

    esac
done