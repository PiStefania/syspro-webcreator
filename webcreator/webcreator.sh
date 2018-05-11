#!/bin/bash

args=("$@")
arrExternal=()
arrInfo=()
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
addinfo_page_content(){
	#printf "<!DOCTYPE html>\n<html>\n	<body>\n"
	#calc random k
	lines=$( wc -l < "${args[1]}" )
	LIMITK=$(($lines-2000))
	DIFFK=$((LIMITK-1+1))
	k=$(($(($RANDOM%$DIFFK))+1))
	#calc random m
	DIFFM=$((2000-1000+1))	
	m=$(($(($RANDOM%$DIFFM))+1000))
	#calc internal links
	internalLinks=$(((${#arrPTemp[@]}/2)+1))
	f=()
	entries=($(shuf -i 0-$((${#arrPTemp[@]}-1)) -n $internalLinks))
	LIMITINTERNAL=${#entries[@]}
	for ((internal=0;internal < LIMITINTERNAL;internal++))
	{
		f+=(${arrPTemp[${entries[$internal]}]})
	}
	
	temp=("/$stringw/${arrP[$j]}" "$k" "$m" "$f")
	arrInfo+=("$temp")
	#printf "		word1 word2 … <a href=”link1”>link1_text</a>\n"
	#printf "		wordn wordn+1 … <a href=”link2”>link2_text</a>"
	#printf " wordm wordm+1 ...\n"
	
	
	#printf "	</body>\n</html>"
}

add_external_info(){
	array1=(name1 name2)
	name1=(one two)
	name2=(red blue)


	for name in "${array1[@]}"
	do
	  typeset -n nameref="$name"
	  for value in "${nameref[@]}"
	  do
		printf '%s\n' "$name - $value"
	  done
	done
}

#create pages in each website
handle_pages(){
	LIMITW=${args[2]}-1
	LIMITP=${args[3]}-1
	zero=0
	for ((i=0;i <= LIMITW;i++))
	{
		stringw="site$i"
		arrP=()
		for ((j=0;j <= LIMITP;j++))
		{
			stringp="page"$i"_$RANDOM.html"
			arrP+=($stringp)
			arrPTemp+=($stringp)
		}
		for ((j=0;j <= LIMITP;j++))
		{
			#delete same page from internal links
			if (("$LIMITP" > "$zero"));  then
				unset arrPTemp[$j]
				arrPTemp=( "${arrPTemp[@]}" )
			fi
			#create_page_content > "./${args[0]}/$stringw/${arrP[$j]}"
			addinfo_page_content
			arrExternal+=("/$stringw/${arrP[$j]}")
			arrPTemp=("${arrP[@]}")
		}
	}
}

check_arguments
check_correct_arguments
create_websites
handle_pages
add_external_info