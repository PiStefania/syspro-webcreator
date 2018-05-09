#!/bin/bash

args=("$@")

#if all arguments given
check_arguments (){
	if [ "${args[0]}" == "" ]; then
		echo "No parameters given"
		exit -1
	elif [ "${args[1]}" == "" ]; then
		echo "Need text_file"
		exit -1
	elif [ "${args[2]}" == "" ]; then
		echo "Need no of websites"
		exit -1
	elif [ "${args[3]}" == "" ]; then
		echo "Need no of sites per website"
		exit -1
	else
		echo "All parameters given"
	fi
}


#if arguments are correct
check_correct_arguments (){
	if [ ! -d "${args[0]}" ]; then
		echo "Directory doesn't exist"
		exit -2
	fi
	
	if [ ! -e "${args[1]}" ]; then	
		echo "Text file doesn't exist"
		exit -2
	else 
		lines=$( wc -l < "${args[1]}" )
		if [ "$lines" -lt 10000 ]; then
			echo "Lines of file are $lines, need 10000<="
			exit -2
		fi
	fi
	
	if ! [[ "${args[2]}" =~ ^[0-9]+$ ]]; then
        echo "${args[2]} is not an integer"
		exit -2
	fi
	
	if ! [[ "${args[3]}" =~ ^[0-9]+$ ]]; then
        echo "${args[3]} is not an integer"
		exit -2
	fi
}


#create web directories
create_websites(){
	LIMIT=${args[2]}-1
	for ((number=0;number <= LIMIT;number++))
	{
		string="site$number"
		mkdir "./${args[0]}/$string"
	}
}

#create and handle page content
create_page_content(){
	echo "content"
}

#create pages in each website
create_pages(){
	LIMITW=${args[2]}-1
	LIMITP=${args[3]}-1
	for ((i=0;i <= LIMITW;i++))
	{
		stringw="site$i"
		for ((j=0;j <= LIMITP;j++))
		{
			stringp="page"$i"_$RANDOM.html"
			echo 'Hello, world.' > "./${args[0]}/$stringw/$stringp"
		}
	}
}

check_arguments
check_correct_arguments
create_websites
create_pages