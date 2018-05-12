#!/bin/bash

args=("$@")
arrP=()
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


#create pages in each website
save_pages(){
	LIMITW=${args[2]}-1
	LIMITP=${args[3]}-1
	for ((i=0;i <= LIMITW;i++))
	{
		stringw="site$i"
		for ((j=0;j <= LIMITP;j++))
		{
			stringp="page"$i"_$RANDOM.html"
			stringWhole="./${args[0]}/$stringw/$stringp"
			arrP+=($stringWhole)
		}
	}
}

calc_content_page(){
	#get page
	specificSite=$(echo $page | cut -d"/" -f3)
	specificPage=$(echo $page | cut -d"/" -f4)
	
	#calc random k
	lines=$( wc -l < "${args[1]}" )
	LIMITK=$(($lines-2000))
	DIFFK=$((LIMITK-1+1))
	k=$(($(($RANDOM%$DIFFK))+1))
	
	#calc random m
	DIFFM=$((2000-1000+1))	
	m=$(($(($RANDOM%$DIFFM))+1000))
	
	#calc internal links
	arrPTemp=()
	for pageInternal in ${arrP[@]}
	do
		if [[ $pageInternal = *"$specificSite"* ]]; then
	 		arrPTemp+=($pageInternal)
		fi
	done
	
	for ((i=0;i < ${#arrPTemp[@]};i++)) 
	{
	   if [[ "${arrPTemp[$i]}" = "$page" ]]; then
		   unset arrPTemp[$i]
		   arrPTemp=( "${arrPTemp[@]}" )
	   fi
	}
	internalLinks=$(((${#arrPTemp[@]}/2)+1))
	f=()
	entriesInternal=($(shuf -i 0-$((${#arrPTemp[@]}-1)) -n $internalLinks))
	LIMITINTERNAL=${#entriesInternal[@]}
	for ((internal=0;internal < LIMITINTERNAL;internal++))
	{
		f+=(${arrPTemp[${entriesInternal[$internal]}]})
	}

	#calc external links
	arrPExt=()
	for pageExternal in ${arrP[@]}
	do
		if [[ $pageExternal != *"$specificSite"* ]]; then
	 		arrPExt+=($pageExternal)
		fi
	done
	externalLinks=$(((${#arrPExt[@]}/2)+1))
	q=()
	entriesExteral=($(shuf -i 0-$((${#arrPExt[@]}-1)) -n $externalLinks))
	LIMITEXTERNAL=${#entriesExteral[@]}
	for ((external=0;external < LIMITEXTERNAL;external++))
	{
		q+=(${arrPExt[${entriesExteral[$external]}]})
	}		
}

insert_content_page(){
	printf "<!DOCTYPE html>\n<html>\n	<body>\n"
	start=$k
	while [ "$start" -le "$lines" ];
	do
		end=$((($m/(internalLinks+externalLinks))+$start))
		endpage=""$end"p"
		sed -n "$start,$endpage" ${args[1]} |
		while read line; do
		 	printf "\t\t$line\n"
		done
		
		#add links
		
		start=$end
	done
	printf "\n	</body>\n</html>"
}

#create and handle page content
add_content_pages(){

	for page in ${arrP[@]}
	do
		calc_content_page
		insert_content_page > $page
	done
}

check_arguments
check_correct_arguments
create_websites
save_pages
add_content_pages