#!/bin/bash

if [  "$1" = "--c"  ]; then
	echo "Let's compile first"
	g++ -o ../src/Reversi/Reversi ../src/Reversi/*.cpp ../src/Reversi/Game/*.cpp ../src/Reversi/MCTS/*.cpp ../src/Reversi/MiniMax/*.cpp ../src/Reversi/Network/*.cpp ../src/Reversi/Utils/*.cpp ../src/Reversi/DecisionMaker/*.cpp 
fi

ip=$(jq .ip "config.json")
port=$(jq .port "config.json")

moveOrdering=$(jq .moveOrdering "config.json")
mmDepth=$(jq .mmDepth "config.json")
mctsDepth=$(jq .mctsDepth "config.json")
kBest=$(jq .kBest "config.json")
total=$(jq .numberOfGames "config.json")


profile1=$(jq .profile[0].name "config.json")
limit1=$(jq .profile[0].limit "config.json")
prior1=$(jq .profile[0].prior "config.json")
cp1=$(jq .profile[0].cpV "config.json")
keepTree1=$(jq .profile[0].keepTree "config.json")

profile2=$(jq .profile[1].name "config.json")
limit2=$(jq .profile[1].limit "config.json")
prior2=$(jq .profile[1].prior "config.json")
cp2=$(jq .profile[1].cpV "config.json")
keepTree2=$(jq .profile[1].keepTree "config.json")

heurBoard1=$(jq .profile[0].boardWeight "config.json")
heurCorner1=$(jq .profile[0].cornerWeight "config.json")
heurMob1=$(jq .profile[0].mobilityWeight "config.json")
heurScore1=$(jq .profile[0].scoreWeight "config.json")

outWon=$(jq .outputWon "config.json")
outTime=$(jq .outputTimePerMove "config.json")

types=( $profile1 "MS" "MB")

for k in "${types[@]}"
do
  profile1=$k
  echo "I'm on profile ${profile1}"
  for (( j = 1; j <= 1; j++ ))
  do
    #mmDepth=$j
    for (( i = 1; i <= $total; i++ ))
      do
        var=$(date +"%FORMAT_STRING")
        now=$(date +"%H_%M_%S")
        today=$(date +"%Y-%m-%d")

        echo "Iteration #$i"
        echo "Let's start the server"
        gnome-terminal -- bash -c "./server -s -d ${mmDepth} 2>&1" # | tee logs/${today}_${now}_server.log"
  
        echo "Player 1 joines the game"
        if [  "${profile1}" = "\"trivial\"" ]; then
      	    gnome-terminal --wait -- bash -c "./ai_trivial 2>&1"
        else 
  	    gnome-terminal -- bash -c "../src/Reversi/Reversi --pl 1 --pr ${profile1} ${limit1} ${prior1} ${cp1} --p ${port} --i ${ip} --mmDepth ${mctsDepth} --kBest ${kBest} --moveOrdering ${moveOrdering} --keepTree ${keepTree1} --heur ${heurBoard1} ${heurCorner1} ${heurMob1} ${heurScore1} 2>&1 | tee logs/${today}_${now}_player1.log"
  	    #gnome-terminal -- bash -c "../src/Reversi/cpy/test 2>&1 | tee logs/${today}_${now}_playerCPY.log"
        fi
  
        echo "And here comes player 2"
        if [  "${profile2}" = "\"trivial\"" ]; then
    	    gnome-terminal --wait -- bash -c "./ai_trivial 2>&1"
        else 
           echo "Player 2"
  	   gnome-terminal --wait -- bash -c "../src/Reversi/Reversi --pl 2 --pr ${profile2} ${limit2} ${prior2} ${cp2} --p ${port} --i ${ip} --mmDepth ${mctsDepth} --kBest ${kBest} --moveOrdering ${moveOrdering} --keepTree ${keepTree2} 2>&1 | tee logs/${today}_${now}_player2.log"
        fi
    done
  done
  
  today=$(date +"%Y%-m%-d") 
  profile="${profile1//\"}"
  outW="${outWon//\"}"
  outT="${outTime//\"}"
  file="log_$profile$today.csv"
  python csvToLatex.py -c ${file} -i 14 -o ${outW} -oi ${mctsDepth} -oc ${profile} -s sum
  python csvToLatex.py -c ${file} -i 5 -o ${outT} -oi ${mctsDepth}  -oc ${profile} -s mean

  #mctsDepth=$((mctsDepth+50))
done

