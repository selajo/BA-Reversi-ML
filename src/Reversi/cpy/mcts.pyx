#cython: language_level=3

from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils
from PyNode cimport Node
from PyMCTS cimport Algorithm
from PyMCTS cimport MCTS_Solver
from PyMiniMax cimport MiniMax
from PyMiniMax cimport MiniMaxEndState
from PyMiniMax cimport AlphaBeta_ES
from PyMiniMax cimport AlphaBeta
from PyDecisionMaker cimport DecisionMaker

from config import config
from model import model
from copy import deepcopy

import numpy as np


cdef class MCTS_NN(object):
	#MCTS using a neural network for move prediction
	cdef MCTS_Solver * alg
	cdef Game game
	cdef Node * root
	cdef char player
	cdef char enemy
	cdef char ** imap

	def __cinit__(self, net, char play, char enem):
		self.root = NULL
		self.game = Game(8)
		self.net = net
		self.player = play
		self.enemy = enem
		
	def createNumpy(self):
		arr = [ [self.moves[0]], [self.moves[1]], 
			[self.moves[2]], [self.moves[3]],
			[self.moves[4]], [self.moves[5]],
			[self.moves[6]], [self.moves[7]] ]
		return arr
	
	
	cdef public Node * search(self, Game game, Node * node, temperature, MCTS_Solver * i_alg):
		cdef Node* child
		self.root = node
		self.game = game
		self.alg = i_alg
		
		
		for i in range(config.num_mcts_sims):
			node = self.root
			i_game = self.game
			
			# expand if node is not a leaf
			node.g.calcPossibleMoves(self.player)
			moves = node.g.getMoves()
			while node.children.size() > 0:
				node = self.alg.treePolicy(node, moves)
			
			
			self.imap = game.getMap()
			arr = self.createNumpy()
			test = self.moves[0][0]
			psa_vector, v = self.net.predict(arr)
			#x = np.array(game.getMap(), np.int32)
			#psa_vector, v = self.net.predict(x)
			
			if node.parent is NULL:
				psa_vector = self.add_dirichlet_noise(self.player)
			
			node.g.calcPossibleMoves(self.player)
			moves = node.g.getMoves()
			
			#for idx, move in enumerate(moves):
			#	if move[0] is 0:
			#		psa_vector[idx] = 0
			
			psa_vector_sum = sum(psa_vector)
			
			# renormalize psa vector
			if psa_vector_sum > 0:
				psa_vector /= psa_vector_sum
			
			node = self.alg.expand(node, moves)
			
			score = self.alg.defaultPolicy(node.g, node.player == self.player)
			
			self.alg.backup(node, score)
			
		highest_nsa = 0
		highest_index = 0		
		
		i = 0
		for child in self.root.children:
			temperature_exponent = int(1 / temperature)
			
			if child.score ** temperature_exponent > highest_nsa:
				highest_nsa = child.score ** temperature_exponent
				highest_index = i
			i += 1
		
		return self.root.children[highest_index]
	
	def add_dirichlet_noise(self, psa_vector):
		# Add Dirichlet noise to the psa_vector of the root node
		dirichlet_input = [config.dirichlet_alpha for x in range(config.action_size)]
		
		dirichlet_list = np.random.dirichlet(dirichlet_input)
		noisy_psa_vector = []
		
		for idx, psa in enumerate(psa_vector):
			noisy_psa_vector.append(
				(1 - config.epsilon) * psa + config.epsilon * dirichlet_list[idx])
		return noisy_psa_vector
