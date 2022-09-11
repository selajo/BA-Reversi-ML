import logging
import csv
import os

class Logger(object):
	"""!
	Logs output
	"""
	def __init__(self, file):
		"""!
		Creates instance and sets log-file
		@param file logfile
		"""
		logging.basicConfig(filename=file, level=logging.DEBUG)

	def info(self, text):
		"""!
		Logs as info
		@param text info text
		"""
		logging.info(text)
	
	def debug(self, text):
		"""!
		Logs in debug mode
		@param text debug text
		"""
		logging.debug(text)
	
	def warning(self, text):
		"""!
		Logs as warning
		@param text warning text
		"""
		logging.warning(text)
	
class csvLogger(object):
	"""!
	Logs to csv file
	"""
	def __init__(self, file):
		"""!
		Creates instance and sets log-file
		@param file logfile
		"""
		self.filepath = file
	
	def info(self, text):
		"""!
		Logs text as row to csv
		@param text text as row for csv
		"""
		with open(self.filepath, 'a') as f:
			writer = csv.writer(f, delimiter=';')
			writer.writerow(text)
			
