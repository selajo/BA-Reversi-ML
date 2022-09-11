if [  "$1" = "--start"  ] && [ $# -eq 3 ]; then
	echo "Let's compile first"
	(cd ../src/Reversi/ && make)
	
	for (( j = 1; j <= $2; j++ ))
	do 
		#first call
		gnome-terminal --wait -- bash -c "../src/Reversi/cpy/cpyReversi --trainStart ${3} ${j} 2>&1 | tee logs/train.log"
		#for (( i = 2; i <= $2; i++ ))
		#do
		#	gnome-terminal --wait -- bash -c "../src/Reversi/cpy/cpyReversi --trainContinue"
		#done
		gnome-terminal --wait -- bash -c "../src/Reversi/cpy/cpyReversi --evaluation ${3} ${j} 2>&1 | tee logs/eval.log"
	done
elif [  "$1" = "--continue"  ] && [ $# -eq 3 ]; then
	echo "Let's compile first"
	(cd ../src/Reversi/ && make)
	
	for (( j = 1; j <= $2; j++ ))
	do 
		#continue training
		gnome-terminal --wait -- bash -c "../src/Reversi/cpy/cpyReversi --trainContinue ${3} ${j} 2>&1 | tee logs/train.log"
		
		if [ `expr $j % 3` -eq 0 ]; then
    		
    			for (( i = 1; i <= 50; i++ ))
    			do 
    				gnome-terminal -- bash -c "./server -s -d 3 2>&1 | tee logs/${today}_${now}_server.log"

				#gnome-terminal -- bash -c "../src/Reversi/Reversi --pl 1 --pr MCTS 20 10 0.75 --p 7777 --i '127.0.0.1' --mmDepth 50 --kBest 0 --moveOrdering false --keepTree false --heur 0 0 0 0 2>&1 | tee logs/player1.log"
				gnome-terminal -- bash -c "../src/Reversi/cpy/cpyReversi --online ${3} ${i} 2>&1 | tee logs/online.log"
				gnome-terminal --wait -- bash -c "./ai_trivial 2>&1"
			done 
    		
		fi

		
	done
elif [  "$1" = "--online"  ]; then
	(cd ../src/Reversi/ && make)
	echo "Starting online play"
	mmDepth=1
	for (( m = 1; m <= 5; m++ ))
	do
		mmDepth=$m
		for (( j = 1; j <= $2; j++ ))
		do 
			gnome-terminal -- bash -c "./server -s -d ${mmDepth} 2>&1" # | tee logs/${today}_${now}_server.log"
	
			gnome-terminal -- bash -c "../src/Reversi/cpy/cpyReversi --online ${3} ${j} 2>&1 | tee logs/online.log"
	
			gnome-terminal --wait -- bash -c "./ai_trivial 2>&1"
		done
	done
fi

