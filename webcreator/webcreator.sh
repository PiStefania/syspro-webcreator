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
	else
		if [ "${args[2]}" -eq "0" ]; then
			echo "Too few websites to create"
			exit -2
		fi
	fi
	
	if ! [[ "${args[3]}" =~ ^[0-9]+$ ]]; then
        echo "${args[3]} is not an integer"
		exit -2
	else
		if [ "${args[3]}" -eq "0" ]; then
			echo "Too few pages to create"
			exit -2
		fi
	fi
}


#create web directories
check_dir(){
	if [ "$(ls -A ${args[0]})" ]; then
		>&2 echo "# Warning: directory is full, purging …"
		rm -rf ${args[0]}/*
	fi
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
	if [ -n "$arrPTemp" ]; then
		entriesInternal=($(shuf -i 0-$((${#arrPTemp[@]}-1)) -n $internalLinks))
	fi
	LIMITINTERNAL=${#entriesInternal[@]}
	for ((internal=0;internal < LIMITINTERNAL;internal++))
	{
		f+=(${arrPTemp[${entriesInternal[$internal]}]})
	}
	for ((elem=0;elem < ${#f[@]};elem++))
	do
		f[$elem]=$(echo ${f[$elem]} | cut -d"/" -f4)
	done

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
	if [ -n "$arrPExt" ]; then
		entriesExteral=($(shuf -i 0-$((${#arrPExt[@]}-1)) -n $externalLinks))
	fi
	LIMITEXTERNAL=${#entriesExteral[@]}
	for ((external=0;external < LIMITEXTERNAL;external++))
	{
		q+=(${arrPExt[${entriesExteral[$external]}]})
	}
	for ((elem=0;elem < ${#q[@]};elem++))
	do
		q[$elem]="../$(echo ${q[$elem]} | cut -d"/" -f3)/$(echo ${q[$elem]} | cut -d"/" -f4)"
	done
	
}

insert_content_page(){
	#get f+q
	combine=( "${f[@]}" "${q[@]}" )
	printf "<!DOCTYPE html>\n<html>\n	<body>\n"
	start=$k
	add=$(( m/(internalLinks+externalLinks) ))
	copyLines=$((m + k - add))
	end=$k
	link=0
	while [ "$start" -le "$copyLines" ];
	do
		found=0
		end=$(( add + end ))
		endpage=""$end"p"
		sed -n "$start,$endpage" ${args[1]} |
		while read line; do
		 	printf "\t\t$line\n"
		done
		
			#add links
		if [ -n "$combine" ]; then
			while [ "$found" -eq "0" ];
			do
				random=($( shuf -i 0-${#combine[@]} -n 1 ))
				if [ ! -z "${combine[$random]}" ]; then
					modifyStr=$(echo ${combine[$random]} | cut -d"." -f2)
					if [ "$modifyStr" = "html" ]; then
						ss=$(echo $page | cut -d"/" -f3)
						modifyStr="${args[0]}/$ss/${combine[$random]}"
					else
						ss=$(echo ${combine[$random]} | cut -d"/" -f2)
						sp=$(echo ${combine[$random]} | cut -d"/" -f3)
						modifyStr="${args[0]}/$ss/$sp"
					fi	
					>&2 echo "#     Adding link to $modifyStr"
					link=1
					printf "\t\t<a href="${combine[$random]}">	Link: ${combine[$random]}	</a>\n"
					unset combine[$random]
					combine=( "${combine[@]}" )
					found=1
				fi
			done
		fi
		
		start=$(( start + add ))
	done
	
	if [ "$link" -ne "1" ]; then
		links=0
	else
		links=1
	fi
	printf "\n	</body>\n</html>"
}

#create and handle page content
add_content_pages(){
	links=0
	LIMITW=${args[2]}
	LIMITP=${args[3]}
	startP=0
	for ((number=0;number <LIMITW;number++))
	{
		stringw="site$number"
		>&2 echo "# Creating web site $number …"
		mkdir "./${args[0]}/$stringw"
		for ((j=startP;j < $((LIMITP + startP)) ;j++))
		{
			page=${arrP[$j]}
			calc_content_page
			>&2 echo "#   Creating page $page with $m lines starting at line $k …"
			insert_content_page > $page
		}
		startP=$(( $startP + $LIMITP ))
	}
	
	if [ "$links" -eq "1" ]; then
		>&2 echo "# All pages have at least one incoming link"
	else
		>&2 echo "# Not all pages have an incoming link"
	fi
	>&2 echo "# Done."
}

check_arguments
check_correct_arguments
check_dir
save_pages
add_content_pages