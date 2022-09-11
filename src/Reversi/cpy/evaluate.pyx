from config import config
from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils
from PyNode cimport Node
from PyMCTS cimport Algorithm
from PyMCTS cimport MCTS_Solver
from PyMCTS cimport MCTS
from PyMiniMax cimport MiniMax
from PyMiniMax cimport MiniMaxEndState
from PyMiniMax cimport AlphaBeta_ES
from PyMiniMax cimport AlphaBeta
from PyDecisionMaker cimport DecisionMaker

from PyNetwork cimport Network

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from model import model, ModelWrapper
from copy import deepcopy
import socket
from config import config
import numpy as np
import os
import sys, getopt
from logger import Logger, csvLogger
from datetime import datetime
from Helper import Helper


cdef createNumpy(char** moves):
	"""!
	Creates numpy from 2d array
	"""
	map = []
	for i in range(8):
		map.append([0 * j for j in range(8)])
	for i in range(8):
		for j in range(8):
			map[i][j] = moves[i][j] - 48
			if map[i][j] == 2:
				map[i][j] = -1

	map = np.array(map)
	return map
	
def reverse(input):
	"""!
	reverses game board: 1 <-> -1
	"""
	for i in range(64):
		if input[i] != 0:
			input[i] = -1 * (input[i] - 1)
	return input
	
cdef onlyMoves(psa_vector, vector[Point] moves):
	"""!
	Created psa_vector with given possible moves
	"""
	save = deepcopy(psa_vector)
	for i in range(64):
		psa_vector[i] = 0
	for move in moves:
		psa_vector[8 * move.y + move.x] = save[8 * move.y + move.x]

	return psa_vector
	
cdef char otherPlayer(player):
	"""!
	Calculates other play: 1 -> 2 vice versa
	"""
	if player == 49:
		return 50
	else:
		return 49

def printMap(iarray):
	"""!
	Prints Array as map
	"""
	for i in range(64):
		if i % 8 == 0 and i != 0:
			print("\n")
		print("{:.1f}".format(iarray[i]), " ", end = '')

cdef class MCTS_NN(object):
	"""!
	MCTS using values of neural network
	Based on: https://github.com/blanyal/alpha-zero/blob/master/mcts.py
	"""
	cdef MCTS * alg
	cdef Game game
	cdef Node * root
	cdef char player
	cdef char enemy
	cdef char ** imap
	cdef net
	cdef child_psas

	def __cinit__(self, net, char play, char enem):
		self.root = NULL
		self.game = Game(8)
		self.net = net
		self.player = play
		self.enemy = enem	
	
	cdef Node * search(self, Node* node, Game game, temperature, MCTS * i_alg):
		"""!
		Starts UCT-Search with values from neural network
		"""
		# mcts loop to get the best move
		cdef Node* child
		cdef Point move
		cdef Point action
		self.root = node
		self.game = game
		self.alg = i_alg
		curr_player = node.player
		self.player = curr_player
		if(curr_player == b'1'):
			self.enemy = b'2'
		else:
			self.enemy = b'1'
		self.root = new Node(NULL, game, node.player)
		i_game = self.game
		game.calcPossibleMoves(node.player)
		moves = game.getMoves()
		
		print("Starting MCTS search for player", node.player)
		for i in range(config.num_mcts_sims):			
			# expand if node is not a leaf -> potential optimization
			node = self.alg.treePolicy(self.root, moves)
			
			self.imap = node.g.getMap()
			arr = createNumpy(self.imap)
			print(arr)
			
			if node.player != self.player and self.player != 50:
				arr = -arr
			elif node.player == self.player and self.player == 50:
				arr = -arr
			
			if self.net == None:
				node = self.alg.expand(node, moves)

				print("Rollout")
				score = self.alg.defaultPolicy(node.g, node.player)
				print("Backup with score:", score)
				self.alg.backup(node, score)
				continue
			
			
			# get move probs and values from network for current state
			psa_vector, v = self.net.predict(arr)
			v = -v
			
			# add dirichlet noise to the psa_vector of the node
			psa_vector = self.add_dirichlet_noise(psa_vector)
			
			node.g.calcPossibleMoves(node.player)
			moves = node.g.getMoves() 
			
			# fill only moveable tiles
			psa_vector = onlyMoves(psa_vector, moves)
			
			psa_vector_sum = sum(psa_vector)
			
			# renormalize psa vector
			if psa_vector_sum > 0:
				psa_vector /= psa_vector_sum
				
			# sort moves
			if i == 0:
				self.child_psas = psa_vector
			
			print("For moves:")
			printMap(psa_vector)
			print("Predicted:", v)

			node = self.alg.expand(node, moves)
			node.psa = psa_vector[8 * node.lastUsed.y + node.lastUsed.x]
			print(node.psa)
			
			if node.g.checkForEndOfGame():
				if node.g.getPlayerCount(self.root.player) > 32:
					v = 1
				else:
					v = -1
				
			v = self.alg.defaultPolicy(node.g, node.player)
			
			print("Backup")
			self.alg.backup(node, v)
			
		highest_nsa = 0
		highest_index = 0		
		cp = 1.0
		if str(sys.argv[1]) == "--trainContinue":
			cp = config.c_puct
		else:
			cp = 0
		
		return self.alg.bestChild(self.root, cp)
	
	def add_dirichlet_noise(self, psa_vector):
		"""!
		Adds dirichlet noise to vector to get probabilities
		"""
		# Add Dirichlet noise to the psa_vector of the root node
		dirichlet_input = [config.dirichlet_alpha for x in range(64)]
		
		dirichlet_list = np.random.dirichlet(dirichlet_input)
		noisy_psa_vector = []
		
		for idx, psa in enumerate(psa_vector):
			noisy_psa_vector.append(
				(1 - config.epsilon) * psa + config.epsilon * dirichlet_list[idx])
		return noisy_psa_vector

	cdef getActionProb(self, Node * node, temp=1):
		"""!
		Calculates probabilities with current node and child nodes (for training)
		"""
		cdef Point move
		cdef Node * child
		
		counts = self.child_psas
		for i in range(64):
			counts[i] = 0
		
		for child in node.children:
			print(child.visits)
			counts[8 * child.lastUsed.y + child.lastUsed.x] = child.visits
			
		if temp == 0:
			bestAs = np.array(np.argwhere(counts == np.max(counts))).flatten()
			bestA = np.random.choice(bestAs)
			probs = [0] * len(counts)
			probs[bestA] = 1
			return probs
		
		counts = [x ** (1. / temp) for x in counts]
		countssum = float(sum(counts))
		probs = [x / countssum for x in counts]
		
		return probs

cdef class Evaluate(object):
	"""!
	Evaluates current_model with best model
	"""
	
	cdef Node * node
	cdef Node * best_child
	cdef char player
	cdef char enemy
	cdef Game game
	cdef MCTS_NN curr_mcts
	cdef MCTS_NN eval_mcts
	
	def __cinit__(self, current_mcts, eval_mcts):
		self.curr_mcts = current_mcts
		self.eval_mcts = eval_mcts
		
	def evaluate(self):
		"""!
		Evaluates current_model with best model
		based on: https://github.com/blanyal/alpha-zero/blob/master/train.py
		"""
		# play self-play games between the two networks and record game stats
		cdef char current_player 
		cdef DecisionMaker ds
		wins = 0
		losses = 0
		self.game = Game(8)
		cdef Point action
		cdef MCTS * alg_self = ds.setAlgorithm(config.mcts_type, b'1', b'2', 0.75, 0, 0) 
		cdef MCTS * alg_eval = ds.setAlgorithm(config.eval_mcts_type, b'2', b'1', 0.75, 0, 0)
		
		# self-play loop
		for i in range(config.num_eval_games):
			print("Start evaluation self-play, game:", i, "\n")

			#game_over = False
			value = 0
			current_player = b'1'
			self.player = current_player
			self.enemy = b'2'
			self.game = Game()
			
			# keep playing until the game is in a terminal state
			while not self.game.checkForEndOfGame():
				self.node = new Node(NULL, self.game, current_player)
				# mcts simulation to get best child
				# curr_mcts -> player 1
				# eval_mcts -> player 2
				if current_player == b'1':
					action = self.curr_mcts.search(game=self.game, node=self.node, temperature=config.temp_final, i_alg=alg_self).lastUsed
					next = self.enemy
				else:
					action = self.eval_mcts.search(game=self.game, node=self.node, temperature=config.temp_final, i_alg=alg_eval).lastUsed
					next = self.player
					
				self.game.calcPossibleMoves(current_player)
				self.game.makeMoveTo(action.x, action.y, current_player)
				
				print(current_player-48, "made move to", action.toString().decode("utf-8"))
				print(self.game.mapToString().decode("utf-8"))

				self.node.deleteFromRoot()
				if self.game.checkForMoves(next):
					current_player = next
			
			value = self.game.getPlayerCount(b'1')
			if value > 32:
				print("win")
				wins += 1
			elif value < 32:
				print("loss")
				losses += 1
			else:
				print("draw")
			print("\n")
			
		
		return wins, losses
		
		

cdef class Train(object):
	"""!
	Trains current model
	based on: https://github.com/blanyal/alpha-zero/blob/master/train.py
	"""
	#trains NN using MCTS
	cdef Node * node
	cdef Node * best_child
	cdef char player
	cdef char enemy
	cdef Game game
	cdef MCTS_NN current_mcts
	cdef MCTS_NN eval_mcts
	cdef eval_net
	cdef net
	cdef Algorithm * mcts
	cdef logfile
	cdef statistic
	
	def __cinit__(self, inet):
		self.game = Game()
		self.eval_net = ModelWrapper()
		self.net = inet
		
		self.current_mcts = MCTS_NN(net=self.net, play=49, enem=50)
		self.eval_mcts = MCTS_NN(net=self.eval_net, play=50, enem=49)
		self.logfile = "Train_" + str(datetime.now())
		self.statistic = ""
		
	
	def start(self):
		"""!
		Starts training process
		"""
		cdef Game game
		#main training loop
		log = csvLogger(sys.argv[2])

		#log.info(["Iteration:", str(sys.argv[3])])
		start = datetime.now()
		self.statistic = ""
		for i in range(config.num_iterations):
			print("Iteration", i + 1)
			
			training_data = [] # list to store self play states and scores
			
			game = Game()
			if config.use_MCTS_sims_4_train:
				self.play_sims(game)
			else:
				player = 49
				for j in range(config.num_games): #game loop
					print("Start training self-play game", j + 1)
					game = Game()
					self.play_game(game, training_data, player)
					player = otherPlayer(player)
			
			# save current model
			self.net.save_model()
			
		log.info(["Training time:", str(datetime.now()-start)])

	
	
	cdef evaluate(self, log):
		log = csvLogger(sys.argv[2])
		# load currently best model into evaluator
		self.eval_net.load_model("best_model")
		
		# initialize MCTS for both networks
		self.current_mcts = MCTS_NN(self.net, 49, 50)
		self.eval_mcts = MCTS_NN(self.eval_net, 50, 49)
			
			
		print("Starting evaluation")
		evaluator = Evaluate(self.current_mcts, self.eval_mcts)
		wins, losses = evaluator.evaluate()
			
		print("wins:", wins)
		print("losses:", losses)
			
		num_games = wins + losses
		
		win_rate = 0
		if num_games == 0:
			win_rate = 0
		else:
			win_rate = wins / num_games
			
		print("win rate:", win_rate)
			
		if win_rate > config.eval_win_rate:
			# save current model as new best model
			print("New model saved as best model.")
			self.net.save_model("best_model")
		else:
			# old model is still better -> discard new model
			print("New model discarded and previous model loaded.")
			self.net.load_model()
		
		log.info([str(sys.argv[3]), str(win_rate)])
	
	cdef play_sims(self, Game game):
		"""!
		Collect training date using MCTS simulations
		"""
		cdef Point action
		cdef DecisionMaker ds
		cdef MCTS * alg_self = ds.setAlgorithm(config.mcts_type, b'1', b'2', config.c_puct, 0, 0) 
		cdef Node * bestChild
		cdef char curr_player 
		self.game = Game()
		
		for ep in range(config.epochs):
			value = 0
			self_play_data = []
			training_data = []
			count = 0
			curr_player = b'1'
			#self.node = new Node(NULL, Game(8), curr_player)
			self.player = curr_player
			self.enemy = b'2'
		
			for t in range(config.max_MCTS_sims):
				# MCTS Sims
				print("It's player", curr_player-48,"'s turn")
				self.node = new Node(NULL, self.game, curr_player)
				print("New action from player", self.node.player)
				# get best child from MCTS simulation
				bestChild = self.current_mcts.search(game=self.game, node=self.node, temperature=config.temp_init, i_alg=alg_self)
				action = bestChild.lastUsed
				next = otherPlayer(curr_player)
				
				# store state, prob and v for training
				child_psas = self.current_mcts.child_psas
			
				print("Child_psas:", child_psas)
			
				self_play_data.append([deepcopy(createNumpy(game.getMap())),
						deepcopy(child_psas),
						float(bestChild.score)/float(bestChild.visits)])

				self.game.calcPossibleMoves(curr_player)
				self.game.makeMoveTo(action.x, action.y, curr_player)
				print(curr_player-48, "made move to", action.toString().decode("utf-8"))
				print(self.game.mapToString().decode("utf-8"))
			
				bestChild.deleteFromRoot()
				self.node.deleteFromRoot()
			
				count += 1
				if self.game.checkForEndOfGame():
					self.game = Game()
					break
			
				if self.game.checkForMoves(next):
					curr_player = next
				
			print("Reached end of simulation")
			#print(self_play_data)

			
			for game_state in self_play_data:
				self.augment_data(game_state, training_data, 8, 8)
			self.net.train2(training_data)
			
	
	cdef play_game(self, Game game, training_data, iplayer):
		"""!
		Collect training data using complete games
		"""
		training_data = []
		# loop for each self play game
		cdef Point action
		cdef DecisionMaker ds
		cdef MCTS * alg_self = ds.setAlgorithm(config.mcts_type, b'1', b'2', config.c_puct, 0, 0) 
		cdef MCTS * alg_eval = ds.setAlgorithm(config.eval_mcts_type, b'2', b'1', config.c_puct, 0, 0)
		#mcts = MCTS_NN(self.net)
		#game_over = False
		value = 0
		self_play_data = []
		count = 0
		cdef char curr_player = b'1'
		self.node = new Node(NULL, Game(8), curr_player)
		self.player = iplayer
		self.enemy = otherPlayer(self.player)
		self.game = game

		
		# play until the game is in a terminal state
		print("Starting new game")
		t = 0
		while True:
			# MCTS Sims

			print("It's player", curr_player-48,"'s turn")
			self.node = new Node(NULL, self.game, curr_player)
			print("New action from player", self.node.player)
			# get best child from MCTS simulation
			bestChild = self.current_mcts.search(game=self.game, node=self.node, temperature=config.temp_init, i_alg=alg_self)
			action = bestChild.lastUsed
			next = otherPlayer(curr_player)
			
			# store state, prob and v for training
			temp = int(t < config.temp_thresh)
			child_psas = self.current_mcts.getActionProb(self.current_mcts.root, temp)
		
			print("Child_psas:")
			printMap(child_psas)
			
			imap = createNumpy(self.game.getMap())
			if curr_player != self.player and t != 0:
				imap = -imap
			print(imap)
		
			self_play_data.append([deepcopy(imap),
					deepcopy(child_psas),
					curr_player-48])

			self.game.calcPossibleMoves(curr_player)
			self.game.makeMoveTo(action.x, action.y, curr_player)
			print(curr_player-48, "made move to", action.toString().decode("utf-8"))
			print(self.game.mapToString().decode("utf-8"))
			
			bestChild.deleteFromRoot()
			self.node.deleteFromRoot()
			count += 1
			t += 1
			if self.game.checkForEndOfGame():
				break
				
			if self.game.checkForMoves(next):
				curr_player = next
		
		print("Reached end of game")

		value = game.getPlayerCount(self.player)
		score = 0
		# check end state
		if value > 32:
			score = 1
		else:
			score = -1
		
		for game_state in self_play_data:
			if game_state[2] == self.player-48: 
				game_state[2] = score
			else:
				game_state[2] = -score
			self.augment_data(game_state, training_data, 8, 8)
		print(training_data)
		self.net.train(training_data)
	
	def augment_data(self, game_state, training_data, row, column):
		# append newly learned to training data
		state = deepcopy(game_state[0])
		psa_vector = deepcopy(game_state[1])
		
		training_data.append([state, psa_vector, game_state[2]])
		
cdef class client(object):
	"""!
	Connects to sever to play games
	"""
	cdef Game game
	cdef char player
	#cdef Algorithm * mcts
	def __cinit__(self, char player):
		cdef Game game
		print("Set new game")
		self.game = Game()
		self.player = player
	
	def evalMessage(self, message, helper):
		"""!
		Evaluates message from server
		"""
		mtype = message[0]
		if mtype == 2:
			print("Read map")
		elif mtype == 3:
			self.game.setPlayer(message[5])
			self.player = message[5] + 48
			print("Set player:", self.player)
		elif mtype == 4:
			print("I need to make a move")
			return 4
		elif mtype == 6:
			player = 48 + message[10]
			x = message[6]
			y = message[8]
			print(player, "makes move to", x, " ", y)
			self.game.calcPossibleMoves(player)
			self.game.makeMoveTo(x, y, player)
			print(self.game.mapToString().decode("utf-8"))
			
			if player == self.player:
				helper.addMovePlayer()
			else:
				helper.addMoveEnemy()
			
		elif mtype == 7:
			return 7
		elif mtype == 8 or mtype == 9:
			return 8
		return 0

	cdef tuple gameLoop(self, ip, port, net, MCTS * mcts, helper):
		"""!
		Holds connection and sends moves
		"""
		cdef MCTS * alg = mcts
		cdef Point p
		cdef MCTS_NN current_mcts = MCTS_NN(net=net, play=50, enem=49)
		cdef Node * node
		serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		serversocket.connect((ip, port))
		
		#send first message with group-nr
		str = bytearray([1, 0, 0, 0,1 ,1])
		serversocket.send(str);
		
		helper.startTimer()
		status = 0
		while(True):
			answer = serversocket.recv(1024)
			
			ev = self.evalMessage(answer, helper)
			
			if ev == 4:
				# calulate
				helper.startMiniTimer()
				
				alg.mmDepth = answer[9]
				helper.setMMDepth(alg.mmDepth)
				#p = alg.uctSearch(self.game, alg.mctsDepth);
				node = new Node(NULL, self.game, self.player)
				p = current_mcts.search(game=self.game, node=node, temperature=config.temp_final, i_alg=alg).lastUsed
				print("Calculated:", p.toString())
				node.deleteFromRoot()
				move = bytearray([5, 0, 0, 0, 5, 0, p.x, 0, p.y, 0])
				
				helper.endMiniTimer()
				
				serversocket.send(move)
			elif ev == 7 or ev == 8:
				status = ev
				break
			else:
				continue
		print("Close connection")
		serversocket.close()
		helper.endTimer()
		
		score = self.game.getPlayerCount(self.player)	
		
		if score > 32:
			status = 1
		elif score < 32:
			status = -1
		
		return (status, score)
		
	cdef int calcEdges(self):
		"""!
		Calculates numbers of edges captured by player
		"""
		cdef char** map
		map = self.game.getMap()
		score = 0
		if map[0][0] == self.player:
			score += 1
		if map[0][7] == self.player:
			score += 1
		if map[7][0] == self.player:
			score += 1
		if map[7][7] == self.player:
			score += 1
			
		return score
"""
cdef Point * p = new Point(1,2)
p.addDirection(4)
print(str(p.toString()))

Utils.printHelp()
cdef Game * g = new Game(8)
print(g.checkForEndOfGame())
g.calcPossibleMoves(ord('1'))
print(g.possibleMovesToString(ord('1')).decode("utf-8"))
g.makeMoveTo(4, 2, ord('1'))
print(g.mapToString().decode("utf-8"))


cdef Node * n = new Node(NULL, Game(8), ord('1'))

#cdef AbstractMCTS * ms = new MCTS_Solver('1', '2', 1.0)
#print ms.uctSearch(Game(8), 10).toString()

#cdef MCTS_MR * mr = new MCTS_MR('1', '2', 1.0)
#print mr.uctSearch(Game(8), 10).toString()
cdef DecisionMaker * dc = new DecisionMaker()
cdef Algorithm * mc = dc.setAlgorithm(b"Solver", ord('1'), ord('2'), 1.0, 2, 2)
print(mc.uctSearch(Game(8), 20).toString().decode("utf-8"))		
"""	
cdef Network nw
cdef MCTS * imcts
cdef DecisionMaker ds
cdef client iclient = client(50)
def main():
	helper = Helper()
	if not os.path.exists(sys.argv[2]):
		log = csvLogger(sys.argv[2])
		log.info(["Learning rate:", str(config.learning_rate)])
		log.info(["Number of total iterations:", str(config.num_iterations)])
		log.info(["Dirichlet noise alpha:", str(config.dirichlet_alpha)])
		log.info(["Batch size:", str(config.batch_size)])
		log.info(["Training games total:", str((config.num_games * config.num_iterations))])
		log.info(["Thinking time (simulations):", str(config.max_MCTS_sims * config.num_mcts_sims)])

	# if best model exists, use it
	if str(sys.argv[1]) == "--trainStart":
		print("Starting to train")
		net = ModelWrapper()
		if config.load_model:
			file_path = config.model_directory + "best_model.meta"
			if os.path.exists(file_path):
				net.load_model("best_model")
			else:
				print("Trained model doesn't exist. Starting from scratch.")
		else:
			print("Trained model not loaded. Starting from scratch.")
		
		train = Train(net)
		train.start()	
	
	# continue training for current model
	elif str(sys.argv[1]) == "--trainContinue":
		print("Starting to train")
		net = ModelWrapper()
		
		file_path = config.model_directory + "current_model.meta"
		if os.path.exists(file_path):
			net.load_model("current_model")
			print("Loading existing model.")
		else:
			print("Trained model doesn't exist. Starting from scratch.")

		
		train = Train(net)
		train.start()	
		
	# start evaluation: best vs current
	elif str(sys.argv[1]) == "--evaluation":
		net = ModelWrapper()
		
		file_path = config.model_directory + "current_model.meta"
		if os.path.exists(file_path):
			net.load_model("current_model")
		
		file_path = config.model_directory + "best_model.meta"
		if os.path.exists(file_path):
			print("Best model exists already")
		else:
			print("There is no currently best model. Saving current as best.")
			net.save_model("best_model")
			return 0
			
		log = csvLogger(sys.argv[2])
		train = Train(net)
		train.evaluate(log)
	#starts online play
	else:
		log = csvLogger(sys.argv[2])
		print("Didn't get any parameters")
		net = ModelWrapper()
		if config.load_model:
			file_path = config.model_directory + "current_model.meta"
			if os.path.exists(file_path):
				net.load_model("current_model")
				
		# play via server
		if config.online:
			if config.onlyMCTS == 1:
				net = None
			print("Let's play online.")
			imcts = ds.setAlgorithm(config.mcts_type, b'2', b'1', config.c_puct, 3, 10)
			imcts.mctsDepth = 100
			ret = iclient.gameLoop(config.ip, config.port, net, imcts, helper)
			score = ret[1]
			ret = ret[0]
		
			if ret == 7:
				print("I got disqualified")
			else:
				print("Reached the end of game")
				if ret == 1:
					print("I won!")
				elif ret == -1:
					ret = 0
					print("I lost...")
				else:
					ret = 0.5
					print("It's a draw.")
				log.info([str(sys.argv[3]), ret])
				helper.writeToCSV("NN_"+ str(sys.argv[2]), ret, config.num_mcts_sims, score, iclient.calcEdges())
			
		

if __name__ == '__main__':
	main()
		
					
