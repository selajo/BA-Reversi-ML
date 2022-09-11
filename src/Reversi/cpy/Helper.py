from datetime import datetime
import csv
import os.path

class Helper(object):
	"""!
	Holds important values for csv-files
	"""

	def __init__(self):
		self.mmDepth = 0
		self.movesPlayer = 0
		self.movesEnemy = 0
		self.edges = 0
		self.startTime = 0
		self.miniStartTime = 0
		self.timePerMove = 0
		self.totalTime = 0
		self.timePerMove = 0
	
	def setMMDepth(self, mm):
		"""!
		Sets depth for minimax search
		@param depth of minimax search
		"""
		self.mmDepth=mm
	
	def addMovePlayer(self):
		"""!
		Adds move to player count
		"""
		self.movesPlayer += 1
	
	def addMoveEnemy(self):
		"""!
		Adds move to enemy count
		"""
		self.movesEnemy += 1
	
	def startTimer(self):
		"""!
		Starts timer
		"""
		self.startTime = datetime.now()
	
	def endTimer(self):
		"""!
		Ends timer
		"""
		endTime = datetime.now()
		diff = endTime - self.startTime
		self.totalTime = diff.total_seconds() * 1000
	
	def startMiniTimer(self):
		"""!
		Starts mini timer. (used for time per move)
		"""
		self.miniStartTime = datetime.now()
	
	def endMiniTimer(self):
		"""!
		Ends mini timer. (used for time per move)
		"""
		endMiniTime = datetime.now()
		diff = endMiniTime - self.miniStartTime
		self.timePerMove += diff.total_seconds() * 1000
	
	def writeToCSV(self, filepath, endState, mctsDepth, points, edges):
		"""!
		Writes values to csv-file
		@param filepath name of csv-file
		@param endstate 1: player won, -1: else
		@param mctsDepth depth of mcts search
		@param points captured fields of player
		@param edges captured corners by player
		"""
		# create if file does not exist
		if not os.path.isfile(filepath):
			with open(filepath, 'a+') as f:
				writer = csv.writer(f, delimiter=';')
				writer.writerow(["MCTS-Tiefe", "MM-Tiefe", "#Zuege Player", "#Zuege Gegner", "D Zeit pro Spiel", "Spielzeit", "#Steine", "Edges", "Sieg"])
		
		#write data to csv
		with open(filepath, 'a') as f:
			writer = csv.writer(f, delimiter=';')
			writer.writerow([mctsDepth, self.mmDepth, self.movesPlayer, self.movesEnemy, self.timePerMove / self.movesPlayer, self.totalTime, points, edges, endState])
		
		
	
	
	
		
	
			
	
	
